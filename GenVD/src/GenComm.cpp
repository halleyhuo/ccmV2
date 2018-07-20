/**********************************************************************
 *
 * Copyright:
 *		Imaging dynamic Company
 *
 * FileName:
 *		GenComm.cpp
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


#include "GenComm.h"
#include "GenVdTypes.h"

#include <string.h>
#include <sys/select.h>
#include <unistd.h>

static int				g_fd;
static pthread_mutex_t	g_mutexComm;
static pthread_t		g_pThreadRecv;

CGenComm::CGenComm(GenCommCallback genCommCallback)
{
	m_genCommCallback = genCommCallback;
	memset(&g_pThreadRecv, 0, sizeof(pthread_t));
	pthread_mutex_init(&g_mutexComm,NULL);
	g_fd = 0;
}

CGenComm::~CGenComm()
{

}

BOOL CGenComm::Start(void)
{
	BOOL		ret = TRUE;


	g_fd = OpenDev("/dev/ttymxc2");

	if(g_fd < 0)
	{
		return FALSE;
	}

	pthread_mutex_init(&g_mutexComm, NULL);

	if(pthread_create(&g_pThreadRecv, NULL, CGenComm::GenCommRecv, this) != 0)
	{
		return FALSE;
	}

	return ret;
}

int32 CGenComm::OpenDev(const char *dev)
{
	return 0;
}

void * CGenComm::GenCommRecv(void * args)
{
	fd_set			fdSet;
	int32			retVal;
	uint8			recvData[14];
	RecvState		recvState;
	GenRecvDataSt	genRecvData;


	while(1)
	{
		pthread_mutex_lock(&g_mutexComm);
		FD_ZERO(&fdSet);
		FD_SET(g_fd, &fdSet);

		retVal = select(g_fd + 1, &fdSet, NULL, NULL, (struct timeval *)0 );

		switch(retVal)
		{
			case -1:
				goto END;

			case 0:
				break;

			default:
				if(read(g_fd, recvData, 1) > 0)
				{
					if(recvData[0] == '>' && recvState == RecvStateStart)
					{
						recvState == RecvStateRecving;
						genRecvData.dataLen = 0;
					}

					if(recvState == RecvStateRecving)
					{
						if(recvData[0] == 0x0A)
						{
							recvState == RecvStateEnd;
						}
						else
						{
							genRecvData.data[genRecvData.dataLen] = recvData[0];
							genRecvData.dataLen++;
						}
					}

					if(recvState == RecvStateEnd)
					{
						((CGenComm*)args)->m_genCommCallback((char*)genRecvData.data, genRecvData.dataLen);
						recvState = RecvStateStart;
					}
				}
				break;
		}
END:
		pthread_mutex_unlock(&g_mutexComm);
	}
}

int32 CGenComm::GenCommSend(char * str, uint32 len)
{
	pthread_mutex_lock(&g_mutexComm);
	write(g_fd, str, len);
	pthread_mutex_unlock(&g_mutexComm);
}

