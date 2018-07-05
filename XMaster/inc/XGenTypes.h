/**********************************************************************
 *
 * Copyright:
 *		Imaging dynamic Company
 *
 * FileName:
 *		XGenTypes.h
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

#ifndef __X_GEN_TYPES_H__
#define __X_GEN_TYPES_H__

#include "Types.h"


typedef uint16			XGenCmdId;


/* param XGenFluoroMode is valid */
#define	XGEN_CMD_FLUORO_MODE			100

/* param XGenFluoroVAS is valid*/
#define XGEN_CMD_FLUORO_VAS				200

/* param XGenKvValue is valid*/
#define XGEN_CMD_FLUORO_KV				211
/* param XGenMsValue is valid*/
#define XGEN_CMD_FLUORO_MS				212
/* param XGenMaValue is valid*/
#define XGEN_CMD_FLUORO_MA				213
/* param XGenMasValue is valid*/
#define XGEN_CMD_FLUORO_MAS				214

/* param XGenFluoroFps is valid*/
#define XGEN_CMD_FLUORO_FPS				300

/* param XGenFluoroDose is valid*/
#define XGEN_CMD_FLUORO_DOSE			400



typedef uint8	XGenCmdFluoroMode;

#define XGEN_CMD_FLUORO_MODE_N			0
#define XGEN_CMD_FLUORO_MODE_C			1
#define XGEN_CMD_FLUORO_MODE_P			2
#define XGEN_CMD_FLUORO_MODE_S			3



typedef uint8	XGenCmdFluoroVAS;

#define XGEN_CMD_FLUORO_VAS_PLUS		1
#define XGEN_CMD_FLUORO_VAS_MINUS		2


typedef uint8	XGenCmdFluoroFps;

#define XGEN_CMD_FLUORO_FPS_N			0
#define XGEN_CMD_FLUORO_FPS_5			1
#define XGEN_CMD_FLUORO_FPS_10			2
#define XGEN_CMD_FLUORO_FPS_15			3
#define XGEN_CMD_FLUORO_FPS_20			4
#define XGEN_CMD_FLUORO_FPS_30			5



typedef uint8	XGenCmdFluoroDose;

#define XGEN_CMD_FLUORO_DOSE_N			0
#define XGEN_CMD_FLUORO_DOSE_1			1
#define XGEN_CMD_FLUORO_DOSE_2			2
#define XGEN_CMD_FLUORO_DOSE_3			3


typedef uint16		XGenReportId;

/*
 * confirm received X-ray parameters for single, continuous and pulsed mode
 * parameter PresetParam is valid
 */
#define XGEN_REPORT_CFM_PRESET_PARAM	100


typedef struct _XGenCmd
{
	XGenCmdId		xGenCmdId;

	union
	{
		XGenCmdFluoroMode		xGenCmdFluoroMode;
		XGenCmdFluoroVAS		xGenCmdFluoroVAS;
		KvValueType				xGenKvValue;
		MaValueType				xGenMaValue;
		MsValueType				xGenMsValue;
		MasValueType			xGenMasValue;
		XGenCmdFluoroFps		xGenCmdFluoroFps;
		XGenCmdFluoroDose		xGenCmdFluoroDose;
	} param;

} XTP_DEF_PACKED XGenCmd;


typedef struct _XGenReport
{
	XGenReportId		xgenReportId;

	union
	{
		PresetParam 			preParam;
	} param;

} XTP_DEF_PACKED XGenReport;


#endif

