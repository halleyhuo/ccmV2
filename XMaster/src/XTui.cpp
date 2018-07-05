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
			ITCObjHeader	* p = (ITCObjHeader *)buf;

			switch(p->ITCObjectID)
			{
				case TRANS_TUI_CMD:
				{
					TuiCmd	*tuiCmd = (TuiCmd *)(p + 1);
					switch(tuiCmd->tuiCmdId)
					{
						default:
						break;
					}
				}
				break;

				default:
				break;
			}
		}
	}
}


