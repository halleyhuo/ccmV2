
#ifndef __IDC_ALARM_H__
#define __IDC_ALARM_H__

#include <signal.h>
#include <time.h>

#include "Types.h"

typedef void (*AlarmTimeout)(void*);
#define MAX_DELAYED_MESSAGES		128

typedef struct
{
	bool			bInUse;
	uint32			tIdent;
	AlarmTimeout	handler;
	void			*param;
	timer_t			tTimerId;
	sigevent		tEvent;
	bool			bLoop;
} tinyAlarm_t;

void start_alarm();

bool create_alarm( uint16 elapseMs, AlarmTimeout handler, void* param, uint32 &newId, bool bLoop);
bool delete_alarm( uint32 id );


class CIdcAlarm 
{
public:
	CIdcAlarm();
	static CIdcAlarm* GetInstance();
	static CIdcAlarm* CreateInstance();
	static CIdcAlarm* m_pSingleton;			// singleton instance
	virtual ~CIdcAlarm();

	bool CreateAlarm( uint16 elapseMs, AlarmTimeout handler, void*, uint32 &newId, bool bLoop);

	bool DeleteAlarm( uint32 id );

	uint32				m_tUniqueAlarmId;	//0 is an invalid Id

	static tinyAlarm_t	m_asDelayedMsg[MAX_DELAYED_MESSAGES];

private:

};


#endif


