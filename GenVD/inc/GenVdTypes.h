/**********************************************************************
 *
 * Copyright:
 *		Imaging dynamic Company
 *
 * FileName:
 *		GenVdTypes.h
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

#ifndef __GEN_VD_TYPES_H__
#define __GEN_VD_TYPES_H__

#include "Types.h"

#define IDC_TASK_GENVDPROC			1

typedef enum
{
	RecvStateStart,
	RecvStateRecving,
	RecvStateEnd
} RecvState;

#define MAX_GEN_RECV_DATA_LEN		1024

typedef struct _GenRecvDataSt
{
	uint8			data[MAX_GEN_RECV_DATA_LEN];
	uint32			dataLen;
} GenRecvDataSt;


#endif /*__GEN_VD_TYPES_H__*/

