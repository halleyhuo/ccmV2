/**********************************************************************
 *
 * Copyright:
 *		Imaging dynamic Company
 *
 * FileName:
 *		IDCXTaskID.h
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

#ifndef __IDC_X_TASK_ID_H__
#define __IDC_X_TASK_ID_H__

#include "Types.h"
#include "TuiTypes.h"
#include "GenTypes.h"

#define IDC_TASK_XMASTER			1
#define IDC_TASK_XTUI				2
#define IDC_TASK_XGENERATOR			3




/* transmission TUI command from XMaster to XTui */
#define TRANS_TUI_CMD				0
/* the frame from XMaster to XTui*/
typedef struct _TransTuiCmdFrame
{
	ITCObjHeader	header;
	TuiCmd			tuiCmd;
} TransTuiCmdFrame;

/* transmission from XTui to XMaster */
#define TRANS_TUI_REPORT			1
/* the frame from XTui to XMaster */
typedef struct _TransTuiReportFrame
{
	ITCObjHeader	header;
	TuiReport		tuiReport;
} TransTuiReportFrame;

/* transmission from XMaster to XGenerator */
#define TRANS_GEN_REPORT			10
/* the frame from XMaster to XGenerator */
typedef struct _TransGenReprotFrame
{
	ITCObjHeader	header;
	GenReport		genReport;
} TransGenReportFrame;

/* transmission from XGenerator to XMaster */
#define TRANS_GEN_CMD				11
/* the frame from XGenerator to XMaster */
typedef struct _TransGenCmdFrame
{
	ITCObjHeader	header;
	GenCmd			genCmd;
} TransGenCmdFrame;

/* internal transmission to XGenerator (E.g from XTui to XGenerator) */
#define X_GEN_CMD					15
/* the frame transmission to XGenerator in internal thread */
typedef struct _XGenCmdFrame
{
	ITCObjHeader	header;
	XGenCmd			xGenCmd;
} XGenCmdFrame;


#define X_GEN_REPORT				16

typedef struct _XGenReportFrame
{
	ITCObjHeader	header;
	XGenReport		xGenReport;
} XGenReportFrame;


#endif /*__IDC_X_TASK_ID_H__*/

