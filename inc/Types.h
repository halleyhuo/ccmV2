/**********************************************************************
 *
 * Copyright:
 *		Imaging dynamic Company
 *
 * FileName:
 *		Types.h
 *
 * Description:
 *		Define the basic type
 *
 * Author:
 *		Halley
 *
 * Create Date:
 *		2018/3/23
 *
 */


#ifndef __TYPES_H__
#define __TYPES_H__

typedef unsigned char			uint8_t;
typedef signed char				int8_t;
typedef unsigned short			uint16_t;
typedef signed short			int16_t;
typedef unsigned int			uint32_t;
typedef signed int				int32_t;
typedef unsigned long long		uint64_t;
typedef signed long long		int64_t;


typedef uint8_t					uint8;
typedef uint16_t				uint16;
typedef uint32_t				uint32;
typedef uint64_t				uint64;
typedef int8_t					int8;
typedef int16_t					int16;
typedef int32_t					int32;
typedef int64_t					int64;


#ifndef BOOL
#define BOOL	uint8
#define TRUE	(1 == 1)
#define FALSE	(1 == 0)
#endif


typedef uint8					ENUM8;
typedef uint16					ENUM16;

/**
 * Indicate the paramters property
 */
#define PARAMS_IN				/* Paramters for input */
#define PARAMS_OUT				/* Paramters for output */
#define PARAMS_INOUT			/* Paramters for input and output*/




////////////////////////////////////////////////////////////////////////////////////

#define XTP_DEF_PACKED __attribute__ ((packed))

#ifndef STACK_SIZE_DEFAULT
#define STACK_SIZE_DEFAULT		0x50000u //320KB
#endif

#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN		0x10000//64k as application
#endif

inline void IDC_DELAY(uint32 uiDelay) 
{
	for(uint64 i = uiDelay * 10000; i > 0; i--);
}


typedef int32					IDCErrnoType;
#define NO_TIMEOUT				0

#define IDC_OK					0
#define IDC_ERROR				-1
#define IDC_TIMEOUT				-2
#define IDC_NO_SUFFICIENT_BUF	-3

#define IDC_MAX_NAME_LENGTH		32

#ifndef handle_error_en
#define handle_error_en(en, msg)	\
	do								\
	{								\
		errno = en;					\
		perror(msg);				\
		exit(EXIT_FAILURE);			\
	}while (0)
#endif

#define MAX_ITC_LENGTH		1024 /* Length of Inter Thread Communication Object <= 1024, which contain header length */


#define getprio(X) getpriority( PRIO_PROCESS, X)
#define setprio(X, Y) setpriority( PRIO_PROCESS, X, Y)


#endif /*__TYPES_H__*/
