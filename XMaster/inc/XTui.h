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
#include "TuiTypes.h"


class CXTui : public CIDCThread
{
	public:
		CXTui();

		virtual ~CXTui();

		virtual void run();

	private:

		void OnTuiCmdGenMode(TuiCmdParams cmdParams);

		void OnTuiCmdGenFps(TuiCmdParams cmdParams);

		void OnTuiCmdGenMagnify(void);

		void OnTuiCmdGenDose(void);

		void OnTuiCmdGenRoad(void);

		void OnTuiCmdGenSub(void);

		void OnTuiCmdGenFluoro(void);

		void OnTuiCmdGenSingleImage(void);

		void OnTuiCmdGenKvPlus(void);

		void OnTuiCmdGenKvMinus(void);

		void OnTuiCmdGenMaPlus(void);

		void OnTuiCmdGenMaMinus(void);

		void OnTuiCmdCArmDown(void);

		void OnTuiCmdCArmUp(void);

		void OnTuiCmdColUdNarrow(void);

		void OnTuiCmdColUdExpand(void);

		void OnTuiCmdColLrNarrow(void);

		void OnTuiCmdColLrExpand(void);

};


#endif

