/**********************************************************************
 *
 * Copyright:
 *		Imaging dynamic Company
 *
 * FileName:
 *		GenComm.h
 *
 * Description:
 *
 *
 * Author:
 *		Halley
 *
 * Create Date:
 *		2018/6/3
 *
 */

#ifndef __GEN_COMM_H__
#define __GEN_COMM_H__

#include "Types.h"
#include <pthread.h>


typedef int32(*GenCommCallback)(char * str, uint32 len);


class CGenComm
{

public:
	CGenComm(GenCommCallback genCommCallback);

	virtual ~CGenComm();

	BOOL Start(void);

	static void * GenCommRecv(void * args);
	int32 GenCommSend(char * str, uint32 len);

private:
	int32 OpenDev(const char *dev);

private:
	GenCommCallback			m_genCommCallback;

};

#endif

