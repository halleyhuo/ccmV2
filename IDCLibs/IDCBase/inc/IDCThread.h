#ifndef __IDC_THREAD_H__
#define __IDC_THREAD_H__


#include <mqueue.h>
#include <string.h>

#include "Types.h"
#include "IDCMsgQueue.h"
//class CIDCMsgQueue;

#define IDC_MAX_NAME_LENGTH			32

#define IDC_TASK_NONE				0

#define IDC_TASK_SIZE				255

typedef uint16						IDCTaskId;


typedef struct
{
	IDCTaskId		taskIdent;
	CIDCMsgQueue	*msgQRcv;
} IDCItcMap;

class CIDCThread
{
public:
	enum StackSize
	{
		/**
		 * Small stack size.
		 */
		Small	= PTHREAD_STACK_MIN,
		/**
		 * Medium stack size.
		 */
		Medium	= STACK_SIZE_DEFAULT,
		/**
		 * Large stack size.
		 */
		Large	= 0x100000
	};

	enum TaskPriority
	{
		/**
		 * Realtime task priority.
		 */
		Realtime	= 9,
		/**
		 * Highest task priority.
		 */
		Highest		= 10,
		/**
		 * Very high task priority.
		 */
		 VeryHigh	= 11,
		/**
		 * Higher task priority.
		 */
		Higher		= 12,
		/**
		 * High task priority.
		 */
		High		= 13,
		/**
		 * Moderate task priority.
		 */
		Moderate	= 14,
		/**
		 * Low task priority.
		 */
		Low			= 15,
		/**
		* Lower task priority.
		 */
		Lower		= 16,
		/**
		 * Very low task priority.
		 */
		VeryLow		= 17,
		/**
		 * Lowest task priority.
		 */
		Lowest		= 18
	};

public :
	static int32 ITCSend(const ITCObjHeader*);

	CIDCThread(const char threadName[IDC_MAX_NAME_LENGTH]	= NULL,
				IDCTaskId threadIdent						= IDC_TASK_NONE,
				StackSize stackSize							= Small,
				TaskPriority priority						= Low,
				bool _ifRcvITC								= false);

	virtual ~CIDCThread();

	//## operation run()
	virtual void run() = 0;

	//## operation setDetachState(int32)
	int32 setDetachState(int32 iDetachState);

	//## operation setPriority(int32)
	int32 setPriority(int32 iPrio);

	//## operation setStack(char*,int32)
	//int32 setStack(char* pStack, int32 iStackSize);

	//## operation waitForThreadEnd()
	void waitForThreadEnd(); 

	inline const IDCErrnoType getIDCErrno() const;

	//## operation kill()
	virtual void kill();

	bool Init();

protected :
	//## operation execute()
	pthread_t execute(); 

	//## operation start(void *)
	static void * start(void * pThread);

	static CIDCMsgQueue* GetITCRcvMsgQ(const IDCTaskId);

	pthread_t tID;				//## attribute tID

	pthread_attr_t myAttr;		//## attribute myAttr

	IDCErrnoType m_ErrNo;		//## attribute m_ErrNo

	pthread_t tJoinID;			//## attribute tJoinID

	char taskName[IDC_MAX_NAME_LENGTH];
	IDCTaskId taskIdent;
	bool ifRecvITC;
	CIDCMsgQueue* m_msgQueue;	//will be instantiate  only if ifJoinIPC=true

	static IDCItcMap itcMap[IDC_TASK_SIZE];

private:

	void killThread();
};

// Class CIDCThread

//## Get and Set Operations for Class Attributes (inline)

inline const IDCErrnoType CIDCThread::getIDCErrno() const {
	return m_ErrNo;
}

#endif
