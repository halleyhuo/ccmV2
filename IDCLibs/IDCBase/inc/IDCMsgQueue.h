
#ifndef __IDC_MSGQUEUE_H__
#define __IDC_MSGQUEUE_H__

#include <mqueue.h>
#include <time.h>
#include "Types.h"

class CIDCMsgQueue 
{

public:

	CIDCMsgQueue();
	virtual ~CIDCMsgQueue();
	virtual const CIDCMsgQueue & operator=(const CIDCMsgQueue & right);
	mqd_t openCreate(const char* pName, int32 lNrMsg, int32 lMsgSize, int32 iAccessMode = O_RDWR, bool fExclusive = true);
	int32 destroy();

	mqd_t open(const char * pName, int32 iAccessMode = O_RDWR);
	int32 close();

	int32 send(const void* pMsg, size_t tLength, uint32 uiPrio = 0);
	int32 recv(void* pMsg, const size_t bufLen, int32 lTimeout = NO_TIMEOUT, uint32 * pPrio = NULL);
	int32 getattr(mq_attr * pAttr);
	int32 setattr(const mq_attr * pMqstat, mq_attr * pOmqstat = NULL);

	inline IDCErrnoType GetIDCErrno() const;

protected :

	mqd_t iMqdes;		//## attribute iMqdes
	char * pQueueName;		//## attribute pQueueName
	// Attributes of the message queue
	// 
	mq_attr tMqAttr;		//## attribute tMqAttr

	// Local errno variable.
	IDCErrnoType m_ErrNo;		//## attribute m_ErrNo
};

inline IDCErrnoType CIDCMsgQueue::GetIDCErrno() const {
	//#[ operation GetIDCErrno() const

	return m_ErrNo;
	//#]
}

#endif
