#ifndef __TRAP_H__
#define __TRAP_H__

typedef int (*vfptr) ( void );

vfptr set_evec ( int, vfptr );
int trap ( void );
int nmi ( void );
int addr_error ( void );
int bus_error ( void );

void exit_to_mon ( void );

void set_leds ( int );
void set_enable ( int );
int get_enable ( void );

void menureset ( void );
void bootreset ( void );
void k2reset ( void );
void softreset ( void );

int sendtokbd ( int );
int peek ( void* );
int pokec ( void*, char );
void resetinstr ( void );

int setbus ( void * );
void unsetbus ( void * );

#endif __TRAP_H__
