/**********************************************************************
 *
 * Copyright:
 *		Imaging dynamic Company
 *
 * FileName:
 *		XGenTypes.h
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

#ifndef __X_GEN_TYPES_H__
#define __X_GEN_TYPES_H__

#include "Types.h"



#define X_CMD_GEN_MODE					101
#define X_CMD_GEN_FPS					102

#define X_CMD_GEN_MAGNIFY				103
#define X_CMD_GEN_DOSE					104
#define X_CMD_GEN_ROAD					105
#define X_CMD_GEN_SUB					106
#define X_CMD_GEN_FLUORO				107
#define X_CMD_GEN_SINGLE_IMAGE			108


#define X_CMD_GEN_KV_PLUS				111
#define X_CMD_GEN_KV_MINUS				112
#define X_CMD_GEN_MA_PLUS				113
#define X_CMD_GEN_MA_MINUS				114


typedef uint32			XCmdGenMode;

#define X_CMD_GEN_MODE_N				0
#define X_CMD_GEN_MODE_C				1
#define X_CMD_GEN_MODE_P				2
#define X_CMD_GEN_MODE_S				3


typedef uint32			XCmdGenFps;

#define X_CMD_GEN_FPS_N					0
#define X_CMD_GEN_FPS_5					1
#define X_CMD_GEN_FPS_10				2
#define X_CMD_GEN_FPS_15				3
#define X_CMD_GEN_FPS_20				4
#define X_CMD_GEN_FPS_30				5



typedef uint32			XCmdGenId;
typedef uint32			XCmdGenParam;


typedef struct _XCmdGen
{
	XCmdGenId		xCmdGenId;
	XCmdGenParam	xCmdGenParam;
} XTP_DEF_PACKED XCmdGen;



typedef uint32			XReportGenId;
typedef uint32			XReportGenParam;

typedef struct _XReportGen
{
	XReportGenId	xReporGentId;
	XReportGenParam	xReportGenParam;
} XTP_DEF_PACKED XReportGen;


#endif

