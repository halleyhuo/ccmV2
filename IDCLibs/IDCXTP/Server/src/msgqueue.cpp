/*
* msgqueue.cpp
*
* This is for the IPC support
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h> 
#include <sys/stat.h>
#include <pthread.h>
#include <sys/resource.h>

#include "Types.h"
#include "XtpServer.h"
#include "msgqueue.h"
#include "IDCLog/inc/LogTrace.h"
#include "IDCBase/inc/IDCMsgQueue.h"

static msgq_info mq_info;
conn_info conn_list[MAX_CONN_NUM];

static int32 own_chid = -1;

const rlim_t MAX_MQ_SZ = 81920000;
const rlim_t MIN_MQ_SZ = 819200;


static bool ChangeMQLimit( const int32 newSize ){
	rlimit old_limit;
	bool rc = false;
	if( -1 == getrlimit(RLIMIT_MSGQUEUE, &old_limit) ){
		perror("getrlimit: ");
	}else{
		if( (MAX_MQ_SZ<=old_limit.rlim_max)||(MAX_MQ_SZ<=old_limit.rlim_cur) ){
#ifdef DEBUGLINUXADAPTER
			printf(">>>Warning: Cur MQ SZ is already reaches MAX\n");
#endif

		}else if( ((MIN_MQ_SZ>=old_limit.rlim_max)||(MIN_MQ_SZ>=old_limit.rlim_cur)) &&
			(newSize<0) ){
#ifdef DEBUGLINUXADAPTER
				printf(">>>Warning: Cur MQ SZ is already reaches MIN\n");
#endif                  
		}else{
			rlimit new_limit; 
			new_limit.rlim_max = MAX_MQ_SZ;
			new_limit.rlim_cur = new_limit.rlim_max;

			if( -1==setrlimit(RLIMIT_MSGQUEUE, &new_limit) ){
				perror("setrlimit: ");
			}else{
#ifdef DEBUGLINUXADAPTER
				printf(">>>set new MQ Size Limit Scussfully, sz change from %d -> %d\n",
					old_limit.rlim_max,MAX_MQ_SZ);
				rc = true;
#endif
			}
		}
	}
	return rc;    
}

void msg_q_init(int32 count, int32 size )
{
	mq_info.mq        = -1;
	mq_info.ErrId     = IDC_OK;
	//mq_info.ErrMsg    = "";
	mq_info.Count     = count;
	mq_info.BufSize   = size;
	mq_info.pBuf      = (char*)malloc((size_t)mq_info.BufSize);

	memset(&conn_list[0], 0, sizeof(conn_info)*MAX_CONN_NUM);
}

bool msgq_create(int32 count, int32 size)
{
	struct mq_attr def_attr, act_attr, tmp_attr;
	uint32 prio;               // Priority
	msg_q_init(count, size);


	memset(&mq_info.mqName[0], 0, 30);
	memcpy(&mq_info.mqName[0], xtpServerPath, 15);

	//if a channel with same name exists delete it
	// wait 5s if queue is used
	for(int32 i=0;i<50;i++)
	{
		mq_info.mq = mq_open (&mq_info.mqName[0], O_RDONLY | O_NONBLOCK);
		if (mq_info.mq!=-1)
		{
			//printf(">>>try to close mqueue %s, try cnt %d\n",mq_info.mqName[0],i);
			mq_unlink (mq_info.mqName);
			IDC_DELAY(100);
		}
		else
			break;
	}

	def_attr.mq_maxmsg = mq_info.Count;
	def_attr.mq_msgsize = mq_info.BufSize;
	def_attr.mq_flags = 0;

	ChangeMQLimit(MAX_MQ_SZ);	

	//Blocking call. Some times write to self(Exit CMD if Client Lost Connection) is necessary
	if ( ( mq_info.mq = mq_open( &mq_info.mqName[0], O_RDWR | O_CREAT | O_TRUNC , 0777, &def_attr ) ) < 0 )
	{
		perror( mq_info.mqName );
		return false;
	}

	// Get the attributes
	mq_getattr (mq_info.mq, &act_attr);

	if (act_attr.mq_curmsgs != 0) {

		// There are some messages on this queue....eat em

		tmp_attr.mq_maxmsg = mq_info.Count;
		tmp_attr.mq_msgsize = mq_info.BufSize;
		tmp_attr.mq_flags = O_NONBLOCK;

		// First set the queue to not block any calls
		//attr.mq_flags = O_NONBLOCK;
		mq_setattr (mq_info.mq, &tmp_attr, NULL);

		// Now eat all of the messages
		while (mq_receive (mq_info.mq, mq_info.pBuf, mq_info.BufSize, &prio) != -1)
		{
//			IDC::Log(IDC::LOG_DEBUG, LOG_TAG,"Received a message with priority %d.\n", prio);
		}

		// The call failed.  Make sure errno is EAGAIN
		if (errno != EAGAIN) {
			perror ("mq_receive()");
			(void)kill(getpid(),SIGINT);
		}

		// Now restore the attributes
		mq_setattr (mq_info.mq, &def_attr, NULL);
	}

	//Just in case
	/*if( (old_attr.mq_maxmsg  != attr.mq_maxmsg) ||
	(old_attr.mq_msgsize != attr.mq_msgsize) )
	{
	mq_setattr (mq_info.mq, &attr, &old_attr);
	}*/

	return true;
}

int32 msgq_destroy()
{
	if ( mq_info.mq > 0 )
	{
		mq_close( mq_info.mq );
		//delete if exists
		if( mq_unlink( mq_info.mqName ) < 0 )
		{
			perror( mq_info.mqName );
			return 0;
		}
	}
	return 1;
}

int32 msgq_recv(void *message)
{
	int32 ret = 0;
	uint32    mprio;
	ssize_t     msize;

	msize = mq_receive( mq_info.mq, mq_info.pBuf, mq_info.BufSize, &mprio );


	if ( msize == -1 && errno != EAGAIN )
	{
		perror( "mq_receive\n" );
		ret = -1;
	}//else if( msize <= mq_info.BufSize )

	memcpy(message, mq_info.pBuf, msize);

	ret = msize;

	return ret;
}

//Process ID is App's 
//Channel ID is the fd of fifo
static int32 conn_channel_get(int32 pid, int32 chid)
{
	int32 conn = -1;
	int32 i = 0;

	if( pid <= 0 || chid <= 0 )
		return -1;

	//find connection
	for(i=0; i<MAX_CONN_NUM; i++)
	{
		if( conn_list[i].pid == pid &&
			conn_list[i].chid == chid    )
		{
			conn = i;
			break;
		}
	}
	return conn;
}

//Get Channel (not Channle Index) based on the NodeID, -1 indicates no found 
CIDCMsgQueue* conn_channel_get(const uint8 NodeID)
{
	CIDCMsgQueue* connChnnel = NULL;

	if( NodeID != 0 )
	{
		//find connection
		for(int32 i=0; i<MAX_CONN_NUM; i++)
		{
			if( conn_list[i].NodeID == NodeID )
			{
				connChnnel = conn_list[i].msgQReceiver;
				break;
			}
		}
	}
	return connChnnel;
}


//open the pipe communicate with the XTP client
static CIDCMsgQueue* conn_channel_attach(int32 pid, int32 chid)
{
	int32 iRetry = 5;
	CIDCMsgQueue* msgQRcv = new CIDCMsgQueue;
	if(NULL!=msgQRcv)
	{
		char msgQPath[128];
		memset(msgQPath, 0, sizeof(msgQPath));
		sprintf(msgQPath,"%s_%d_%d",msgq_xtp_srv2cltpre,pid,chid); 
		bool ifOpenSucessfully = false;

		do
		{
			iRetry--;
			if(IDC_ERROR==msgQRcv->open(msgQPath, O_WRONLY))
			{
				//try again
			}
			else
			{
				ifOpenSucessfully = true;
				iRetry = 0;
			}
		} while (iRetry != 0);
		if(!ifOpenSucessfully)
		{
			delete msgQRcv;
			msgQRcv = NULL;
		}
	}   

	return msgQRcv;
}

int32 conn_channel_send_l( CIDCMsgQueue* msgQSrv2Client, void *buf, int32 len)
{
	int32 iRetval = -1;
	int32 iRetry = 10;

	if(NULL==buf || msgQSrv2Client==NULL){
		printf("###error\n");
		return -1;
	}	
	do
	{
		iRetry--;

		iRetval = msgQSrv2Client->send(buf, len);
//		IDC::Log(IDC::LOG_INFO,LOG_TAG,"reply(%d) to App result %s",len,(iRetval==0)?"OK":"FAIL");

		if ( (iRetval == -1) && ((errno==EAGAIN) || (errno==EINTR)) )
			(void)usleep(1000);
		else
		{
			iRetry = 0;
		}

	}  while (iRetry != 0);
	return iRetval;
}
//int32 conn_channel_send( CIDCMsgQueue* msgQ, void *buf, const int32 len )
//{
//	return conn_channel_send_l(msgQ,buf,len);
//}

int32 conn_channel_send( void* buf, const int32 len,const int32 pid, const int32 chid )
{
	int32 conn_idx = conn_channel_get(pid, chid);
	int32 rc = -1;

	//find connection
	if( (conn_idx != -1)&&conn_list[conn_idx].bus_config )
	{
		rc = conn_channel_send_l(conn_list[conn_idx].msgQReceiver,buf,len);
	}
	return  rc;
}


//used for send out to self(Server), handle in app_handler
int32 conn_channel_send(void* buf, const int32 len)
{
	int32 rc = -1;     
	if(mq_info.mq!=-1 && buf!=NULL && len<=sizeof(tXtpMessage))
	{       
		int32 iRetry = 5;//Try 5 time
		do
		{                
			rc  = mq_send( mq_info.mq, (char*)buf, len, 0 );
			iRetry--;

			if ( (rc == -1) && ((errno==EAGAIN) || (errno==EINTR)) )
			{
			}
			else
				iRetry = 0;
		}  while (iRetry != 0);
	}
	return rc;
}


bool conn_channel_add(int32 pid, int32 chid, int32 bus/*= 0*/)
{
	bool rc = false;

	int32 conn_idx = 0;

	//Check if connection to channel ok
//	IDC::Log(IDC::LOG_DEBUG, LOG_TAG,"conn_channel_add pid:%d chid:%d", pid, chid);

	CIDCMsgQueue* msgQRcv = conn_channel_attach(pid, chid);
	if(msgQRcv==NULL)
	{
		return rc;
	}

//	IDC::Log(IDC::LOG_DEBUG, LOG_TAG,"conn_channel_add ok!!");

	//find empty connection
	for(conn_idx=0; conn_idx<MAX_CONN_NUM; conn_idx++)
	{
		if( conn_list[conn_idx].pid == 0 )
		{
			break;
		}
	}
	if( (conn_idx<MAX_CONN_NUM) && (bus<MAX_PORT_SUPPORT) )
	{        
		conn_list[conn_idx].pid     = pid;
		conn_list[conn_idx].chid    = chid;
		//OwnChid  = OwnChannelId;
		conn_list[conn_idx].bus     = bus;        
		conn_list[conn_idx].msgQReceiver    = msgQRcv;
		conn_list[conn_idx].alarm   = 0;
		conn_list[conn_idx].NodeID  = chid;

		// Is bus configured?
		conn_list[conn_idx].bus_config = true;
	}
	else
	{
		conn_list[conn_idx].bus_config = false;
	}
	return conn_list[conn_idx].bus_config;

}

//delete Channel will remove all the related ID filter.
//So if a channel for server and client lost, call this function to remove the channel.
//return value is the Client MsgQ Handler. for Answer the Client
CIDCMsgQueue* conn_channel_del(int32 pid, int32 chid)
{	
    CIDCMsgQueue* clientForAnswer = NULL;
	int32 conn_idx;

	//find connection
	if((conn_idx = conn_channel_get(pid, chid)) != -1 )
	{
        clientForAnswer = conn_list[conn_idx].msgQReceiver;
    	conn_list[conn_idx].pid     = 0;
    	conn_list[conn_idx].chid    = 0;
    	conn_list[conn_idx].bus_config = false;
    	conn_list[conn_idx].bus     = 0;
    	conn_list[conn_idx].msgQReceiver    = NULL;
    	conn_list[conn_idx].alarm   = 0;
    	//remove all the ID Filter of this channel
    	memset(conn_list[conn_idx].id_filter,0,sizeof(conn_list[conn_idx].id_filter));
        conn_list[conn_idx].idFilterNumber = 0;
	}

	return clientForAnswer;
}

int32 conn_channel_recv(void* buf)
{
	int32 iRetval = -1;
	int32 iRetry  = 10;    

	if ( own_chid != -1 )
	{        
		bool gotMsg = false;
		do
		{
			fd_set set;
			struct timeval timeout;          

			FD_ZERO(&set); /* clear the set */
			FD_SET(own_chid, &set); /* add our file descriptor to the set */

			iRetry--;

			timeout.tv_sec = 0;
			timeout.tv_usec = 50000;
			iRetval = select(own_chid + 1, &set, NULL, NULL, &timeout);            
			if(iRetval == -1){
				if((errno==EINTR)||(errno==EAGAIN)){//interupt by SIGNAL, try again                

				}else{
					perror("###Error happens select:"); /* an error accured */                
					break;
				}
			}    
			else if(iRetval == 0){/* a timeout occured */                
				break;
			}
			else{
				gotMsg = true;                          
				break;
			} 			
		}  while (iRetry != 0);

		if(gotMsg){
			iRetry = 5;//Try 5 time
			do
			{                
				iRetval   = read(own_chid, buf, sizeof(tXtpMessage));
				iRetry--;

				if ( (iRetval == -1) && ((errno==EAGAIN) || (errno==EINTR)) )
				{
				}
				else
					iRetry = 0;
			}  while (iRetry != 0);
		}
	}
	else
	{
//		IDC::Log(IDC::LOG_ERR,LOG_TAG,"own_chid is illegal yet %d",own_chid);
	}

	return iRetval;
}


int32 conn_channel_reset_alarm(const int32 pid, const int32 chid)
{
	const int32 conn_idx = conn_channel_get(pid, chid);
	if(conn_idx != -1 )
	{
		conn_list[conn_idx].alarm = 0;
	}		
	return conn_idx;
}

int32 conn_channel_filter(int32 pid, int32 chid, uint16 Id, uint8 add)
{
	int32 conn_idx;

	//find connection
	if((conn_idx = conn_channel_get(pid, chid)) == -1 )
		return -1;

	if (conn_list[conn_idx].msgQReceiver !=NULL )
	{
		if( conn_list[conn_idx].bus_config )            
		{
			if( add == 1 )//Add
			{
				conn_channel_filter_add(conn_idx, Id);				
			}
			else
			{
				conn_channel_filter_del(conn_idx, Id);				
			}
		}

		return 0;
	}

	return -1;
}

//use to reply to Client about SysInit, SysExit, SysSend(only fail). Length is const COMM_LENGTH_SERVER_CLIENT
int32 conn_channel_answer (int32 pid, int32 chid, uint8 command, uint8 state)
{
	int32 conn_idx;
	bool rc = -1;
	//find connection
	if((conn_idx = conn_channel_get(pid, chid)) != -1 )
	{
		if (conn_list[conn_idx].msgQReceiver != NULL)
		{
			tXtpMessage answer;

			answer.bCommand = command;
			answer.bState = state;	
//			IDC::Log(IDC::LOG_INFO,LOG_TAG,"Answer CMD %d State %d to pid %d chid 0x%x",
//				command,state,pid,chid);

			rc = conn_channel_send_l(conn_list[conn_idx].msgQReceiver,
				&answer,
				COMM_LENGTH_SERVER_CLIENT);
		}  
	}
	else
	{
//		IDC::Log(IDC::LOG_ERR,LOG_TAG,"conn_idx Illegal");
	}

	return rc;
}

int32 conn_channel_filter_add(int32 index, int32 id)
{
	int32 i = 0;

	//find first element with ID -1
	for(i=0; i<MAX_ID_FILTER_EACH_CHN; i++)
	{
		if( conn_list[index].id_filter[i] == 0 )
		{
			conn_list[index].id_filter[i] = id;
            conn_list[index].idFilterNumber++;
			break;
		}
	}
	return i;
}

int32 conn_channel_filter_del(int32 index, int32 id)
{
	int32 i = 0;

	//find first element with ID id
	for(i=0; i<MAX_ID_FILTER_EACH_CHN; i++)
	{
		if( conn_list[index].id_filter[i] == id )
		{
			conn_list[index].id_filter[i] = 0;
            if(conn_list[index].idFilterNumber>0)
            {
                conn_list[index].idFilterNumber--;
            }
			break;
		}
	}
	return i;
}

CIDCMsgQueue* conn_channel_filter_get(int32 id, int32& pos, uint8& target )
{
	CIDCMsgQueue* rc = NULL;

	//find first element with ID id
	while( (pos<MAX_CONN_NUM) && (rc==NULL) )
	{        
		if( conn_list[pos].bus_config )
		{
//			IDC::Log(IDC::LOG_DEBUG,LOG_TAG,"Pre scan connlist pos %d,ifconfig %d, Node 0x%x,to be check 0x%x filter cnt %d",
//						pos,conn_list[pos].bus_config,conn_list[pos].NodeID,id,conn_list[pos].idFilterNumber);
			int32 j = 0;
			for(int32 i=0; i<MAX_ID_FILTER_EACH_CHN; i++)
			{
				if(conn_list[pos].id_filter[i]!=0)
				{
//					IDC::Log(IDC::LOG_DEBUG,LOG_TAG,"Pre scan connlist pos %d,ifconfig %d, Node 0x%x id 0x%x",
//							pos,conn_list[pos].bus_config,conn_list[pos].NodeID,conn_list[pos].id_filter[i]);
					if(j++<conn_list[pos].idFilterNumber)
					{
						if( conn_list[pos].id_filter[i] == id )
						{
							rc = conn_list[pos].msgQReceiver;
							target = conn_list[pos].NodeID;
							break;
						}
					}
					else
					{
						break;
					}
				}
			}
		}
		if(rc==NULL)
		{
			pos++;
		}
	}
	return rc;
}


int32 conn_channel_bus_get(int32 pid, int32 chid)
{
	int32 conn_idx;

	// find connection and check if the its bus is configured
	if((conn_idx = conn_channel_get(pid, chid)) == -1 || !conn_list[conn_idx].bus_config )
		return -1;
	return conn_list[conn_idx].bus;
}
