/**********************************************************************
 *
 * Copyright:
 *		Imaging dynamic Company
 *
 * FileName:
 *		TuiTypes.h
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


#ifndef __TUI_TYPES_H__
#define __TUI_TYPES_H__

#include "Types.h"
#include "GenTypes.h"

#define MAX_CMD_COUNTS		5
#define MAX_REPORT_COUNTS	5


/* param TuiFluoroMode is valid */
#define TUI_CMD_GEN_MODE				101
#define TUI_CMD_GEN_FPS					102

#define TUI_CMD_GEN_MAGNIFY				103
#define TUI_CMD_GEN_DOSE				104
#define TUI_CMD_GEN_ROAD				105
#define TUI_CMD_GEN_SUB					106
#define TUI_CMD_GEN_FLUORO				107
#define TUI_CMD_GEN_SINGLE_IMAGE		108


#define TUI_CMD_GEN_KV_PLUS				111
#define TUI_CMD_GEN_KV_MINUS			112
#define TUI_CMD_GEN_MA_PLUS				113
#define TUI_CMD_GEN_MA_MINUS			114


#define TUI_CMD_C_ARM_UP				201
#define TUI_CMD_C_ARM_DOWN				202

#define TUI_CMD_COL_UD_NARROW			301
#define TUI_CMD_COL_UD_EXPAND			302
#define TUI_CMD_COL_LR_NARROW			311
#define TUI_CMD_COL_LR_EXPAND			312




typedef uint8	TuiFluoroMode;

#define TUI_GEN_MODE_N					0
#define TUI_GEN_MODE_C					1
#define TUI_GEN_MODE_P					2
#define TUI_GEN_MODE_S					3



typedef uint8	TuiFluoroFps;

#define TUI_GEN_FPS_N					0
#define TUI_GEN_FPS_5					1
#define TUI_GEN_FPS_10					2
#define TUI_GEN_FPS_15					3
#define TUI_GEN_FPS_20					4
#define TUI_GEN_FPS_30					5



typedef uint32			TuiCmdId;
typedef uint32			TuiCmdParams;

typedef struct _TuiCmd
{
	TuiCmdId		tuiCmdId;
	TuiCmdParams	tuiCmdParams;
} XTP_DEF_PACKED TuiCmd;

typedef struct _TuiCmdSets
{
	uint8		counts;
	TuiCmd		tuicmds[MAX_CMD_COUNTS];
} XTP_DEF_PACKED TuiCmdSets;


typedef uint32			TuiReportId;
typedef uint32			TuiReportParams;

typedef struct _TuiReport
{
	TuiReportId		tuiReportId;
	TuiReportParams	tuiReportParams;
} XTP_DEF_PACKED TuiReport;

typedef struct _TuiReportSets
{
	uint8		counts;
	TuiReport	tuiReport[MAX_REPORT_COUNTS];
} XTP_DEF_PACKED TuiReportSets;


#endif

