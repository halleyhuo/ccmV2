/*
 * ExtComm.h
 *
 *  Created on: 2017.07.29
 */

#ifndef __EXT_COMM_H__
#define __EXT_COMM_H__

#include "Types.h"
#include "xtpobj.h"


typedef enum 
{
	ExtLinkManager,
	ExtLinkSimu,
	ExtLinkSize,
}ExtLinkIdx;

 
void InitExtComm();
void* ExtRcv(void* _para);
void SendExtMsg(void* data, const uint16 _len, const uint16 extType = ExtID_ObjFlow_Info, const ExtLinkIdx linkIdx = ExtLinkManager);


void RecordXTP(const char* const fmt, ...);


#endif /* __EXT_COMM_H__ */

