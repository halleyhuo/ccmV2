/*
* ExtComm.cpp
*
* This is for the ExtComm support
*/

#include "ExtComm.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <mqueue.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <sched.h>
#include <pthread.h>
#include <sys/timeb.h>
#include <time.h>
#include <errno.h>
#include <sys/resource.h>

#include "Types.h"
#include "IDCLog/inc/LogTrace.h"
#include "xtpcomm.h"
#include "msgqueue.h"



#define ADDR_SRV "192.168.8.13"
	const uint16 PORT_SRV = 2288;

#define ADDR_SELF "192.168.8.30"
	const uint16 PORT_SELF = 2288;

#define ADDR_SIMU "192.168.8.15"
const uint16 PORT_SIMU = 2288;

static int16 sockudpsend[ExtLinkSize] = {0,0};
static struct sockaddr_in addrSrv[ExtLinkSize];  
void InitExtComm()
{
	memset(&addrSrv,0, sizeof(addrSrv));
	/* create a socket  Manager Server*/
	sockudpsend[ExtLinkManager] = socket(AF_INET, SOCK_DGRAM, 0); 
		addrSrv[ExtLinkManager].sin_family = AF_INET;

		addrSrv[ExtLinkManager].sin_addr.s_addr = inet_addr(ADDR_SRV);//htonl(INADDR_ANY); //  
		addrSrv[ExtLinkManager].sin_port = htons(PORT_SRV);	

	
		/* create a socket	Simu Server*/
		sockudpsend[ExtLinkSimu] = socket(AF_INET, SOCK_DGRAM, 0); 
			addrSrv[ExtLinkSimu].sin_family = AF_INET;
		
			addrSrv[ExtLinkSimu].sin_addr.s_addr = inet_addr(ADDR_SIMU);//htonl(INADDR_ANY); //  
			addrSrv[ExtLinkSimu].sin_port = htons(PORT_SIMU); 	
		
		printf(">>>INFO Ready For ExtComm ...\n");	
}

void* ExtRcv(void* _para)
{
	int16 sockfd;
	if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0){  
        perror ("socket");  
        return 0;  
    }  
	struct sockaddr_in addr;  
    /*填写sockaddr_in 结构*/  
    bzero ( &addr, sizeof(addr) );  
    addr.sin_family=AF_INET;  
    addr.sin_port=htons(PORT_SELF);  
    addr.sin_addr.s_addr=INADDR_ANY; 
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr))<0){  
        perror("connect");  
        return 0;  
    }  
	char buffer[256];   
	socklen_t addr_len = sizeof addr;	
    while(1)
	{  
        bzero(buffer,sizeof(buffer));  
        int16 len = recvfrom(sockfd,buffer,sizeof(buffer), 0 , (struct sockaddr *)&addr ,&addr_len); 
		if(len>=4)
		{
			Comm_XTP_ExtHeader* header = (Comm_XTP_ExtHeader*)buffer;
//			IDC::Log(IDC::LOG_NOTICE,LOG_TAG,"XTP_SERVER ExtID = %d", header->ExtID);
			switch(header->ExtID)
			{
				case ExtID_SimuSend:
				{
					tXtpObjHeader* XtpObj = (tXtpObjHeader*)&buffer[sizeof(Comm_XTP_ExtHeader)];
					//printf("SimuCMD 0x%x %x->%x, len %d\n",XtpObj->telID,XtpObj->sender,XtpObj->target,XtpObj->length);
					tXtpMessage Command;
					Command.bCommand = XtpSysSend;
					Command.msgHeader.telID = XtpObj->telID;
					Command.msgHeader.sender = XtpObj->sender;
					Command.msgHeader.target = XtpObj->target;
					Command.msgHeader.length = XtpObj->length;
					Command.msgHeader.mode = XtpObj->mode;
					memcpy(Command.data,(char*)(XtpObj+1),XtpObj->length-sizeof(XtpObj));
						
					//Note app_handler to remove this connection
					conn_channel_send(&Command,COMM_LENGTH_SERVER_CLIENT+Command.msgHeader.length);				
				}
					break;

				case ExtID_Manager:
				{
					tXtpObjHeader* XtpObj = (tXtpObjHeader*)&buffer[sizeof(Comm_XTP_ExtHeader)];
					switch(XtpObj->telID)
					{
						case SIMU_IP_SET:
							if(XtpObj->length==sizeof(SIMU_IP_SET_t)+sizeof(tXtpObjHeader))
							{
								addrSrv[ExtLinkSimu].sin_addr.s_addr = htonl(((SIMU_IP_SET_t*)(XtpObj+1))->SimuIp);//htonl(INADDR_ANY); //  
								addrSrv[ExtLinkSimu].sin_port = htons( ((SIMU_IP_SET_t*)(XtpObj+1))->SimuPort );
							}
							break;
						default:
							break;
					}
				}
				break;
				default:
					break;
			}
			
			
			
		}
	}
return 0;	
}

void SendExtMsg(void* data, const uint16 _len, const uint16 extType/* = ExtID_ObjFlow_Info*/, const ExtLinkIdx linkIdx/* = ExtLinkManager*/)
{
	//initialize the udp
		int16 iSendSize = 0;  
		if(sockudpsend[linkIdx]!=0)
		{
			Comm_XTP_Ext buf = {extType};
			memcpy(buf.data,data,_len);
			buf.FrameLen = sizeof(buf.ExtID)+sizeof(buf.FrameLen)+_len;
			
			iSendSize = sendto(sockudpsend[linkIdx], &buf, buf.FrameLen, 0,  
						(struct sockaddr *) &addrSrv[linkIdx], sizeof(struct sockaddr));
		}		
}

void RecordXTP(const char* const fmt, ...)
{
	if(sockudpsend[ExtLinkManager]!=0)
	{
		Comm_XTP_Ext buf = {ExtID_ObjFlow_Info};
		memset(buf.data,0,sizeof(buf.data));
		strncpy(buf.data,"TXT:",4);
		va_list argptr;	
		va_start(argptr, fmt);
		(void)vsnprintf((char*)&buf.data[4], sizeof(buf.data)-4, fmt, argptr);
		va_end(argptr);
		buf.FrameLen = sizeof(buf.ExtID)+sizeof(buf.FrameLen)+strlen((char*)buf.data)+1;
		int16 iSendSize = sendto(sockudpsend[ExtLinkManager], &buf, buf.FrameLen, 0,  
						(struct sockaddr *) &addrSrv[ExtLinkManager], sizeof(struct sockaddr));
	}
}
