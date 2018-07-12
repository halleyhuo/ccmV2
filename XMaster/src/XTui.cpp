/**********************************************************************
 *
 * Copyright:
 *		Imaging dynamic Company
 *
 * FileName:
 *		XTui.cpp
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

#include "Types.h"
#include "XTui.h"
#include "IDCBase/inc/IDCThread.h"
#include "XTypes.h"

CXTui::CXTui()
{

}

CXTui::~CXTui()
{

}

void CXTui::run()
{
	while(1)
	{
		char		buf[MAX_ITC_LENGTH];
		const int	rc = m_msgQueue->recv(buf, MAX_ITC_LENGTH);

		if(rc > 0)
		{
			ITCObjHeader	*p = (ITCObjHeader *)buf;

			switch(p->ITCObjectID)
			{
				case TRANS_TUI_CMD_SETS:
				{
					TuiCmdSets	*tuiCmdSets = (TuiCmdSets *)(p + 1);
					uint8		i;

					for(i = 0; i < tuiCmdSets->counts; i++)
					{
						switch(tuiCmdSets->tuicmds[i].tuiCmdId)
						{
							case TUI_CMD_GEN_MODE:
								OnTuiCmdGenMode(tuiCmdSets->tuicmds[i].tuiCmdParams);
							break;

							case TUI_CMD_GEN_FPS:
								OnTuiCmdGenFps(tuiCmdSets->tuicmds[i].tuiCmdParams);
							break;

							case TUI_CMD_GEN_MAGNIFY:
								OnTuiCmdGenMagnify();
							break;

							case TUI_CMD_GEN_DOSE:
								OnTuiCmdGenDose();
							break;

							case TUI_CMD_GEN_ROAD:
								OnTuiCmdGenRoad();
							break;

							case TUI_CMD_GEN_SUB:
								OnTuiCmdGenSub();
							break;

							case TUI_CMD_GEN_FLUORO:
								OnTuiCmdGenFluoro();
							break;

							case TUI_CMD_GEN_SINGLE_IMAGE:
								OnTuiCmdGenSingleImage();
							break;

							case TUI_CMD_GEN_KV_PLUS:
								OnTuiCmdGenKvPlus();
							break;

							case TUI_CMD_GEN_KV_MINUS:
								OnTuiCmdGenKvMinus();
							break;

							case TUI_CMD_GEN_MA_PLUS:
								OnTuiCmdGenMaPlus();
							break;

							case TUI_CMD_GEN_MA_MINUS:
								OnTuiCmdGenMaMinus();
							break;

							case TUI_CMD_C_ARM_DOWN:
								OnTuiCmdCArmDown();
							break;

							case TUI_CMD_C_ARM_UP:
								OnTuiCmdCArmUp();
							break;

							case TUI_CMD_COL_UD_NARROW:
								OnTuiCmdColUdNarrow();
							break;

							case TUI_CMD_COL_UD_EXPAND:
								OnTuiCmdColUdExpand();
							break;

							case TUI_CMD_COL_LR_NARROW:
								OnTuiCmdColLrNarrow();
							break;

							case TUI_CMD_COL_LR_EXPAND:
								OnTuiCmdColLrExpand();
							break;

							default:
							break;

						}
					}
				}
				break;

				default:
				break;
			}
		}
	}
}



void CXTui::OnTuiCmdGenMode(TuiCmdParams cmdParams)
{
	XCmdGenFrame		xCmdGenFrame;
	XCmdGenMode			genMode;

	xCmdGenFrame.header.tidSrc			= IDC_TASK_XTUI;
	xCmdGenFrame.header.tidTarget		= IDC_TASK_XGENERATOR;
	xCmdGenFrame.header.mode			= 0;
	xCmdGenFrame.header.flowControl 	= 0;
	xCmdGenFrame.header.len 			= sizeof(XCmdGenFrame);
	xCmdGenFrame.header.ITCObjectID 	= X_CMD_GEN;

	xCmdGenFrame.xCmdGen.xCmdGenId		= X_CMD_GEN_MODE;
	switch(cmdParams)
	{
		case TUI_GEN_MODE_C:
			genMode = X_CMD_GEN_MODE_C;
			break;

		case TUI_GEN_MODE_P:
			genMode = X_CMD_GEN_MODE_P;
			break;

		case TUI_GEN_MODE_S:
			genMode = X_CMD_GEN_MODE_S;
			break;

		case TUI_GEN_MODE_N:
		default:
			genMode = X_CMD_GEN_MODE_N;
			break;

	}
	xCmdGenFrame.xCmdGen.xCmdGenParam = genMode;

	CIDCThread::ITCSend(&xCmdGenFrame.header);
}

void CXTui::OnTuiCmdGenFps(TuiCmdParams cmdParams)
{
	XCmdGenFrame		xCmdGenFrame;
	XCmdGenFps			genFps;

	xCmdGenFrame.header.tidSrc			= IDC_TASK_XTUI;
	xCmdGenFrame.header.tidTarget		= IDC_TASK_XGENERATOR;
	xCmdGenFrame.header.mode			= 0;
	xCmdGenFrame.header.flowControl 	= 0;
	xCmdGenFrame.header.len 			= sizeof(XCmdGenFrame);
	xCmdGenFrame.header.ITCObjectID 	= X_CMD_GEN;

	xCmdGenFrame.xCmdGen.xCmdGenId		= X_CMD_GEN_FPS;
	switch(cmdParams)
	{
		case TUI_GEN_FPS_5:
			genFps = X_CMD_GEN_FPS_5;
			break;

		case TUI_GEN_FPS_10:
			genFps = X_CMD_GEN_FPS_10;
			break;

		case TUI_GEN_FPS_15:
			genFps = X_CMD_GEN_FPS_15;
			break;

		case TUI_GEN_FPS_20:
			genFps = X_CMD_GEN_FPS_20;
			break;

		case TUI_GEN_FPS_30:
			genFps = X_CMD_GEN_FPS_30;
			break;

		case TUI_GEN_FPS_N:
		default:
			genFps = X_CMD_GEN_FPS_N;
			break;

	}
	xCmdGenFrame.xCmdGen.xCmdGenParam = genFps;

	CIDCThread::ITCSend(&xCmdGenFrame.header);
}

void CXTui::OnTuiCmdGenMagnify(void)
{
	XCmdGenFrame		xCmdGenFrame;

	xCmdGenFrame.header.tidSrc			= IDC_TASK_XTUI;
	xCmdGenFrame.header.tidTarget		= IDC_TASK_XGENERATOR;
	xCmdGenFrame.header.mode			= 0;
	xCmdGenFrame.header.flowControl 	= 0;
	xCmdGenFrame.header.len 			= sizeof(XCmdGenFrame);
	xCmdGenFrame.header.ITCObjectID 	= X_CMD_GEN;

	xCmdGenFrame.xCmdGen.xCmdGenId		= X_CMD_GEN_MAGNIFY;
	xCmdGenFrame.xCmdGen.xCmdGenParam	= 0;

	CIDCThread::ITCSend(&xCmdGenFrame.header);
}

void CXTui::OnTuiCmdGenDose(void)
{
	XCmdGenFrame		xCmdGenFrame;

	xCmdGenFrame.header.tidSrc			= IDC_TASK_XTUI;
	xCmdGenFrame.header.tidTarget		= IDC_TASK_XGENERATOR;
	xCmdGenFrame.header.mode			= 0;
	xCmdGenFrame.header.flowControl 	= 0;
	xCmdGenFrame.header.len 			= sizeof(XCmdGenFrame);
	xCmdGenFrame.header.ITCObjectID 	= X_CMD_GEN;

	xCmdGenFrame.xCmdGen.xCmdGenId		= X_CMD_GEN_DOSE;
	xCmdGenFrame.xCmdGen.xCmdGenParam	= 0;

	CIDCThread::ITCSend(&xCmdGenFrame.header);
}


void CXTui::OnTuiCmdGenRoad(void)
{
	XCmdGenFrame		xCmdGenFrame;

	xCmdGenFrame.header.tidSrc			= IDC_TASK_XTUI;
	xCmdGenFrame.header.tidTarget		= IDC_TASK_XGENERATOR;
	xCmdGenFrame.header.mode			= 0;
	xCmdGenFrame.header.flowControl 	= 0;
	xCmdGenFrame.header.len 			= sizeof(XCmdGenFrame);
	xCmdGenFrame.header.ITCObjectID 	= X_CMD_GEN;

	xCmdGenFrame.xCmdGen.xCmdGenId		= X_CMD_GEN_ROAD;
	xCmdGenFrame.xCmdGen.xCmdGenParam	= 0;

	CIDCThread::ITCSend(&xCmdGenFrame.header);
}


void CXTui::OnTuiCmdGenSub(void)
{
	XCmdGenFrame		xCmdGenFrame;

	xCmdGenFrame.header.tidSrc			= IDC_TASK_XTUI;
	xCmdGenFrame.header.tidTarget		= IDC_TASK_XGENERATOR;
	xCmdGenFrame.header.mode			= 0;
	xCmdGenFrame.header.flowControl 	= 0;
	xCmdGenFrame.header.len 			= sizeof(XCmdGenFrame);
	xCmdGenFrame.header.ITCObjectID 	= X_CMD_GEN;

	xCmdGenFrame.xCmdGen.xCmdGenId		= X_CMD_GEN_SUB;
	xCmdGenFrame.xCmdGen.xCmdGenParam	= 0;

	CIDCThread::ITCSend(&xCmdGenFrame.header);
}


void CXTui::OnTuiCmdGenFluoro(void)
{
	XCmdGenFrame		xCmdGenFrame;

	xCmdGenFrame.header.tidSrc			= IDC_TASK_XTUI;
	xCmdGenFrame.header.tidTarget		= IDC_TASK_XGENERATOR;
	xCmdGenFrame.header.mode			= 0;
	xCmdGenFrame.header.flowControl 	= 0;
	xCmdGenFrame.header.len 			= sizeof(XCmdGenFrame);
	xCmdGenFrame.header.ITCObjectID 	= X_CMD_GEN;

	xCmdGenFrame.xCmdGen.xCmdGenId		= X_CMD_GEN_FLUORO;
	xCmdGenFrame.xCmdGen.xCmdGenParam	= 0;

	CIDCThread::ITCSend(&xCmdGenFrame.header);
}


void CXTui::OnTuiCmdGenSingleImage(void)
{
	XCmdGenFrame		xCmdGenFrame;

	xCmdGenFrame.header.tidSrc			= IDC_TASK_XTUI;
	xCmdGenFrame.header.tidTarget		= IDC_TASK_XGENERATOR;
	xCmdGenFrame.header.mode			= 0;
	xCmdGenFrame.header.flowControl 	= 0;
	xCmdGenFrame.header.len 			= sizeof(XCmdGenFrame);
	xCmdGenFrame.header.ITCObjectID 	= X_CMD_GEN;

	xCmdGenFrame.xCmdGen.xCmdGenId		= X_CMD_GEN_SINGLE_IMAGE;
	xCmdGenFrame.xCmdGen.xCmdGenParam	= 0;

	CIDCThread::ITCSend(&xCmdGenFrame.header);
}


void CXTui::OnTuiCmdGenKvPlus(void)
{
	XCmdGenFrame		xCmdGenFrame;

	xCmdGenFrame.header.tidSrc			= IDC_TASK_XTUI;
	xCmdGenFrame.header.tidTarget		= IDC_TASK_XGENERATOR;
	xCmdGenFrame.header.mode			= 0;
	xCmdGenFrame.header.flowControl 	= 0;
	xCmdGenFrame.header.len 			= sizeof(XCmdGenFrame);
	xCmdGenFrame.header.ITCObjectID 	= X_CMD_GEN;

	xCmdGenFrame.xCmdGen.xCmdGenId		= X_CMD_GEN_KV_PLUS;
	xCmdGenFrame.xCmdGen.xCmdGenParam	= 0;

	CIDCThread::ITCSend(&xCmdGenFrame.header);
}


void CXTui::OnTuiCmdGenKvMinus(void)
{
	XCmdGenFrame		xCmdGenFrame;

	xCmdGenFrame.header.tidSrc			= IDC_TASK_XTUI;
	xCmdGenFrame.header.tidTarget		= IDC_TASK_XGENERATOR;
	xCmdGenFrame.header.mode			= 0;
	xCmdGenFrame.header.flowControl 	= 0;
	xCmdGenFrame.header.len 			= sizeof(XCmdGenFrame);
	xCmdGenFrame.header.ITCObjectID 	= X_CMD_GEN;

	xCmdGenFrame.xCmdGen.xCmdGenId		= X_CMD_GEN_KV_MINUS;
	xCmdGenFrame.xCmdGen.xCmdGenParam	= 0;

	CIDCThread::ITCSend(&xCmdGenFrame.header);
}


void CXTui::OnTuiCmdGenMaPlus(void)
{
	XCmdGenFrame		xCmdGenFrame;

	xCmdGenFrame.header.tidSrc			= IDC_TASK_XTUI;
	xCmdGenFrame.header.tidTarget		= IDC_TASK_XGENERATOR;
	xCmdGenFrame.header.mode			= 0;
	xCmdGenFrame.header.flowControl 	= 0;
	xCmdGenFrame.header.len 			= sizeof(XCmdGenFrame);
	xCmdGenFrame.header.ITCObjectID 	= X_CMD_GEN;

	xCmdGenFrame.xCmdGen.xCmdGenId		= X_CMD_GEN_MA_PLUS;
	xCmdGenFrame.xCmdGen.xCmdGenParam	= 0;

	CIDCThread::ITCSend(&xCmdGenFrame.header);
}


void CXTui::OnTuiCmdGenMaMinus(void)
{
	XCmdGenFrame		xCmdGenFrame;

	xCmdGenFrame.header.tidSrc			= IDC_TASK_XTUI;
	xCmdGenFrame.header.tidTarget		= IDC_TASK_XGENERATOR;
	xCmdGenFrame.header.mode			= 0;
	xCmdGenFrame.header.flowControl 	= 0;
	xCmdGenFrame.header.len 			= sizeof(XCmdGenFrame);
	xCmdGenFrame.header.ITCObjectID 	= X_CMD_GEN;

	xCmdGenFrame.xCmdGen.xCmdGenId		= X_CMD_GEN_MA_MINUS;
	xCmdGenFrame.xCmdGen.xCmdGenParam	= 0;

	CIDCThread::ITCSend(&xCmdGenFrame.header);
}


void CXTui::OnTuiCmdCArmDown(void)
{

}


void CXTui::OnTuiCmdCArmUp(void)
{

}


void CXTui::OnTuiCmdColUdNarrow(void)
{

}


void CXTui::OnTuiCmdColUdExpand(void)
{

}


void CXTui::OnTuiCmdColLrNarrow(void)
{

}


void CXTui::OnTuiCmdColLrExpand(void)
{

}


