#include "LogTrace.h"

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

namespace IDC
{
	/******************************************************************************/
	/********************* Below parameters can be changed-> **********************/
	/******************************************************************************/	

	//lock which make sure the print task will be only create once
	static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;	
	
    static char originName[MAX_ORIGIN_NAME_LEN] = {0};//Will be overide in InitLog()
	static int32 HeaderLen = 0;//Will be overide in InitLog()

	inline int32 napms( int32 uiDelay){
		for(uint64 i=uiDelay*10000;i>0;i--){}
		return 0;
	}
	
	static void trigPath(char pathName[], char rltBuff[], const int32 BuffLen)
	{
		if(NULL!=pathName){
    		char * pch;
    		pch=strrchr(pathName,'/');
    		if(NULL!=pch){
    		    strncpy(&rltBuff[0],&pch[1],BuffLen); 
    	    }else{
    		    strncpy(&rltBuff[0],pathName,BuffLen); 
    	    }
		    rltBuff[BuffLen-1] = '\0';
		}
	}
	
	static char selfProgName[32] = "";

	static void program_path()
	{    
		char pathName[128];
		memset(pathName,0,sizeof(pathName));
	    
	        if (readlink("/proc/self/exe", pathName, sizeof(pathName)) != -1) {
	        	memset(selfProgName,0,sizeof(selfProgName));                
	        	trigPath(pathName,selfProgName,sizeof(selfProgName));
                //printf("PathName %s EXE Name %s\n",pathName,selfProgName);
	        }      
	}

	static mqd_t iMqdes;
	int32 InitLog() 
	{   
		HeaderLen = sizeof(logInfo_t)-QUEUE_LENGTH;
		int32 iRetry = 10;

		do
		{
			iRetry--;
            //use NONBLOCK, prefer forgive debug infomation compare to BLOCK App
			iMqdes = mq_open(IDC_LOG_CHANNEL, O_WRONLY|O_NONBLOCK);
			if ( (iMqdes == -1) && (errno == EINTR) )
				(void)napms(1); // try again because mq_send was interupted by a signal
			else
				iRetry = 0;
		}  while (iRetry != 0);

		if (iMqdes == -1)
		{
			//printf( "open mq failed, check if the Log Server Already Start via command \"ps\" or check msgQ file\n" );
			return -1;
		}else{
			program_path();
			memset(&originName[0],0,sizeof(originName));
			strncpy(&originName[0],selfProgName,sizeof(originName));		
			originName[sizeof(originName)-1] = 0;				
			return 0;
		}		
	}

	static bool LinkmsgQSrv = false;
	static bool AbortLogasSrv = false;
	
	static bool MarkSureOneInstanse = false;
	static pthread_mutex_t mutexLogM = PTHREAD_MUTEX_INITIALIZER;	
	
	static void* LogMTask(void* _para){
		int32 sockfd,len;  
	    struct sockaddr_in addr;  
	    //int32 addr_len = sizeof(struct sockaddr_in); 
	    socklen_t addr_len = sizeof addr;
	     
	    logM_t buffer;
	    
	    /*建立socket*/  
	    if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0){  
	        perror ("socket");  
	        return 0;  
	    }  
	    /*填写sockaddr_in 结构*/  
	    bzero ( &addr, sizeof(addr) );  
	    addr.sin_family=AF_INET;  
	    addr.sin_port=htons(PORT);  
	    addr.sin_addr.s_addr=htonl(INADDR_ANY) ;  
	    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr))<0){  
	        perror("connect");  
	        return 0;  
	    }  
	    
	    while(true){  
	        memset(&buffer,0,sizeof(buffer));  
	        len = recvfrom(sockfd,&buffer,sizeof(buffer), 0 , (struct sockaddr *)&addr ,&addr_len);  
	        /*显示client端的网络地址*/  
	        printf("receive Log M from %s\n" , inet_ntoa( addr.sin_addr)); 
	        if(buffer.commandPassword = 0x6688){
	        }else{
	        	printf("Log Manager Command Recv but password fail 0x%x\n",buffer.commandPassword);
	        }
	    }
		return 0;
	}
	
	void InitLogM()
	{
		//DCLP to make sure this function will only be executed once
		if(!MarkSureOneInstanse){
			if(0==pthread_mutex_lock( &mutexLogM )){
				if(!MarkSureOneInstanse){
					pthread_attr_t attr;    
					pthread_attr_init(&attr);
					pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED );
				
					pthread_t tidClientM;
					pthread_create(&tidClientM, &attr, LogMTask, NULL );
				}
				pthread_mutex_unlock( &mutexLogM );
			}
		}
	}

	/*------------------------------------------------------------------------------
	* Function: Log
	* Brief:	The Interface used to call to record the trace log
	* Param:	level	: trace log's level
	*           pathName: File name the code call this function,you can give __FILE__
	*           _line   : line number the code call this funtion,you can give __LINE__
	*           fmt		: format output the trace message
	* Retval:	
	* Date:		2016-02-03
	------------------------------------------------------------------------------*/
	void Log(const int32 level,
		char pathName[],
		const uint64 _line, 
		const char* const fmt, ...)
	{	
		if(AbortLogasSrv){			
			return;
		}
		if( (level<debug_level_local) || (level<debug_level_remote) ){
			//DCLP Pattern to Link to the one msgServer
			if(!LinkmsgQSrv){
				pthread_attr_t attr;
				sched_param param;
				if(0==pthread_mutex_lock( &mutex )){
					if(!LinkmsgQSrv){
						if(0 == InitLog()){
							LinkmsgQSrv = true;
						}
					}

					pthread_mutex_unlock( &mutex );
				}
			}

			if(LinkmsgQSrv){
                //Local Buffer to format the Log
                logInfo_t LogFrame;
				
				//1. Record level
				LogFrame.logSeverity = level;
				
				//2. Record Origin
				strncpy(LogFrame.origin,originName,MAX_ORIGIN_NAME_LEN-1);
                //printf("origin %s\n",LogFrame.origin);
                LogFrame.origin[MAX_ORIGIN_NAME_LEN-1]=0;//Make Sure null-terminated

				//3. Record file name, trig the filePath if it's too long
				if(NULL==pathName){
					strcpy(&LogFrame.strModuleName[0],"UNKNOWN");					
				}else{						
	  				trigPath(pathName,&LogFrame.strModuleName[0],MAX_MODULE_NAME_LEN);							
				}

				//4. Record line of the source file
				LogFrame.logLine = _line;

				//5. Generate Time
				char sysLogGenTm[MAX_GEN_TIME_BUF_SZ];
				memset(sysLogGenTm,0,MAX_GEN_TIME_BUF_SZ);				
				struct timeb timebuf;			
				ftime( &timebuf );
				/* Note that we're cutting ctime( &timebuf.time ) off
				* after 15 characters to avoid the
				* \n that ctime() appends to the
				* formatted time string.
				*/
				strncpy(sysLogGenTm,ctime( &timebuf.time )+4,15);
				char msBuf[12];
				memset(msBuf,0,sizeof(msBuf));
				sprintf(msBuf,".%03d",timebuf.millitm);
				strncat(sysLogGenTm,msBuf,4);
				
				//printf("\n%s\n",sysLogGenTm);
				memcpy(LogFrame.GenTm,sysLogGenTm,MAX_GEN_TIME_BUF_SZ);
				
				//6. Record Log text
				va_list argptr;	
				va_start(argptr, fmt);
				(void)vsnprintf(&LogFrame.log[0], QUEUE_LENGTH, fmt, argptr);
				va_end(argptr);
				//make sure the rear must be 0
				LogFrame.log[QUEUE_LENGTH-1]=0;				

				int32 iRetry = 5;
				int32 iRetval;
				do
				{
					iRetry--;
					iRetval = mq_send( iMqdes, (char*)&LogFrame, strlen(LogFrame.log)+HeaderLen+1, 0 );
					
					if ( (iRetval == -1) && (errno==EINTR) )
						(void)napms(1); // try again because mq_send was interupted by a signal
					else
						iRetry = 0;
				} while (iRetry != 0);
				if ( iRetval == -1 ){          
					perror( "Log_mq_send" );
                    LinkmsgQSrv = false;
				}
			}
		}
	}
}

