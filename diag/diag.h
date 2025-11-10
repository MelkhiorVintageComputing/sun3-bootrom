#ifndef __DIAG_H__
#define __DIAG_H__

/* -- never called from C code */
void selftest ( void );
void diag_berr ( void );
/* */

int mod3write(unsigned int start_addr, unsigned int end_addr, unsigned int pattern);
int mod3read(unsigned int start_addr, unsigned int end_addr, unsigned int pattern);
int mod3read_tramp(unsigned int start_addr, unsigned int end_addr, unsigned int pattern);


#endif __DIAG_H__
