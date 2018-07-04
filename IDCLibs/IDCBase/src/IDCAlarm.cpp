#include "Types.h"
#include "IDCAlarm.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <errno.h>


static pthread_mutex_t     tTimerMutex = PTHREAD_MUTEX_INITIALIZER;
static CIdcAlarm* pAlarmTask;


CIdcAlarm* CIdcAlarm::m_pSingleton = NULL;
tinyAlarm_t CIdcAlarm::m_asDelayedMsg[MAX_DELAYED_MESSAGES];


CIdcAlarm* CIdcAlarm::GetInstance()
{
	return CreateInstance();
}

CIdcAlarm* CIdcAlarm::CreateInstance()
{
	if (m_pSingleton == NULL)
	{
		m_pSingleton = new CIdcAlarm();
	}
	return m_pSingleton;
}

static void handler(int32 sig, siginfo_t *si, void *uc)
{     
	pthread_mutex_lock(&tTimerMutex);

	/* get the message nr */
	const int16 m_iNr = si->si_int; 


	if ((m_iNr >= 0) && (m_iNr < MAX_DELAYED_MESSAGES)) 
	{		
		if(CIdcAlarm::m_asDelayedMsg[m_iNr].handler!=NULL){
			CIdcAlarm:: m_asDelayedMsg[m_iNr].handler(CIdcAlarm::m_asDelayedMsg[m_iNr].param);
		}

		if (!CIdcAlarm::m_asDelayedMsg[m_iNr].bLoop) {
			if (timer_delete(CIdcAlarm::m_asDelayedMsg[m_iNr].tTimerId)) {
				perror("###error alarm_tsk: ");
			}

			CIdcAlarm::m_asDelayedMsg[m_iNr].bInUse = false;
			CIdcAlarm::m_asDelayedMsg[m_iNr].tIdent = 0;
		}
	}
	pthread_mutex_unlock(&tTimerMutex);


}


CIdcAlarm::CIdcAlarm()
{
	m_tUniqueAlarmId = 1;

	//Init array
	for (int16 m_iNr = 0; m_iNr < MAX_DELAYED_MESSAGES; m_iNr++) {
		m_asDelayedMsg[m_iNr].bInUse = false;
		m_asDelayedMsg[m_iNr].tIdent = 0;
		//m_asDelayedMsg[m_iNr].bHandleFinish = true;
	}
	struct sigaction sa;
	/* Establish handler for timer signal */

	//printf("Establishing handler for signal %d\n", SIGUSR1);
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = handler;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGUSR1, &sa, NULL) == -1)
		perror("sigaction");

}


CIdcAlarm::~CIdcAlarm()
{

}


//user can select semaphore or IPC telegram for alarm notify, but only one of them
//if the AlarmSem is not NULL, then we think a semaphore alarm used;
//else an IPC telegram alarm used;
bool CIdcAlarm::CreateAlarm( uint16 elapseMs,
	AlarmTimeout handler, void* param, uint32 &newId, bool bLoop)
{

	int16               m_iNr;
	struct itimerspec   tTime;

	//It is not allowed to create alarm with receiver 0 (SEND_INDIREKT).
	//This would cause this INT_ALARM to be sent to all tasks
	//which are registered to receive INT_ALARM

	pthread_mutex_lock( &tTimerMutex );

	for (m_iNr = 0; m_iNr < MAX_DELAYED_MESSAGES; m_iNr++)
	{
		if (!m_asDelayedMsg[m_iNr].bInUse)
		{
			break;
		}
	}

	if (m_iNr == MAX_DELAYED_MESSAGES)
	{
		printf("###error create_alarm: MAX_DELAYED_MESSAGES!!!\n");
		pthread_mutex_unlock( &tTimerMutex );
		return false;
	}
	if(NULL!=handler){
		m_asDelayedMsg[m_iNr].param = param;
		//alarm semphore used to notify the application that alarm ring should be created in customer side
		m_asDelayedMsg[m_iNr].handler= handler;
	}    

	//struct sigevent evp;
	//memset(&m_asDelayedMsg[m_iNr].tEvent, 0, sizeof(struct sigevent));
	m_asDelayedMsg[m_iNr].tEvent.sigev_value.sival_int = m_iNr;

	m_asDelayedMsg[m_iNr].tEvent.sigev_notify = SIGEV_SIGNAL; //to save resouce more than SIGEV_THREAD           
	//m_asDelayedMsg[m_iNr].tEvent.sigev_notify_function = CIdcAlarm::OnTimer; 
	m_asDelayedMsg[m_iNr].tEvent.sigev_signo  = SIGUSR1;
	//m_asDelayedMsg[m_iNr].tEvent._sigev_un._tid = alarmPid;

	if(timer_create(CLOCK_REALTIME, &m_asDelayedMsg[m_iNr].tEvent, &m_asDelayedMsg[m_iNr].tTimerId))
	{
		perror("create_alarm: \n");		
		pthread_mutex_unlock( &tTimerMutex );
		return false;
	}

	tTime.it_value.tv_sec       = elapseMs / 1000;
	tTime.it_value.tv_nsec      = (elapseMs % 1000) * 1000000;
	tTime.it_interval.tv_sec    = 0;
	tTime.it_interval.tv_nsec   = 0;

	if (bLoop)
	{
		m_asDelayedMsg[m_iNr].bLoop = bLoop;
		tTime.it_interval.tv_sec    = elapseMs / 1000;
		tTime.it_interval.tv_nsec   = (elapseMs % 1000) * 1000000;
	}

	timer_settime(m_asDelayedMsg[m_iNr].tTimerId, 0, &tTime, NULL);

	/* Everything is OK, so set as valid */
	m_asDelayedMsg[m_iNr].bInUse            = true;
	newId = m_asDelayedMsg[m_iNr].tIdent    = m_tUniqueAlarmId++;

	pthread_mutex_unlock( &tTimerMutex );

	return true;
}

bool CIdcAlarm::DeleteAlarm( uint32 id )
{
	int16   m_iNr;

	//0 is an invalid id
	if ( id == 0 )
	{
		//printf("delete_alarm: invalid id: %d\n", id);
		return false;
	}

	//printf("NEU!!delete_alarm %d\n", id );
	pthread_mutex_lock( &tTimerMutex );
	for (m_iNr = 0; m_iNr < MAX_DELAYED_MESSAGES; m_iNr++)
	{
		if ( (   m_asDelayedMsg[m_iNr].tIdent == id  )
			&&  m_asDelayedMsg[m_iNr].bInUse    )
		{
			break;
		}
	}
	if (m_iNr == MAX_DELAYED_MESSAGES)
	{
		//printf("delete_alarm: not found %d\n", id);
		pthread_mutex_unlock( &tTimerMutex );
		return false;
	}

	if (timer_delete(m_asDelayedMsg[m_iNr].tTimerId))
	{		
		perror("delete_alarm: timer_delete");
		pthread_mutex_unlock( &tTimerMutex );
		return false;
	}

	m_asDelayedMsg[m_iNr].bInUse    = false;
	m_asDelayedMsg[m_iNr].tIdent    = 0;
	m_asDelayedMsg[m_iNr].bLoop     = false;
	id = 0;
	//m_asDelayedMsg[m_iNr].bHandleFinish = true;

	pthread_mutex_unlock( &tTimerMutex );

	
	return true;
}

void start_alarm()
{
    
    pAlarmTask = CIdcAlarm::CreateInstance();
}

bool create_alarm( uint16 elapseMs,
	AlarmTimeout handler,void* param, uint32 &newId, bool bLoop)
{	

	if (CIdcAlarm::m_pSingleton)
	{
		return CIdcAlarm::GetInstance()->CreateAlarm( elapseMs, handler, param,newId, bLoop);
	}
	else
	{
		printf("###error create_alarm: alarmtask not initialized yet!\n");
		return false;
	}
}

bool delete_alarm( uint32 id )
{  
	if (CIdcAlarm::m_pSingleton)
	{
		return CIdcAlarm::GetInstance()->DeleteAlarm( id );
	}
	else
	{
		printf("###error delete_tAlarm fail\n");
		return false;
	}
}


