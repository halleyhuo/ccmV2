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
#include "IDCBase/inc/IDCThread.h"
#include "XTypes.h"



static void XMasterCallBackFunc(tXtpObjHeader * msg, void * para);

static void OnTuiCmdSets(TuiCmdSets * tuiCmdSets);

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
				case TRANS_TUI_REPORT_SETS:
					break;

				case TRANS_CMD_GEN:
				{
					GenCmd		*pGenCmd = (GenCmd *)(p + 1);

					OnTransCmdGen(pGenCmd);
				}
				break;

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
			case TUI_CMD_SETS:
			{
				TuiCmdSets	*pTuiCmdSets = (TuiCmdSets *)(msg + 1);
				OnTuiCmdSets(pTuiCmdSets);
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


void OnTuiCmdSets(TuiCmdSets * tuiCmdSets)
{
	TransTuiCmdFrame		transTuiCmdFrame;

	transTuiCmdFrame.header.tidSrc			= IDC_TASK_XMASTER;
	transTuiCmdFrame.header.tidTarget		= IDC_TASK_XTUI;
	transTuiCmdFrame.header.mode			= 0;
	transTuiCmdFrame.header.flowControl		= 0;
	transTuiCmdFrame.header.len				= sizeof(TransTuiCmdFrame);
	transTuiCmdFrame.header.ITCObjectID		= TRANS_TUI_CMD_SETS;
	memcpy(&transTuiCmdFrame.tuiCmdSets, tuiCmdSets, sizeof(TuiCmdSets));

	CIDCThread::ITCSend(&transTuiCmdFrame.header);
}


void OnGenReport(GenReport * genReport)
{
	TransReportGenFrame		transReportGenFrame;

	transReportGenFrame.header.tidSrc		= IDC_TASK_XMASTER;
	transReportGenFrame.header.tidTarget	= IDC_TASK_XGENERATOR;
	transReportGenFrame.header.mode			= 0;
	transReportGenFrame.header.flowControl	= 0;
	transReportGenFrame.header.len			= sizeof(TransReportGenFrame);
	transReportGenFrame.header.ITCObjectID	= TRANS_REPORT_GEN;
	memcpy(&transReportGenFrame.genReport, genReport, sizeof(GenReport));

	CIDCThread::ITCSend(&transReportGenFrame.header);
}

void CXMaster::OnTransCmdGen(GenCmd *pGenCmd)
{
	GenCmdFrame			genCmdFrame;

	genCmdFrame.header.sender	= xtpClient->GetNodeID();
	genCmdFrame.header.target	= XTP_NODE_GENERATOR;
	genCmdFrame.header.telID	= GEN_CMD;
	genCmdFrame.header.mode		= XTP_MODE_DIRECT;
	genCmdFrame.header.length	= sizeof(GenCmdFrame);
	memcpy(&genCmdFrame.genCmd, pGenCmd, sizeof(GenCmd));

	xtpClient->XtpSend(&genCmdFrame.header);
}

