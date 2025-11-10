#ifndef __COMMON_H__
#define __COMMON_H__

#include "../dev/dklabel.h"

void bzero ( void *p, int n );
void bcopy ( void *src, void *dest, int count);
int chklabel ( struct dk_label *label );

int isspinning ( int (*isready)(char *, int), char *, int );

#endif __COMMON_H__
