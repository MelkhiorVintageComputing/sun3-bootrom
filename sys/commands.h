#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include "../h/montrap.h"

void monitor ( struct monintstack monintstack );
void boot_me ( char *string );
void vector_default ( char *addr, char *string );

#endif __COMMANDS_H__
