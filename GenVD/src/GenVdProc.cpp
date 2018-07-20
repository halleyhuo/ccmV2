/**********************************************************************
 *
 * Copyright:
 *		Imaging dynamic Company
 *
 * FileName:
 *		GenVdProc.cpp
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


#include "GenVdProc.h"
#include "GenComm.h"
#include "GenVdTypes.h"
#include "IDCBase/inc/IDCThread.h"
#include "IDCXTP/Client/inc/xtpClient.h"
#include "XtpNode.h"
#include "BroadcastList.h"


static void GenVdCallBackFunc(tXtpObjHeader * msg, void * para);
static int32 GenCommRecv(char * str, uint32 len);

CGenVdProc::CGenVdProc() : CIDCThread("GenVdProc", IDC_TASK_GENVDPROC, Small, Moderate, true)
{
	m_xtpClient = new CXtpClient(XTP_NODE_GENERATOR);
	m_genComm = new CGenComm(GenCommRecv);
}

CGenVdProc::~CGenVdProc()
{

}

void CGenVdProc::run()
{
	while(!m_xtpClient->XtpConnect(GenVdCallBackFunc, GetBroadList(m_xtpClient->GetNodeID()),NULL))
	{
		sleep(2);
	}
}

static int32 GenCommRecv(char * str, uint32 len)
{
	
}

static void GenVdCallBackFunc(tXtpObjHeader * msg, void * para)
{
	if(para != NULL)
	{
		switch(msg->telID)
		{
			case GEN_CMD:
			break;

			default:
			break;
		}
	}
}


