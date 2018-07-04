/**********************************************************************
 *
 * Copyright:
 *		Imaging dynamic Company
 *
 * FileName:
 *		XTui.h
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

#ifndef __XTUI_H__
#define __XTUI_H__

#include "Types.h"
#include "IDCBase/inc/IDCThread.h"


class CXTui : public CIDCThread
{
	public:
		CXTui();

		virtual ~CXTui();

		virtual void run();

	private:

};


#endif

