#ifndef __INET_H__
#define __INET_H__

#include "../dev/saio.h"

void inet_print ( struct in_addr );
void inet_init ( struct saioreq *, struct sainet *, char * );
int ip_output ( struct saioreq *, caddr_t, int, struct sainet *, caddr_t );
int ip_input ( struct saioreq *, caddr_t, struct sainet * );
int in_lnaof ( struct in_addr in );

#endif __INET_H__
