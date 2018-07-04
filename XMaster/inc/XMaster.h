/**********************************************************************
*
* Copyright:
*	   Imaging dynamic Company
*
* FileName:
*	   XMaster.h
*
* Description:
*	   Class XMaster
*
* Author:
*	   Halley
*
* Create Date:
*	   2018/6/3
*
*/



#ifndef __X_MASTER_H__
#define __X_MASTER_H__


#include "Types.h"
#include "IDCBase/inc/IDCThread.h"
#include "IDCXTP/inc/xtpobj.h"
#include "TuiTypes.h"
#include "GenTypes.h"

class CXtpClient;

class CXMaster : public CIDCThread
{
	public:
		CXMaster();

		virtual ~CXMaster();

		virtual void run();

	private:

		CXtpClient		*xtpClient;
};
#endif // __X_MASTER_H__

