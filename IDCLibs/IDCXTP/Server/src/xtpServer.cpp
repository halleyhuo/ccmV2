/*
* xtpServer.cpp
*
* This is for the XTP server running in Linux
* This is main module in the exe XTPServer
*/

#include "msgqueue.h"

#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>                 // siginfo_t, signals
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Types.h"
#include "IDCLog/inc/LogTrace.h"

#include "XtpServer.h"
#include "xtpcomm.h"
#include "ExtComm.h"


#define MAX_XTP_OBJ_LENGTH			1032

namespace IDC
{
	const int32 debug_level_remote=LOG_DEBUG+1; 
	const int32 debug_level_local=0;
}

int32 sockExt[MAX_PORT_SUPPORT] = {-1,-1}; 

//control all the thread loop. Convinient for UT
static bool INRUNNING = true;

//Send data to Client. 
//return value: -1 indicates fail. other value indicates the actual send value
static int32 SendtoClient( tXtpMessage *InTel,int32 _DeviceNum=0 )
{
	int32 retVal = -1;
	if (InTel!=NULL&&InTel->msgHeader.length>=sizeof(tXtpObjHeader))
	{
		const uint8 TargetNodeID = InTel->msgHeader.target;
//		IDC::Log(IDC::LOG_INFO,LOG_TAG,"Prepare route msg 0x%x(%d) to client mode %d,target %d", 
//				InTel->msgHeader.telID,InTel->msgHeader.length,InTel->msgHeader.mode,TargetNodeID);
		//Simulation Solution
//		if(InTel->msgHeader.telID == SFLCMD && InTel->msgHeader.mode == XTP_MODE_BROADCAST)
//		{
//			SendExtMsg(&InTel->msgHeader,InTel->msgHeader.length,ExtID_SimuOperate,ExtLinkSimu);
//		}
		if(InTel->msgHeader.mode == XTP_MODE_DIRECT)
		{    
			//modity the command to Recv
			InTel->bCommand  = XtpSysReceive;
			//modify, state change to XtpOk
			InTel->bState 	 = XtpOk;
			//get channel based on the target NodeID
			CIDCMsgQueue* TargetChannel = conn_channel_get(TargetNodeID);
			if(TargetChannel!=NULL)
			{
				retVal = conn_channel_send_l(TargetChannel, InTel, InTel->msgHeader.length+COMM_LENGTH_SERVER_CLIENT);
                RecordXTP( "XTP Msg %xH(%d) mode %d %xH --> %xH", 
		            InTel->msgHeader.telID,InTel->msgHeader.length,InTel->msgHeader.mode,InTel->msgHeader.sender,InTel->msgHeader.target);
				SendExtMsg(&InTel->msgHeader,InTel->msgHeader.length);
			}
		} 
		else if(InTel->msgHeader.mode == XTP_MODE_BROADCAST)
		{
			int32 pos = 0;
            char targetBuf[64];
            
            memset(targetBuf,0,sizeof(targetBuf));
			while(pos<MAX_CONN_NUM)
			{
                uint8 targetNodeID = 0xff;

				CIDCMsgQueue* TargetChannel = conn_channel_filter_get(InTel->msgHeader.telID, pos, targetNodeID);
				if(TargetChannel!=NULL)
				{
//					IDC::Log(IDC::LOG_INFO,LOG_TAG,"Will route Broadcast msg %xH(%d) to client pos %d", 
//							InTel->msgHeader.telID,InTel->msgHeader.length,InTel->msgHeader.mode);
					//modity the command to Recv
					InTel->bCommand  = XtpSysReceive;
					//modify, state change to XtpOk
					InTel->bState 	 = XtpOk;
					char NodeIDBuf[16];
					sprintf(NodeIDBuf," --> %xH",targetNodeID);

					strncat(targetBuf, NodeIDBuf, 64-strlen(NodeIDBuf));
					retVal = conn_channel_send_l(TargetChannel, InTel, InTel->msgHeader.length+COMM_LENGTH_SERVER_CLIENT);
					pos++;
				}
				else
				{
					continue;
				}
			}
			RecordXTP( "XTP Msg %xH(%d) mode %d %xH %s", InTel->msgHeader.telID,InTel->msgHeader.length,InTel->msgHeader.mode,InTel->msgHeader.sender,targetBuf);
		}
		else//other mode not support here
		{
//			IDC::Log(IDC::LOG_ERR,LOG_TAG,"###error, Prepare route msg 0x%x(%d) to client mode %d fail", 
//				InTel->msgHeader.telID,InTel->msgHeader.length,InTel->msgHeader.mode);
			retVal = -1;
		}
	}

	return retVal;	
}

// this thread is used to receive Socket Packet from other Node
void *	ExtNodeSocket_Handler (void *arg)
{      
	char buf[MAX_XTP_OBJ_LENGTH]; 
	memset(&buf, 0, sizeof(buf));
	while(INRUNNING) {	
		//set of socket descriptors
		fd_set readfds;
		//clear the socket set
		FD_ZERO(&readfds);
		int32 max_sd = 0;
		for(int32 i=0; i<MAX_PORT_SUPPORT; i++){
			//if valid socket descriptor then add to read list
			if(sockExt[i]>0){
				FD_SET( sockExt[i] , &readfds);
				//printf(">>>infor, add sockExt fd %d to sets\n",sockExt[i]);
			}else{
				printf("###error happen on invalid sockExt idx %d\n",i);
			}
			//highest file descriptor number, need it for the select function
			if(sockExt[i]>max_sd){
				max_sd = sockExt[i];
			}            
		}   

		//wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
		int32 activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

		if ((activity < 0) && (errno!=EINTR)) 
		{
			perror("###error happens on select: ");
		}else if(0==activity){
		}else{          
			//check which socket fd is set
			for (int32 i = 0; i < MAX_PORT_SUPPORT; i++) 
			{
				int32 sd = sockExt[i];

				if (FD_ISSET( sockExt[i] , &readfds)) 
				{
					/* Read a message back */
					int32 bytes_read = read( sockExt[i], buf, sizeof(buf) );
					if(bytes_read<0){
						perror("call read sockExt failed: ");
					}else if(0==bytes_read){
						/*Nothing read, retry again*/
					}else{
					}                   
				}
			}
		}      

	}

	exit(-1);
	//return 0;
}

// this thread is dedicated to handling and managing message communication
// over IPC
void * app_handler (void *arg)
{

	int32 id;
	struct sigevent  tSevent;
	struct itimerspec tItime;

	tXtpMessage Command;

	int32 fKill = 0;

	if(msgq_create(100, sizeof(tXtpMessage)))
	{
		//Handle Data from Client
		while(INRUNNING)
		{  
            const int32 rc = msgq_recv(&Command);
			if ( rc>0 )
			{
				if((Command.bCommand != XtpSysStat)  &&(Command.bCommand != XtpSysReceive)
					&&(XtpSysPulse!=Command.bCommand)){
//						IDC::Log(IDC::LOG_INFO,LOG_TAG,"Rcv (%d) from App Cmd:%d from PID:%d", rc,Command.bCommand,Command.pid);
				}

				switch (Command.bCommand)
				{
				case XtpSysInit:
					{                    
						if(Command.msgHeader.telID==SCONNECT)
						{
							const ushort objCnt = ((sconnect_t1*)Command.data)->no_objects;
							if( (sizeof(objCnt)*objCnt+sizeof(sconnect_t1)+sizeof(tXtpObjHeader))
								== Command.msgHeader.length )
							{
//								IDC::Log(IDC::LOG_INFO,LOG_TAG,"Got SCONNECT(%d) from Client 0x%x Process %d ", Command.msgHeader.length,Command.chid,Command.pid);
								//Add the channel(from Server to Client)
								if(!conn_channel_add(Command.pid, Command.chid))
								{
									conn_channel_answer(Command.pid, Command.chid,XtpSysInit,XtpNotOk);
								}
								else
								{
									for(ushort i = 0; i<objCnt; i++)//Add brocast telegram ID 
									{
										conn_channel_filter(Command.pid, Command.chid, 
											((sconnect_t2*)(Command.data+sizeof(sconnect_t1))+i)->object_id , 1 );
									}
									conn_channel_answer(Command.pid, Command.chid,XtpSysInit,XtpOk);
									RecordXTP("Node %xH Connect to the XTP", Command.chid);
								}
							}
							else
							{
//								IDC::Log(IDC::LOG_ERR,LOG_TAG,"Length Fail objCnt %d act Length %d",objCnt,Command.msgHeader.length);
							}

						}
						else
						{//Command.msgHeader.telID
//							IDC::Log(IDC::LOG_WARNING,LOG_TAG,"Illegal TelID 0x%x (%d) for SysInit", Command.msgHeader.telID,Command.msgHeader.length);
						}
					}
					break;
				case XtpSysExit:
				{
					CIDCMsgQueue* clientMsgQ = conn_channel_del(Command.pid, Command.chid);
					if(clientMsgQ!=NULL)
					{
						tXtpMessage answer;
						answer.bCommand = XtpSysExit;
						answer.bState = XtpOk;	
						conn_channel_send_l(clientMsgQ, &answer, COMM_LENGTH_SERVER_CLIENT);
					}
				}
					break;
				case XtpSysAddId:
//					IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"Add Id:0x%X to PID:%d,chid:%d", id,Command.pid, Command.chid);
					conn_channel_filter(Command.pid, Command.chid, id, 1 );
					break;
				case XtpSysDelId:
					conn_channel_filter(Command.pid, Command.chid, id, 0 );
					break;
				case XtpSysSend:
					{
						if(-1==SendtoClient(&Command))
						{
//							IDC::Log(IDC::LOG_ERR,LOG_TAG,"###Send 0x%x(%d) fail.Mode %d, Src 0x%x",Command.msgHeader.telID,
//									Command.msgHeader.length,Command.msgHeader.mode,Command.msgHeader.sender);
							conn_channel_answer(Command.pid, Command.chid, XtpSysSend,XtpNotOk);
						}
					}
					break;
				case XtpSysKill:
					(void)kill(getpid(),SIGTERM);
					break;
				case XtpSysStat:
					{                    
						//State Frame shall got every 3s
						//Not necessary reply to App if got State Frame from App					
						//RecordXTP("NODE %xH KEEP ALIVE",Command.chid);
						conn_channel_reset_alarm(Command.pid,Command.chid);					
					}
					break;
				default:
					break;
				}
			}	
			else
			{
//				IDC::Log(IDC::LOG_ERR,LOG_TAG,"###error, call rcv msg fail as result %d",rc);
			}
		}
	}
	else
	{
		printf("###error,conn_channel_create call failed\n");
		(void)kill(getpid(),SIGINT);
	}

	return 0;
}

void * ChannelStatMonitor (void *arg)
{
	while(INRUNNING)
	{
		for(int32 i=0; i<MAX_CONN_NUM;i++)
		{
			if(conn_list[i].bus_config)
			{
				conn_list[i].alarm++;
				if(conn_list[i].alarm>=CHNSTAT_TIMEOUT)
				{
					struct timespec curRealTm; 
					clock_gettime( CLOCK_REALTIME, &curRealTm );
//					IDC::Log(IDC::LOG_ERR,LOG_TAG,"Send out XtpSysExit to Self as haven't got STAT in timeout PID:%d CHID %d@[%d:%09d]",
//						conn_list[i].pid, conn_list[i].chid,curRealTm.tv_sec,curRealTm.tv_nsec);
					tXtpMessage Command;
					Command.bCommand = XtpSysExit;
					Command.pid = conn_list[i].pid;
					Command.chid= conn_list[i].chid;
					//Note app_handler to remove this connection
					conn_channel_send(&Command,COMM_LENGTH_SERVER_CLIENT);
					//after halt timeout will retry send out the SysExit if this Client Connection still saved as actived in Table
					conn_list[i].alarm = CHNSTAT_TIMEOUT/2;
				}
			}
		}

		usleep(50000);//50ms
	}
}


int32 main(int32 argc, char *argv[])
{ 
	pthread_attr_t attr;
	pthread_t tidCommInnerClient;
	pthread_t tidRcvExtNode;
	pthread_t tidMonitorStat;
	pthread_t tidExtRcvThread;
	pthread_attr_init(&attr);     

	if (pthread_attr_setschedpolicy(&attr,SCHED_FIFO) != 0)
		handle_error_en(errno, "pthread_attr_setschedpolicy");  

	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED );
	int32 ret=pthread_attr_setstacksize(&attr, STACK_SIZE_DEFAULT);
	if (ret != 0)
		handle_error_en(ret, "pthread_attr_setstacksize");

	InitExtComm();

	//ignor signal, it's very important in Linux because write to an unnormal fd will
	//cause a SIGPIPE and make self crash. Usually if a canopenlib client quit 
	//unnormal(without call uninstall...) will leave an unnormal named pipe
	signal(SIGPIPE, SIG_IGN); 
	//for Handle the tel which comes from XTP client inner
	pthread_create(&tidCommInnerClient, &attr, app_handler, NULL );
	//for receive XTP telegram from ethernet, implement in future
	//pthread_create(&tidRcvExtNode, &attr, ExtNodeSocket_Handler, NULL );
	//for monitor Stat from App.
	pthread_create(&tidMonitorStat, &attr, ChannelStatMonitor, NULL );

	pthread_create(&tidExtRcvThread, &attr, ExtRcv, NULL );


	// wait to application termination using signals
	while(true){
		sleep(10);
	}
	INRUNNING = false;
}

