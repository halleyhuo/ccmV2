/*--------------------------------------------------------------------------*/
/*  include files                                                           */
/*--------------------------------------------------------------------------*/

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sched.h>
#include <errno.h>
#include <stdio.h>

#include "Types.h"
#include "xtpClient.h"

#define TIMEOUT 100 /* 100 * 10ms = 1s */
#define MSGQ_MAINTAIN_CNT 32

BRODCAST_LIST* GetBroadList(const uint8 NodeID)
{
	BRODCAST_LIST* rc =NULL;
	for(int32 i=0; i<sizeof(broadcastList)/sizeof(broadcastList[0]); i++)
	{
		if(broadcastList[i].XTPNode==NodeID)
		{
			rc = &broadcastList[i];
			break;
		}
	}
	return rc;
}


CXtpClient::CXtpClient(const uint8 id)
	: m_XTP_Node(id),m_iPid(-1),m_ServerQueue(-1),m_iDstConnection(false),m_RcvThreadId(-1),m_StatThreadId(-1),
	dStackSize(PTHREAD_STACK_MIN),pStack(NULL),iConnectPrio(-1),m_ExitFromServer(false),
	m_own_msgQ(NULL),m_semaPReadyforRcv(false)
{
	memset(m_fifo_xtp_srv2cltPath,0,sizeof(m_fifo_xtp_srv2cltPath));	
}


int32 CXtpClient::CreateChannelSrv2Clt()
{
	int32 rc = -1;
	if ( m_own_msgQ ==NULL )
	{

		m_own_msgQ = new CIDCMsgQueue;
		memset(m_fifo_xtp_srv2cltPath,0,128);
		sprintf(m_fifo_xtp_srv2cltPath,"%s_%d_%d",msgq_xtp_srv2cltpre,getpid(),m_XTP_Node);
		printf(">>>INFO prepare create Channel %s\n",m_fifo_xtp_srv2cltPath);
		rc = m_own_msgQ->openCreate(m_fifo_xtp_srv2cltPath,MSGQ_MAINTAIN_CNT,sizeof(tXtpMessage),O_RDWR,false);
		if(IDC_ERROR==rc)
		{
			printf("###error, create %s fail, errorno %d\n",m_fifo_xtp_srv2cltPath,m_own_msgQ->GetIDCErrno());
			delete m_own_msgQ;
			m_own_msgQ = NULL;
		}
		else
		{
			rc = 0;
		}
	}
	else
	{
		rc = 0;
	}
	return rc;
}


//below function is not thread safe because we suppose the kill thread doesn't happen offen
void CXtpClient::ThreadDestroy()
{
	tXtpMessage ThreadOperate;
	ThreadOperate.bCommand = KILL_THREAD;

	if(m_own_msgQ!=NULL)
	{
		m_own_msgQ->send(&ThreadOperate,COMM_LENGTH_SERVER_CLIENT);		       
	}
}

/*--------------------------------------------------------------------------*/
/* connect to XTPServer                                                           */
/*--------------------------------------------------------------------------*/

bool CXtpClient::XtpConnect (AnswerFuncType funcptr, BRODCAST_LIST* broadacstList, void* extraPara /*=NULL*/)
{
	int32 i;                  
	int32 iPrio = getprio(0);
//	IDC::Log(IDC::LOG_DEBUG,LOG_TAG,"iConnectPrio %d,%d\n", iConnectPrio,iPrio);
	bool rc = false;

	m_RcvHandler = funcptr;
	m_RcvHandlerPara = extraPara;

	if (iConnectPrio==-1)
		iConnectPrio = iPrio;
	else if (iPrio!=iConnectPrio)
		setprio(0,iConnectPrio);

	if (m_RcvThreadId == -1)
	{
		m_semaPReadyforRcv = false;
		/* create channel to receive messages */
		if(-1 == CreateChannelSrv2Clt())
		{//create pipe faile
			printf("###error, create fifo of server2clt failed, error no %d\n",errno);			
		}
		else
		{
			if (m_own_msgQ!=NULL)
			{
				pthread_attr_t CThreadCattr;
				m_iPid  = getpid();
				/* create Thread to wait for XTP Server messages */
				pthread_attr_init(&CThreadCattr);
				if (pthread_attr_setschedpolicy(&CThreadCattr,SCHED_FIFO) != 0)
					handle_error_en(errno, "pthread_attr_setschedpolicy");

				int32 paramPolicy;   
				pthread_attr_getschedpolicy(&CThreadCattr,&paramPolicy);
				pthread_attr_setdetachstate(&CThreadCattr, PTHREAD_CREATE_DETACHED);
				pthread_attr_setstacksize  (&CThreadCattr, (size_t)dStackSize);


				// establish connection          
				if (pthread_create( &m_RcvThreadId, &CThreadCattr, &ReceiveThread,this )!=-1)
				{
					if(-1==m_ServerQueue){
						m_ServerQueue = mq_open (xtpServerPath, O_WRONLY | O_NONBLOCK);
					}
					if (m_ServerQueue!=-1)
					{ 
						while(!m_semaPReadyforRcv)
						{
							usleep(1000);
						}


						/* connect to XTPServer */
//						IDC::Log(IDC::LOG_INFO,LOG_TAG,"Out XtpSysInit");
						const bool sysInitSendout = XtpSend (NULL,XtpSysInit,broadacstList);

						if(sysInitSendout)
						{
							/* wait for connection, block time 5s to wait */
							waitShakeHandfromSrv.WaitFor(5,0);
						}
						//Still havn't connection
						if (!sysInitSendout || !m_iDstConnection)
						{
//							IDC::Log(IDC::LOG_ERR,LOG_TAG,"###Error No Respone for Init from Server");
							ThreadDestroy();
							m_RcvThreadId = -1;

							//close connect channel
							m_own_msgQ->destroy();
							delete m_own_msgQ;
							m_own_msgQ = NULL;	
							mq_close (m_ServerQueue);
							m_ServerQueue = -1;
						}
						else
						{
							//create State Thread StateThread
							if (pthread_create( &m_StatThreadId, &CThreadCattr, &StateThread,this )!=-1)
							{
								rc = true;
							}
						}

					}
					else
					{
						ThreadDestroy( );
						m_RcvThreadId = -1;

						m_own_msgQ->destroy();
						delete m_own_msgQ;
						m_own_msgQ = NULL;	
					}
				}
				else
				{
					m_RcvThreadId = -1;

					m_own_msgQ->destroy();
					delete m_own_msgQ;
					m_own_msgQ = NULL;	
				}
			}
		}
	}
	return rc;
}

/*--------------------------------------------------------------------------*/
/* check and reconnect to Xtp Server                                               */
/*--------------------------------------------------------------------------*/

bool CXtpClient::XtpCheckConnect ()const
{	
	return m_iDstConnection && (m_RcvThreadId!=-1);
}
/*--------------------------------------------------------------------------*/
/* disconnect from Xtp Server                                                      */
/*--------------------------------------------------------------------------*/

void CXtpClient::XtpDisconnect ()
{
	char i;
	m_ExitFromServer=false;
	XtpSend (NULL,XtpSysExit);

	if (m_RcvThreadId!=-1)
	{
		// wait for disconnect      
		for (i=0; (i<100) && (!m_ExitFromServer); i++)
		{
			usleep(10000);
		}
//		IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Will Send Kill Thread to self %d",m_ExitFromServer);

		ThreadDestroy();
		m_RcvThreadId = -1;
	}

	if (m_iDstConnection)
	{ 
		m_iDstConnection = false;
		mq_close (m_ServerQueue);
        m_ServerQueue = -1;
	}

	if (m_own_msgQ!=NULL)
	{
		m_own_msgQ->destroy();
		delete m_own_msgQ;
		m_own_msgQ = NULL;	
	}
}

/*--------------------------------------------------------------------------*/
/* put a command into the Xtp Server queue                                         */
/*--------------------------------------------------------------------------*/

bool CXtpClient::XtpSend (tXtpObjHeader *msg, uint8 command /* =XtpSysSend*/,void* extralPara/* = NULL*/)
{
	tXtpMessage toXtpServerCmd;
	bool bRet = false; /* FALSE */

	toXtpServerCmd.pid      = m_iPid;
	toXtpServerCmd.bCommand = command;
	toXtpServerCmd.chid     = m_XTP_Node;


	switch (command)
	{
	case XtpSysInit:
		toXtpServerCmd.msgHeader.telID=SCONNECT;
		if(extralPara!=NULL)
		{
			BRODCAST_LIST* brodcastList = (BRODCAST_LIST* )extralPara;
			((sconnect_t1*)toXtpServerCmd.data)->no_objects = brodcastList->count;
//			IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Send Out SCONNECT, broadcastcnt %d NodeID 0x%x",
//					brodcastList->count,brodcastList->XTPNode);

			memcpy( ((sconnect_t1*)toXtpServerCmd.data+1),
				brodcastList->List,
				brodcastList->count*sizeof(brodcastList->List[0]));
			toXtpServerCmd.msgHeader.length=sizeof(tXtpObjHeader)+sizeof(sconnect_t1)+
				((sconnect_t1*)toXtpServerCmd.data)->no_objects*sizeof(sconnect_t2);

		}
		else
		{
			((sconnect_t1*)toXtpServerCmd.data)->no_objects = 0;
			toXtpServerCmd.msgHeader.length=sizeof(tXtpObjHeader)+sizeof(sconnect_t1);
		}
		break;
	case XtpSysExit:
	case XtpSysStat:
		break;
	case XtpSysAddId:
	case XtpSysDelId:
		break;
	case XtpSysSend:

		memcpy(&toXtpServerCmd.msgHeader,msg,msg->length);
		

		break;
	default:
		/* wrong command */
		command =XtpSys_NOCMD;
		break;
	}
//	IDC::Log(IDC::LOG_INFO,LOG_TAG,"Send Out:cmd %d send fd %d ",command,m_ServerQueue);

	if (command==XtpSysInit)
	{
		if (m_ServerQueue!=-1)
		{
			int32 i;
			for (i=0; i<TIMEOUT; i++)
			{
				if (mq_send(m_ServerQueue, (const char*)&toXtpServerCmd, COMM_LENGTH_SERVER_CLIENT+toXtpServerCmd.msgHeader.length, getprio(0))>=0)
				{				
					bRet=true;
					break;
				}
			}
		}	
	}
	else if (command!=XtpSys_NOCMD)
	{
		if (m_iDstConnection)
		{         
			while (true)
			{           
				if(mq_send(m_ServerQueue, (const char*)&toXtpServerCmd, 
					COMM_LENGTH_SERVER_CLIENT+toXtpServerCmd.msgHeader.length,getprio(0))>=0)
				{
					bRet=true; /* TRUE */
					break;
				}
				else if (errno!=EINTR)
				{
//					IDC::Log(IDC::LOG_ERR,LOG_TAG,"XtpSend Fail: error No %d",errno);
					if ( (errno==ESRCH) || (errno==EBADF) )
					{ 
						mq_close(m_ServerQueue);
                        m_ServerQueue = -1;
						m_iDstConnection = false;
					}

					sleep(5); // do not block os
					break;
				}
			}
		}

	}
	else//NO COMMADN
	{
		bRet=true;
	}

	return bRet;
}

/*--------------------------------------------------------------------------*/
/* Thread to Send Stat messages to the Xtp Server                                  */
/*--------------------------------------------------------------------------*/

void* CXtpClient::StateThread (void*pArg)
{
	CXtpClient* xtpClient = NULL;
	tXtpMessage* pAnswer = NULL;
	if(pArg!=NULL)
	{
		xtpClient = (CXtpClient*)pArg;
		while(xtpClient!=NULL && xtpClient->m_ServerQueue!=-1)
		{
			tXtpMessage toXtpServerCmd;

			toXtpServerCmd.pid      = xtpClient->m_iPid;
			toXtpServerCmd.bCommand = XtpSysStat;
			toXtpServerCmd.chid     = xtpClient->m_XTP_Node;
			if(mq_send(xtpClient->m_ServerQueue, (const char*)&toXtpServerCmd, 
				COMM_LENGTH_SERVER_CLIENT,getprio(0))<0)
			{
//				IDC::Log(IDC::LOG_ERR,LOG_TAG,"###Error on send out State Node 0x%x error %d",xtpClient->m_XTP_Node,errno);
			}

			sleep(1);
		}
//		IDC::Log(IDC::LOG_WARNING,LOG_TAG,"Will Exit the XTPClientStatLoop %xH",xtpClient->m_XTP_Node);
	}
}


/*--------------------------------------------------------------------------*/
/* Thread to receive messages from the Xtp Server                                  */
/*--------------------------------------------------------------------------*/

void* CXtpClient::ReceiveThread (void*pArg)
{
	CXtpClient* xtpClient = NULL;
	tXtpMessage* pAnswer = NULL;
	if(pArg!=NULL)
	{
		xtpClient = (CXtpClient*)pArg;
		pAnswer = new tXtpMessage;

		while (xtpClient->m_own_msgQ!=NULL)
		{
//			IDC::Log(IDC::LOG_DEBUG,LOG_TAG,"Prepare call recv,pid %d NodeID 0x%x", getpid(),xtpClient->m_XTP_Node);
			xtpClient->m_semaPReadyforRcv=true;
			int32 recvmsgcnt = xtpClient->m_own_msgQ->recv(pAnswer, sizeof(tXtpMessage));   		 

			if (recvmsgcnt>0)
			{				
//				IDC::Log(IDC::LOG_NOTICE,LOG_TAG,
//					"get Rcvmsg from Server,%d, cmd:%d,state %d ",
//					recvmsgcnt,pAnswer->bCommand,pAnswer->bState);
				switch (pAnswer->bCommand)
				{
				case KILL_THREAD://exit self thread right now
//					IDC::Log(IDC::LOG_WARNING,LOG_TAG,"Will Exit the XTPClientRcvLoop %xH",xtpClient->m_XTP_Node);
					return NULL;
					break;
				case XtpSysInit:
					if (!xtpClient->m_iDstConnection)
					{//open the pipe used for sending out the XTP telegram
						xtpClient->m_iDstConnection = true;
						xtpClient->waitShakeHandfromSrv.Post();
					}
					break;
				case XtpSysExit:
					xtpClient->m_ExitFromServer=true;
					break;

				case XtpSysSend:
					if(XtpNotOk==pAnswer->bState)//send XTP Client Fail
					{
//						IDC::Log(IDC::LOG_ERR,LOG_TAG,"###error, Send XTP Msg to Server fail @XTPNodeID 0x%x",xtpClient->m_XTP_Node);
					}
					break;
				default:
					{
						if(pArg!=NULL)
						{
							xtpClient->m_RcvHandler(&pAnswer->msgHeader,xtpClient->m_RcvHandlerPara);
						}
					}

					break;
				}
			}
			else if(recvmsgcnt<0)
			{
//				IDC::Log(IDC::LOG_ERR,LOG_TAG,"###error, call Rcv self MsgQ Fail,pid %d NodeID 0x%x",getpid(),xtpClient->m_XTP_Node);
			}
			else
			{
//				IDC::Log(IDC::LOG_INFO,LOG_TAG,"recv result as 0,pid %d NodeID 0x%x",getpid(),xtpClient->m_XTP_Node);
				usleep(10000); // do not block OS by suspend 10ms
			}
		}
	}


	if(pAnswer!=NULL)
	{
		delete pAnswer;
		pAnswer = NULL;
	}

	xtpClient->m_RcvThreadId = -1;

	return NULL;
}

