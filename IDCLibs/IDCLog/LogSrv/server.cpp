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
#include "LogTrace.h"
#include "cfg.h"

const bool DeBugLocal = false;
const bool DeBugRemote = true;

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

namespace IDC
{
	//syslog protocol, facility local use 0
	const uint8 SYSLOG_FACILITY = 128U;	

	//the max size of record file 5M
	const int32 file_maxSz = 5242880;	

#define ADDR_SRV "192.168.8.13"
	const uint16 PORT_SRV = 514U;

#define ADDR_SELF "192.168.8.30"

	//print task's priority<1,255>
	const uint8 PRINT_TASK_PRIORITY=1U;	
	static char sFileName[64];                      //printf file's name
	static FILE *stream=NULL ;	
	// If create the printing task. The value is 0 if the task has been createn
	static int32 createTask = -1;
#define MARK_FIND_MSG "]"

	//if create new file when system's log task boot up
#define CREATE_NEW_LOG

#ifdef CREATE_NEW_LOG
#define CREATE_FILE_ATTR "w+t"
#else
#define CREATE_FILE_ATTR "a+t"
#endif
	static int32 file_cnt = 0;//the counter of record file	

	inline int32 napms( int32 uiDelay){
		for(uint64 i=uiDelay*10000;i>0;i--){}
		return 0;
	}

	static mqd_t m_iMqdesSrv = -1;

	static void GetFileName(char _fileName[64])
	{		
		sprintf(_fileName,"RAD3_%08d.log",file_cnt);
		file_cnt++;
	}

	static bool ShouldCreateNew(char _fileName[64])
	{
		struct stat info;
		stat(_fileName, &info);
		int32 size = info.st_size;        

		if(size>=file_maxSz){
			fclose( stream );			
			return true;
		}
		return false;				
	}
	static int32 CreateFile(char _filePath[64])
	{
		stream = NULL;
		if((stream = fopen(_filePath, CREATE_FILE_ATTR)) == NULL){
			printf("###### Syslog error: Create log file failed\n");
			return -1;
		}else{				
			printf(">>>>>> Syslog Info: Create log file %s successful\n",_filePath);
			printf(">>>>>> Syslog Info: If Log local:%s\n",DeBugLocal?"Yes":"NO");
			printf(">>>>>> Syslog Info: If Log Remote:%s, local IP:%s, Server Addr:%s[%d]\n",
				DeBugRemote?"Yes":"NO",ADDR_SELF,ADDR_SRV,PORT_SRV);
		}
		return 0;		
	}

	/*------------------------------------------------------------------------------
	* Function: printing
	* Brief:	record Task 
	* Param:	_para	: N.A.				
	* Retval:	
	* Date:		2012-08-08
	------------------------------------------------------------------------------*/
	void* printing(void* _para){
		//initialize the udp
		int32 iSendSize = 0;  
		/* create a socket */
		int32 sockudpsend;  

		sockudpsend = socket(AF_INET, SOCK_DGRAM, 0);  

		/* init servaddr */
		struct sockaddr_in addrSrv;  

		memset(&addrSrv,0, sizeof(addrSrv));


		addrSrv.sin_family = AF_INET;

		addrSrv.sin_addr.s_addr = inet_addr(ADDR_SRV);//htonl(INADDR_ANY); //  
		addrSrv.sin_port = htons(PORT_SRV);
		int32 i=0;
		logInfo_t dataBuffer;
		
		printf(">>>INFO Ready For Log ...\n");
		
		while(true){
			if(m_iMqdesSrv==-1){//msgQ DS is invalid ,waiting 10s then retry
				printf("###error, The Log Srv connect channle is not created\n");
				sleep(10);
				continue;
			}
			int32 iRetry = 10;
			int32 iRetval = -1;

			do
			{				
				iRetry--;
				iRetval = mq_receive( m_iMqdesSrv, (char*)&dataBuffer, sizeof(dataBuffer), 0); //not negative		
				if ( (iRetval == -1) && (errno==EINTR) )
					(void)napms(1); // try again because mq_send was interupted by a signal
				else
					iRetry = 0;
			} while (iRetry != 0);

			if (iRetval < 0)
			{
				printf("error produce %d, error no %d\n",iRetval,errno);
				perror("error ");
			}else if(iRetval>0){
			    //printf("Check Origin %s\n",dataBuffer.origin);
    			if(!PassFilter(dataBuffer.origin))
    			{
                    continue;
    			}
				if(DeBugLocal){
					if(ShouldCreateNew(sFileName)){
						GetFileName(sFileName);
						if(0!=CreateFile(sFileName))
						{
							break;
						}
					}
				}

				//<30>Oct 9 22:33:20 hlfedora auditd[1787]: The audit daemon is exiting.

				char bufToBeLog[512];
#if 0
				char sysLogGenTm[15];
				memset(sysLogGenTm,0,sizeof(sysLogGenTm));				
				struct timeb timebuf;			
				ftime( &timebuf );
				/* Note that we're cutting ctime( &timebuf.time ) off
				* after 15 characters to avoid the
				* \n that ctime() appends to the
				* formatted time string.
				*/
				strncpy(sysLogGenTm,ctime( &timebuf.time )+4,sizeof(sysLogGenTm));
#endif
                //GH Modify this function into safe way
				//      strncpy(sysLogGenTm,"Oct 9 22:33:20 ",sizeof(sysLogGenTm));
				/*                  <Facility and Severity>Generated Origin Tag: LogText */
				//sprintf(bufToBeLog,"<%d>%.19s %s %s[%d]: {%08d}",
				sprintf(bufToBeLog,"<%d>%s %s %s[%d]: {%08d}",
					SYSLOG_FACILITY+dataBuffer.logSeverity,
					dataBuffer.GenTm,
					dataBuffer.origin,
					dataBuffer.strModuleName,
					dataBuffer.logLine,i++);
                //printf("TobeLog %s\n",bufToBeLog);
                const int32 emptyRoom = sizeof(bufToBeLog)-strnlen(bufToBeLog,sizeof(bufToBeLog));
                strncat(bufToBeLog,dataBuffer.log,emptyRoom-1);
                const ulong bufToBeLog_Len = strnlen(bufToBeLog,sizeof(bufToBeLog));
                bufToBeLog[sizeof(bufToBeLog)-1] = 0;//make sure the NULL-Terminated
				const uint8 logSeverity = dataBuffer.logSeverity;

				if(DeBugRemote){
					//send by UDP
					iSendSize = sendto(sockudpsend, bufToBeLog, bufToBeLog_Len, 0,  
						(struct sockaddr *) &addrSrv, sizeof(struct sockaddr));
				}
				// log on local file
				if(DeBugLocal){

					uint8 logFileMsgPos = 0U;


					//match the msg text used the MARK_FIND_MSG
					logFileMsgPos = strcspn(bufToBeLog,MARK_FIND_MSG)+sizeof(MARK_FIND_MSG)+1;
					if (logFileMsgPos>=bufToBeLog_Len)
					{
						printf("### Syslog error:%d\n",logFileMsgPos);
						logFileMsgPos = 0;
					}

					(void)fprintf(stream,&bufToBeLog[logFileMsgPos]);
					(void)fflush(stream);	
				}
				memset(dataBuffer.log,0,sizeof(dataBuffer.log));
			}
		}
		close(sockudpsend);
		return NULL;
	}
}
int32 main() 
{   
	LoadCfg();
	int32 iRetry = 10;
	int32 iUnlinkRet = -1;
	bool ifContinue = true;
	do
	{
		iRetry--;
		iUnlinkRet = mq_unlink(IDC_LOG_CHANNEL); 

		if ( (iUnlinkRet == -1) && ((errno==EINTR) || (errno==EAGAIN)) )
			(void)IDC::napms(1); // try again because mq_unlink was interupted by a signal
		else
			iRetry = 0;
	} while (iRetry != 0);
	mq_attr tMqAttr;		//## attribute tMqAttr
	tMqAttr.mq_maxmsg =  IDC::QUEUE_SZ;
	tMqAttr.mq_msgsize = sizeof(IDC::logInfo_t);


	if(iUnlinkRet==-1){
		if(ENOENT!=errno){
			printf("###warning, Unlink MsgQ %s fail, error no %d\n",IDC_LOG_CHANNEL,errno);
			perror("Unlink:");
			ifContinue = false;
		}
	}
	if(ifContinue){
		ChangeMQLimit(MAX_MQ_SZ);
		iRetry = 10;
		do
		{
			iRetry--;
			IDC::m_iMqdesSrv = mq_open(IDC_LOG_CHANNEL,
				( O_RDWR | O_CREAT|O_EXCL ),
				(S_IRWXU|S_IRWXG|S_IRWXO),
				&tMqAttr
				);

			if ((IDC::m_iMqdesSrv == -1) && (errno==EINTR))
				(void)IDC::napms(1); // try again because mq_open was interupted by a signal
			else
				iRetry = 0;
		}  while (iRetry != 0);

		if(IDC::m_iMqdesSrv==-1){
			printf("###error, Create MsgQ %s fail, error no %d\n",IDC_LOG_CHANNEL,errno);
			perror("mq_open:");
		}
	}
	pthread_attr_t attr;    
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED );

	pthread_t tidSrv;
	pthread_create(&tidSrv, &attr, IDC::printing, NULL );


	while(true){
		sleep(10);
	}
}
