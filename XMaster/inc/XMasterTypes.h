/**********************************************************************
 *
 * Copyright:
 *		Imaging dynamic Company
 *
 * FileName:
 *		XMasterTypes.h
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

#ifndef __IDC_X_MASTER_TYPE_H__
#define __IDC_X_MASTER_TYPE_H__

#include "Types.h"
#include "IDCBase/inc/IDCThread.h"
#include "TuiTypes.h"
#include "GenTypes.h"
#include "XGenTypes.h"

#define IDC_TASK_XMASTER			1
#define IDC_TASK_XTUI				2
#define IDC_TASK_XGENERATOR			3


/* transmission TUI command from XMaster to XTui */
#define TRANS_TUI_CMD_SETS			1
/* the frame from XMaster to XTui*/
typedef struct _TransTuiCmdFrame
{
	ITCObjHeader	header;
	TuiCmdSets		tuiCmdSets;
} TransTuiCmdFrame;

/* transmission from XTui to XMaster */
#define TRANS_TUI_REPORT_SETS		2
/* the frame from XTui to XMaster */
typedef struct _TransTuiReportFrame
{
	ITCObjHeader	header;
	TuiReportSets	tuiReportSets;
} TransTuiReportFrame;

/* transmission from XMaster to XGenerator */
#define TRANS_REPORT_GEN			11
/* the frame from XMaster to XGenerator */
typedef struct _TransReprotGenFrame
{
	ITCObjHeader	header;
	GenReport		genReport;
} TransReportGenFrame;

/* transmission from XGenerator to XMaster */
#define TRANS_CMD_GEN				12
/* the frame from XGenerator to XMaster */
typedef struct _TransCmdGenFrame
{
	ITCObjHeader	header;
	GenCmd			genCmd;
} TransCmdGenFrame;

/* internal transmission to XGenerator (E.g from XTui to XGenerator) */
#define X_CMD_GEN					15
/* the frame transmission to XGenerator in internal thread */
typedef struct _XCmdGenFrame
{
	ITCObjHeader	header;
	XCmdGen			xCmdGen;
} XCmdGenFrame;


#define X_REPORT_GEN				16

typedef struct _XReportGenFrame
{
	ITCObjHeader	header;
	XReportGen		xReportGen;
} XReportGenFrame;


#endif /*__IDC_X_MASTER_TYPE_H__*/

