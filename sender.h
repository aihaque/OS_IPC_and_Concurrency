#ifndef _SENDER_H_
#define _SENDER_H_
#include "list.h"
#include <pthread.h> 

void Sender_init(int socketDescriptors,char* REMOTEIPs,int REMOTEPORTs,List* send_list_param,List* receive_list_param, pthread_mutex_t *localMutex_param);
void Sender_shutdown(void);
void Sender_wait_to_finish(void);
void signal_consumer_sender(void);

#endif