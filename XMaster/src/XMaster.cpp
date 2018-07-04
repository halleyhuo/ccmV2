/**********************************************************************
*
* Copyright:
*	   Imaging dynamic Company
*
* FileName:
*	   XMaster.cpp
*
* Description:
*	   Class XGenerator
*
* Author:
*	   Halley
*
* Create Date:
*	   2018/6/3
*
*/
#include "XMaster.h"

#include "Types.h"
#include "IDCXTP/Client/inc/xtpClient.h"
#include "IDCXTaskID.h"
#include "XtpNode.h"



static void XMasterCallBackFunc(tXtpObjHeader * msg, void * para);

static void OnTuiCmd(TuiCmd * tuiCmd);

static void OnGenReport(GenReport * genReport);


BRODCAST_LIST broadcastList[] = 
{    
	{XTP_NODE_MASTER,				0},
	{XTP_NODE_COLLIMATOR_CONTROL,	0},
	{XTP_NODE_GENERATOR,			0},
};


CXMaster::CXMaster() : CIDCThread("xMaster", IDC_TASK_XMASTER, Small, Moderate, true)
{
	xtpClient = new CXtpClient(XTP_NODE_MASTER);
}

CXMaster::~CXMaster()
{

}

void CXMaster::run()
{
	while(!xtpClient->XtpConnect(XMasterCallBackFunc, GetBroadList(xtpClient->GetNodeID()),NULL))
	{
		sleep(2);
	}

	while(1)
	{
		char		buf[MAX_ITC_LENGTH];
		const int	rc = m_msgQueue->recv(buf, MAX_ITC_LENGTH);

		if(rc > 0)
		{
			ITCObjHeader	*p = (ITCObjHeader *)buf;

			switch(p->ITCObjectID)
			{
				default:
					break;

			}
		}
	}
}

void XMasterCallBackFunc(tXtpObjHeader * msg, void * para)
{
	if(para != NULL)
	{
		switch(msg->telID)
		{
			case TUI_CMD:
			{
				TuiCmd *pTuiCmd = (TuiCmd *)(msg + 1);
				OnTuiCmd(pTuiCmd);
			}
			break;

			case GEN_REPORT:
			{
				GenReport	*genReport = (GenReport *)(msg + 1);
				OnGenReport(genReport);
			}
			break;

			default:
			break;
		}
	}
}


void OnTuiCmd(TuiCmd * tuiCmd)
{
	TransTuiCmdFrame		transTuiCmdFrame;

	transTuiCmdFrame.header.tidSrc			= IDC_TASK_XMASTER;
	transTuiCmdFrame.header.tidTarget		= IDC_TASK_XTUI;
	transTuiCmdFrame.header.mode			= 0;
	transTuiCmdFrame.header.flowControl		= 0;
	transTuiCmdFrame.header.len				= sizeof(TransTuiCmdFrame);
	transTuiCmdFrame.header.ITCObjectID		= TRANS_TUI_CMD;
	memcpy(&transTuiCmdFrame.tuiCmd, tuiCmd, sizeof(TuiCmd));

	CIDCThread::ITCSend(&transTuiCmdFrame.header);
}


void OnGenReport(GenReport * genReport)
{
	TransGenReportFrame		transGenReportFrame;

	transGenReportFrame.header.tidSrc		= IDC_TASK_XMASTER;
	transGenReportFrame.header.tidTarget	= IDC_TASK_XGENERATOR;
	transGenReportFrame.header.mode			= 0;
	transGenReportFrame.header.flowControl	= 0;
	transGenReportFrame.header.len			= sizeof(TransGenReportFrame);
	transGenReportFrame.header.ITCObjectID	= TRANS_GEN_REPORT;
	memcpy(&transGenReportFrame.genReport, genReport, sizeof(GenReport));

	CIDCThread::ITCSend(&transGenReportFrame.header);
}

