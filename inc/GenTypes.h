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

typedef uint8		EndReasonType;
#define ER_MAS_REACHED			1
#define ER_ANY_ERROR			2
#define ER_HV_TRIGGER			3
#define ER_OTHER				4
#define ER_SAFETY_SIGNAL		5
#define ER_TIME					6


typedef uint8		GeneratorState;
#define GEN_STATE_STANDY		1
#define GEN_STATE_INIT			2
#define GEN_STATE_START			3
#define GEN_STATE_ERROR			4
#define GEN_STATE_EXPOSURE		5

const uint32 GEN_FPS[] = {5, 10, 20, 50, 100, 150, 300};

#define GEN_FPS_COUNT		(sizeof(GEN_FPS) / sizeof(uint32))

#define GEN_GET_FPS_INDEX(fps, index)				\
do													\
{													\
	BOOL		find = FALSE;						\
	for(index = 0; index < GEN_FPS_COUNT; index++)	\
	{												\
		if(GEN_FPS[index] == fps)					\
		{											\
			find = TRUE;							\
			break;									\
		}											\
	}												\
	if(!find)										\
	{												\
		index = 0xFF;								\
	}												\
}													\
while (0)

#define GEN_GET_FPS(index, fps)						\
do													\
{													\
	fps = GEN_FPS[index];							\
}													\
while (0)

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
	FluoroMode			mode;
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


#endif // __GEN_TYPES_H__

