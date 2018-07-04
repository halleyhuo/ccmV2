/**********************************************************************
 *
 * Copyright:
 *		Imaging dynamic Company
 *
 * FileName:
 *		GenTypes.h
 *
 * Description:
 *		Define the generators type
 *
 * Author:
 *		Halley
 *
 * Create Date:
 *		2018/6/3
 *
 */

#ifndef __GEN_TYPES_H__
#define __GEN_TYPES_H__

#include "Types.h"



typedef uint8					FluoroMode;
#define FLUORO_MODE_CONTINUOUS	1
#define FLUORO_MODE_PULSED		2
#define FLUORO_MODE_SINGLE		3


typedef uint32		KvValueType;
typedef uint32		MaValueType;
typedef uint32		MsValueType;
typedef uint32		MasValueType;

typedef uint8		FocusType;
#define FOCUS_TYPE_SMALL		1
#define FOCUS_TYPE_LARGE		2

typedef uint8		FpsValueType;
#define FPS_5					5
#define FPS_10					10
#define FPS_15					15
#define FPS_30					30

typedef uint8		EndReasonType;
#define ER_MAS_REACHED			1
#define ER_ANY_ERROR			2
#define ER_HV_TRIGGER			3
#define ER_OTHER				4
#define ER_SAFETY_SIGNAL		5
#define ER_TIME					6


typedef struct _ActualExposureData
{
	KvValueType			kv;
	MaValueType			ma;
	MsValueType			ms;
	MasValueType		mas;
	EndReasonType		endReason;
	uint32				numPulses;
} ActualExposureData;

typedef struct _CassetteExpParam
{
	KvValueType			kv;
	MaValueType			ma;
	MsValueType			ms;
	MasValueType		mas;
	FocusType			focus;
} CassetteExpParam;

typedef struct _ExposuringParam
{
	KvValueType			kv;
	MaValueType			ma;
	MsValueType			ms;
	MasValueType		mas;
} ExposuringParam;

typedef struct _PresetParam
{
	FluoroMode			fluoroMode;
	KvValueType			kv;
	MaValueType			ma;
	MsValueType			ms;
	MasValueType		mas;
	FocusType			focus;
	FpsValueType		fps;
} PresetParam;


typedef uint16			GenCmdId;

/*
 * set the parameters for cassette exposure
 * parameter CassetteExpParam is valid
 */
#define GEN_CMD_SET_PARAM_CE			100

/*
 * change the parameters during exposure for continuous and pulsed mode
 * parameter ExposuringParam is valid
 */
#define GEN_CMD_CHG_PARAM_EXP			200

/*
 * set the parameters for single, continuous and pulsed mode
 * parameter PresetParam is valid
 */
#define GEN_CMD_SET_PARAM_PRE			300



typedef uint16			GenReportId;

/*
 * confirm received X-ray parameters for cassette exposure
 * parameter CassetteExpParam is valid
 */
#define GEN_REPORT_CFM_CE				100

/*
 * confirm received parameters during exposure
 * parameter ExposuringParam is valid
 */
#define GEN_REPORT_CFM_EXP				200

/*
 * confirm received X-ray parameters for single, continuous and pulsed mode
 * parameter PresetParam is valid
 */
#define GEN_REPORT_CFM_PRE				300

/*

 * generator unit reports the actual exposure data
 * parameter ActualExposureData is valid
 */
#define GEN_REPORT_ACTUAL_DATA			400


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


typedef struct _GenCmd
{
	GenCmdId		genCmdId;

	union
	{
		CassetteExpParam		ceParam;
		ExposuringParam			expParam;
		PresetParam				preParam;
	} param;

} XTP_DEF_PACKED GenCmd;

typedef struct _GenReport
{
	GenReportId		genReportId;

	union
	{
		CassetteExpParam		ceParam;
		ExposuringParam			expParam;
		PresetParam				preParam;
		ActualExposureData		actualExpData;
	} param;

} XTP_DEF_PACKED GenReport;


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


#endif // __GEN_TYPES_H__

