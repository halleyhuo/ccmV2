/**********************************************************************
 *
 * Copyright:
 *		Imaging dynamic Company
 *
 * FileName:
 *		XtpNode.h
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

#include "IDCXTP/inc/xtpobj.h"
#include "TuiTypes.h"
#include "GenTypes.h"


#define TUI_CMD_SETS			0x0301


typedef struct _TuiCmdFrame
{
	tXtpObjHeader	header;
	TuiCmdSets		tuiCmdSets;
} XTP_DEF_PACKED TuiCmdFrame;


#define TUI_REPORT_SETS			0x0302


typedef struct _TuiReportFrame
{
	tXtpObjHeader	header;
	TuiReportSets	tuiReportSets;
} XTP_DEF_PACKED TuiReportFrame;



#define GEN_CMD					0x3011

typedef struct _GenCmdFrame
{
	tXtpObjHeader	header;
	GenCmd			genCmd;
} XTP_DEF_PACKED GenCmdFrame;


#define GEN_REPORT				0x0312

typedef struct _GenReportFrame
{
	tXtpObjHeader	header;
	GenReport		genReport;
} XTP_DEF_PACKED GenReportFrame;


