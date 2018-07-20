/**********************************************************************
 *
 * Copyright:
 *		Imaging dynamic Company
 *
 * FileName:
 *		XGenerator.c
 *
 * Description:
 *		Class XGenerator
 *
 * Author:
 *		Halley
 *
 * Create Date:
 *		2018/6/3
 *
 */

#include "XGenerator.h"
#include "Types.h"
#include "XTypes.h"
#include "IDCBase/inc/IDCThread.h"

CXGenerator::CXGenerator() : CIDCThread("xGenerator", IDC_TASK_XGENERATOR, Small, Moderate, true)
{
	genStatus.genPresets.mode		= FLUORO_MODE_CONTINUOUS;
	genStatus.genPresets.kv			= 0;
	genStatus.genPresets.ma			= 0;
	genStatus.genPresets.mas		= 0;
	genStatus.genPresets.focus		= FOCUS_TYPE_SMALL;
	genStatus.genPresets.fps		= GEN_FPS[0];
}

CXGenerator::~CXGenerator()
{

}


void CXGenerator::run()
{
	while(1)
	{
		char			buf[MAX_ITC_LENGTH];
		const int32		rc = m_msgQueue->recv(buf, MAX_ITC_LENGTH);

		if(rc > 0)
		{
			ITCObjHeader	*p = (ITCObjHeader *)buf;

			switch(p->ITCObjectID)
			{
				case X_CMD_GEN:
				{
					XCmdGen		*pXCmdGen = (XCmdGen *)(p + 1);
					OnXCmdGen(pXCmdGen);
				}
				break;

				case TRANS_REPORT_GEN:
					break;

				default:
					break;

			}
		}
	}
}


void CXGenerator::OnXCmdGen(XCmdGen * pXCmdGen)
{
	switch (pXCmdGen->xCmdGenId)
	{
		case X_CMD_GEN_MODE:
			OnXCmdGenMode(pXCmdGen->xCmdGenParam);
			break;

		case X_CMD_GEN_FPS:
			OnXCmdGenFps(pXCmdGen->xCmdGenParam);
			break;

		case X_CMD_GEN_MAGNIFY:
			break;
		
		case X_CMD_GEN_DOSE:
			break;

		case X_CMD_GEN_ROAD:
			break;
		
		case X_CMD_GEN_SUB:
			break;
		
		case X_CMD_GEN_FLUORO:
			break;
		
		case X_CMD_GEN_SINGLE_IMAGE:
			break;

		case X_CMD_GEN_KV_PLUS:
			break;
		
		case X_CMD_GEN_KV_MINUS:
			break;
		
		case X_CMD_GEN_MA_PLUS:
			break;
		
		case X_CMD_GEN_MA_MINUS:
			break;
		
		default:
			break;

	}
}

void CXGenerator::OnXCmdGenMode(XCmdGenParam xCmdGenParam)
{
	XCmdGenMode			xGenMode = (XCmdGenMode) xCmdGenParam;
	TransCmdGenFrame	transCmdGenFrame;

	switch(xGenMode)
	{
		case X_CMD_GEN_MODE_C:
			genStatus.genPresets.mode = FLUORO_MODE_CONTINUOUS;
		break;

		case X_CMD_GEN_MODE_P:
			genStatus.genPresets.mode = FLUORO_MODE_PULSED;
		break;

		case X_CMD_GEN_MODE_S:
			genStatus.genPresets.mode = FLUORO_MODE_SINGLE;
		break;

		case X_CMD_GEN_MODE_N:
		default:
		{
			FluoroMode		mode;

			mode = genStatus.genPresets.mode;
			mode++;
			if(mode > 3)
				mode = 1;
			genStatus.genPresets.mode = mode;
		}
		break;
	}

	transCmdGenFrame.header.tidSrc			= IDC_TASK_XGENERATOR;
	transCmdGenFrame.header.tidTarget		= IDC_TASK_XMASTER;
	transCmdGenFrame.header.mode			= 0;
	transCmdGenFrame.header.flowControl		= 0;
	transCmdGenFrame.header.len				= sizeof(TransCmdGenFrame);
	transCmdGenFrame.header.ITCObjectID		= TRANS_CMD_GEN;

	transCmdGenFrame.genCmd.genCmdId		= GEN_CMD_SET_PARAM_PRE;
	memcpy(&transCmdGenFrame.genCmd.param.preParam, &genStatus.genPresets, sizeof(PresetParam));

	CIDCThread::ITCSend(&transCmdGenFrame.header);
}


void CXGenerator::OnXCmdGenFps(XCmdGenParam xCmdGenParam)
{
	XCmdGenFps			xGenFps = (XCmdGenFps) xCmdGenParam;
	TransCmdGenFrame	transCmdGenFrame;

	switch(xGenFps)
	{
		case X_CMD_GEN_FPS_N:
		default:
		{
			uint8			fpsIndex;

			GEN_GET_FPS_INDEX(genStatus.genPresets.fps, fpsIndex);

			fpsIndex++;
			if(fpsIndex >= GEN_FPS_COUNT)
				fpsIndex = 0;

			GEN_GET_FPS(fpsIndex, genStatus.genPresets.fps);
		}
		break;
	}

	transCmdGenFrame.header.tidSrc			= IDC_TASK_XGENERATOR;
	transCmdGenFrame.header.tidTarget		= IDC_TASK_XMASTER;
	transCmdGenFrame.header.mode			= 0;
	transCmdGenFrame.header.flowControl		= 0;
	transCmdGenFrame.header.len				= sizeof(TransCmdGenFrame);
	transCmdGenFrame.header.ITCObjectID		= TRANS_CMD_GEN;

	transCmdGenFrame.genCmd.genCmdId		= GEN_CMD_SET_PARAM_PRE;
	memcpy(&transCmdGenFrame.genCmd.param.preParam, &genStatus.genPresets, sizeof(PresetParam));

	CIDCThread::ITCSend(&transCmdGenFrame.header);
}



