/*
 * xtpcomm.h
 *
 *  Created on: 2017.04.04
 */

#ifndef XTP_COMM_H
#define XTP_COMM_H

#include <sys/time.h> 
#include <sys/resource.h>
#include <errno.h>
#include <mqueue.h> 
#include <unistd.h>

#include "Types.h"
#include "xtpobj.h"

#define DEFAULT_PORT_SUPPORT 0
#define MAX_XTP_DATA_LENGTH 1024


#define xtpServerPath "/xtpServerMsgQ"

#define msgq_xtp_srv2cltpre "/xtp_reveive"

class CIDCMsgQueue;

typedef enum 
{  
  XtpSysInit   =1,
  XtpSysExit   =2,
  XtpSysAddId  =3,
  XtpSysDelId  =4,
  XtpSysSend   =5,
  XtpSysReceive=6,
  XtpSysOverrun=7,
  XtpSysError  =8,
  XtpSysKill   =9,
  XtpSysStat   =10,
  XtpSysPulse  =249,
  KILL_THREAD  =250,
  XtpSys_NOCMD    =255, // not used, only a dummy
}XtpSysCommand;

/* Xtp state in answer */
enum XtpState
{
  XtpNotOk = 0,
  XtpOk = 1
};


//used in communication between Server and Client.
typedef struct
{
	int32 pid;               //meaningful client->Server, not care Server->Client. chid is similar
	int32 chid;
	uint8 bCommand;//Always meaningful.
    uint8 bState;  //meaningful not care Server->Client, client->Server
	tXtpObjHeader msgHeader;
    uint8         data[MAX_XTP_DATA_LENGTH];
}XTP_DEF_PACKED tXtpMessage;

//adapt to the above struct
const uint16 COMM_LENGTH_SERVER_CLIENT = 10;

typedef struct
{
    //Server variables
    mqd_t           mq;
    char            mqName[30];

    //pid_t           m_pulseId;
    //struct sigevent m_event;

    int32             ErrId;
    char*           pBuf;
    int32             BufSize;
    int32             Count;
    //_msg_info       info;
    int32            NoReply;
}   msgq_info;

/* each CHANNEL support 50 ID filter in most */
const uint8 MAX_ID_FILTER_EACH_CHN = 50;

typedef struct
{
    int32 pid;
    int32 chid;
    int32 bus;
    bool bus_config;
    CIDCMsgQueue* msgQReceiver;//Created by CLient, for Client Receive Msg from Server
    int32   alarm;
    uint8 NodeID;
    uint8 idFilterNumber;
    int32 id_filter[MAX_ID_FILTER_EACH_CHN];
}   conn_info;

#endif /* MSGQUEUE_H_ */
