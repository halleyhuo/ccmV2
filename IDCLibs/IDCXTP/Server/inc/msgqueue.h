/*
 * msgqueue.h
 *
 *  Created on: 2017.03.29
 */

#ifndef MSGQUEUE_H_
#define MSGQUEUE_H_

 
#include <errno.h>
#include <mqueue.h> 

#include "Types.h"
#include "xtpcomm.h"

class CIDCMsgQueue;

//Timeout = 150*50=7.5s. Not receive the Stat for 7.5s then remove the
const int32 CHNSTAT_TIMEOUT = 150; 

//Support 10 Channel connect with Driver in MAX. Not the App Process Number.
const uint8 MAX_CONN_NUM = 32;
//extern msgq_info mq_info;
extern conn_info conn_list[MAX_CONN_NUM];


extern bool  msgq_create(int32, int32);
extern int32 msgq_destroy();
extern int32 msgq_recv(void *message);
extern CIDCMsgQueue* conn_channel_get(const uint8 NodeID);


extern bool conn_channel_add(int32, int32,int32 bus = 0);//currently not support different Eth Port
extern CIDCMsgQueue* conn_channel_del(int32, int32);
extern int32 conn_channel_del_all();
extern int32 conn_channel_recv(void*);
extern int32 conn_channel_send_l( CIDCMsgQueue* msgQSrv2Client, void *buf, int32 len);

//extern int32 conn_channel_send( CIDCMsgQueue* msgQ, void *buf, const int32 len );
extern int32 conn_channel_send( void* buf, const int32 len,const int32 pid, const int32 chid );
extern int32 conn_channel_send(void* buf, const int32 len);
extern int32 conn_channel_reply(int32);

extern int32 conn_channel_reset_alarm(const int32 pid, const int32 chid);
extern int32 conn_channel_filter(int32, int32, uint16, uint8);
extern int32 conn_channel_filter_add(int32, int32);
extern int32 conn_channel_filter_del(int32, int32);
extern CIDCMsgQueue* conn_channel_filter_get(int32, int32 &, uint8 &);
extern int32 conn_channel_answer(int32, int32,uint8, uint8);
extern int32 conn_channel_bus_get(int32 pid, int32 chid);
#endif /* MSGQUEUE_H_ */
