#ifndef __MAPMEM_H__
#define __MAPMEM_H__

#include "../h/pginit.h"

void setupmap ( struct pginit * );
unsigned long mapmem( unsigned long memsize );

#endif __MAPMEM_H__
