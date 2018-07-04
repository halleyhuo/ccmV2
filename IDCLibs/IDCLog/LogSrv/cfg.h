#ifndef CFG_H__
#define CFG_H__

#include "Types.h"

const uint8 MAX_FILTER_CNT = 32;
const uint8 MAX_FILTER_NAME_SIZE = 32;
typedef enum
{
	LogCfgType_Unknown,/*Can't be recognized*/
	LogCfgType_Comment,/*Start with ';'*/
	LogCfgType_PassFilter,/*Start with 'PF:'*/
} LogCfg;

typedef struct
{
	uint8 originPassCnt;
	char originPassFilter[MAX_FILTER_CNT][MAX_FILTER_NAME_SIZE];
}LogCfgStruct;

extern void LoadCfg();

extern bool PassFilter(char originName[]);
#endif /*CFG_H__*/