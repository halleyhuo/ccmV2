/**********************************************************************
 *
 * Copyright:
 *		Imaging dynamic Company
 *
 * FileName:
 *		GenVdProc.h
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

#ifndef __GEN_VD_PROC_H__
#define __GEN_VD_PROC_H__

#include "Types.h"
#include "IDCBase/inc/IDCThread.h"
#include "IDCXTP/Client/inc/xtpClient.h"
#include "GenComm.h"

class CGenComm;

class CGenVdProc : public CIDCThread
{

public:
	CGenVdProc();
	virtual ~CGenVdProc();
	virtual void run();


private:
	CGenComm		*m_genComm;
	CXtpClient		*m_xtpClient;
};

#endif /*__GEN_VD_PROC_H__*/

