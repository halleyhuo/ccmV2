/**********************************************************************
 *
 * Copyright:
 *		Imaging dynamic Company
 *
 * FileName:
 *		XGenerator.h
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


#ifndef __X_GENERATOR_H__
#define __X_GENERATOR_H__

#include "XGenerator.h"
#include "Types.h"
#include "GenTypes.h"
#include "IDCBase/inc/IDCThread.h"
#include "XMasterTypes.h"


typedef struct _GeneratorStatus
{
	PresetParam			genPresets;
	GeneratorState		genState;
} GeneratorStatus;


class CXGenerator : public CIDCThread
{
	public:
		CXGenerator();

		virtual ~CXGenerator();

		virtual void run();

	private:
		void OnXCmdGen(XCmdGen * pXCmdGen);

		void OnXCmdGenMode(XCmdGenParam xCmdGenParam);


	private:
		GeneratorStatus		genStatus;
};
#endif //__X_GENERATOR_H__

