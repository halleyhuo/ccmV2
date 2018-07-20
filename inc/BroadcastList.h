/**********************************************************************
 *
 * Copyright:
 *		Imaging dynamic Company
 *
 * FileName:
 *		BroadcastList.h
 *
 * Description:
 *		This file can only be included by primary XTP Node cpp files.
 *		such as XMaster.cpp, GenVdProc.cpp, ColVdProc.cpp...
 *
 * Author:
 *		Halley
 *
 * Create Date:
 *		2018/6/3
 *
 */

#define XTP_NODE_MASTER					0x10
#define XTP_NODE_COLLIMATOR_CONTROL		0x20
#define XTP_NODE_GENERATOR				0x30
#define XTP_NODE_TUI					0x70


BRODCAST_LIST broadcastList[] = 
{    
	{XTP_NODE_MASTER,				0},
	{XTP_NODE_COLLIMATOR_CONTROL,	0},
	{XTP_NODE_GENERATOR,			0},
};


