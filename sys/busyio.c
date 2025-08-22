/*
 * @(#)busyio.c 1.1 86/09/27
 * Copyright (c) 1986 by Sun Microsystems, Inc.
 */

/*
 * busyio.c:  busywait I/O module for Sun ROM monitor.
 */

#include "../sun3/cpu.addrs.h"
#include "../dev/saio.h"
#include "../dev/zsreg.h"
#include "../sun3/sunmon.h"
#include "../h/globram.h"

#include "../h/protos.h"
#include "../h/config.h"

static int remote_msg ( int ) ;

#ifndef WANT_FB
/* I don't have a better place to put these.
 * We need these stubs for romvec.s when we
 * get rid of the framebuffer/keyboard
 */
void
fwritechar ( unsigned char x ) { }

void
fwritestr ( unsigned char *x, int len ) { }

void
abortfix ( void ) { }

int getkey ( void )
{
	return -1;	/* NOKEY */
}

void initgetkey ( void ) {}
#endif

void
putchar ( unsigned char x )
{
        if (x == '\n')
			putchar('\r');
        while (mayput (x))
			;
}

/*
 *      "Maybe put" routine -- puts a character if possible, returns
 *      zero if it did and -1 if it didn't (because Uart wasn't ready).
 */
int
mayput ( unsigned char x )
{
        if (gp->g_diagecho == 0x12) {
           if (gp->g_outzscc[2 - gp->g_sccflag].zscc_control & ZSRR0_TX_READY) {
                DELAY(10);
                gp->g_outzscc[2 - gp->g_sccflag].zscc_data = x;
#ifdef WANT_FB
                fwritechar (x);
#endif
                return 0;
           }
        } else {
#ifdef WANT_FB
           if (OUTSCREEN == gp->g_outsink) {
                fwritechar (x);
                return 0;
           }
#endif

           if (gp->g_outzscc[2-gp->g_outsink].zscc_control & ZSRR0_TX_READY) {
                DELAY(10);
                gp->g_outzscc[2-gp->g_outsink].zscc_data = x;
                return 0;
           }
        }
        return -1;
}

/*
 * Get, and echo if desired, a characters.  Wait until one arrives.
 */
unsigned char
getchar()
{
        register int c;

        do  c = mayget(); while (c<0);
        if (gp->g_echo)
            putchar( (unsigned char)c);
        return (c);
}

/*
 * Maybe get a character.  Return it if one is there, else return -1.
 */
int
mayget()
{
        int     c;
 
        if (gp->g_diagecho == 0x12) {
#ifdef WANT_KB
           c = getkey();           /* Let's check the keyboard first */
           if (c != -1) {          /* Is there a keyboard character */
                if (c == '#') {
                   remote_msg(0);  /* Initiate message to SCC */
                   c = -1;         /* Invalidate the keyboard input */
                } else if (gp->g_insource != INKEYB) {
                   c = -1;         /* Ignore the keyboard input */
                }
                return(c);         /* pick up keyboard char & quit. */
           }
#endif

           if (!(gp->g_inzscc[2 - gp->g_sccflag].zscc_control & ZSRR0_RX_READY))
                return (-1);       /* no input from the SCC */

           DELAY(10);
           c = gp->g_inzscc[2 - gp->g_sccflag].zscc_data & NOPARITY;
           if (c == '#') { 
              remote_msg(1);  /* Initiate message to Video */
              c = -1;         /* Invalidate the SCC input */ 
           } else if (gp->g_insource == INKEYB) {
              c = -1;         /* Ignore the SCC input */ 
           }
           return(c);
        } else {
#ifdef WANT_KB
           if (INKEYB == gp->g_insource) 
                return (getkey());  /* Just pick up a keyboard char & quit. */
#endif

           if (!(gp->g_inzscc[2-gp->g_insource].zscc_control & ZSRR0_RX_READY))
                return (-1);    /* no char pending */
           DELAY(10);
           return gp->g_inzscc[2-gp->g_insource].zscc_data & NOPARITY;
        }
}

/*
 *      Allow remote messages when in Echo mode
 */

static int
remote_msg ( int scc ) 
{
        int     c;

        printf("\n*** Message from %s:  ", scc == 0 ? "Keyboard" : "SCC");

        for (;;) {              /* Loop until User ends message */
           if (scc) {
                DELAY(10);
                if (!(gp->g_inzscc[2 - gp->g_sccflag].zscc_control & ZSRR0_RX_READY))
                        continue;
                c = gp->g_inzscc[2 - gp->g_sccflag].zscc_data & NOPARITY;
                DELAY(10);
           } else {
#ifdef WANT_KB
                if ((c = getkey()) == -1)       /* Check for keyboard input */
                        continue;
#endif
           }

           if (c == '#') {              /* Special character for EOM */
                printf("\n*** End of Message ***\n");
                break;
           }

           if (c == '\r')               /* Display new line for <CR> */
                printf("\n");
           else                         /* Display other characters */
                putchar(c);
        }
}

/* THE END */
