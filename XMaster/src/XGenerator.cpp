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
#include "IDCXTaskID.h"


CXGenerator::CXGenerator() : CIDCThread("xGenerator", IDC_TASK_XGENERATOR, Small, Moderate, true)
{

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
				case TRANS_GEN_CMD:
					break;

				case TRANS_GEN_REPORT:
					break;

				default:
					break;

			}
		}
	}
}




