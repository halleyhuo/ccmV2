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


typedef uint16			TuiCmdId;


/* param TuiFluoroMode is valid */
#define	TUI_CMD_FLUORO_MODE			100

/* param TuiFluoroVAS is valid*/
#define TUI_CMD_FLUORO_VAS			200

/* param TuiKvValue is valid*/
#define TUI_CMD_FLUORO_KV			211
/* param TuiMsValue is valid*/
#define TUI_CMD_FLUORO_MS			212
/* param TuiMaValue is valid*/
#define TUI_CMD_FLUORO_MA			213
/* param TuiMasValue is valid*/
#define TUI_CMD_FLUORO_MAS			214

/* param TuiFluoroFps is valid*/
#define TUI_CMD_FLUORO_FPS			300

/* param TuiFluoroDose is valid*/
#define TUI_CMD_FLUORO_DOSE			400



typedef uint8	TuiFluoroMode;

#define TUI_FLUORO_MODE_N		0
#define TUI_FLUORO_MODE_C		1
#define TUI_FLUORO_MODE_P		2
#define TUI_FLUORO_MODE_S		3



typedef uint8	TuiFluoroVAS;

#define TUI_CMD_FLUORO_VAS_PLUS		1
#define TUI_CMD_FLUORO_VAS_MINUS	2


typedef KvValueType		TuiKvValue;

typedef MsValueType		TuiMsValue;

typedef MaValueType		TuiMaValue;

typedef MasValueType	TuiMasValue;


typedef uint8	TuiFluoroFps;

#define TUI_FLUORO_FPS_N		0
#define TUI_FLUORO_FPS_5		1
#define TUI_FLUORO_FPS_10		2
#define TUI_FLUORO_FPS_15		3
#define TUI_FLUORO_FPS_20		4
#define TUI_FLUORO_FPS_30		5



typedef uint8	TuiFluoroDose;

#define TUI_FLUORO_DOSE_N		0
#define TUI_FLUORO_DOSE_1		1
#define TUI_FLUORO_DOSE_2		2
#define TUI_FLUORO_DOSE_3		3




typedef struct _TuiCmd
{
	TuiCmdId		tuiCmdId;
	union
	{
		TuiFluoroMode		tuiFluoroMode;
		TuiFluoroVAS		tuiFluoroVAS;
		TuiKvValue			tuiKvValue;
		TuiMaValue			tuiMaValue;
		TuiMsValue			tuiMsValue;
		TuiMasValue			tuiMasValue;
		TuiFluoroFps		tuiFluoroFps;
		TuiFluoroDose		tuiFluoroDose;
	} tuiCmdParams;
} XTP_DEF_PACKED TuiCmd;




typedef uint16			TuiReportId;


typedef struct _TuiReportGenCfm
{
	TuiFluoroMode		tuiFluoroMode;
	TuiKvValue			tuiKvValue;
	TuiMsValue			tuiMsValue;
	TuiMaValue			tuiMaValue;
	TuiMasValue			tuiMasValue;
	TuiFluoroFps		tuiFluoroFps;
	TuiFluoroDose		tuiFluoroDose;
} TuiReportGenCfm;



typedef struct _TuiReport
{
	TuiReportId		tuiReportId;
	union 
	{
		TuiReportGenCfm		tuiReportGenCfm;
	} tuiReportParams;
} XTP_DEF_PACKED TuiReport;




#endif

