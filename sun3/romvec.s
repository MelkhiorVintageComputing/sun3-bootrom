|
|       @(#)romvec.s 1.1 86/09/27
|       Copyright (c) 1986 by Sun Microsystems, Inc.
|
|       Vector Table for the Sun Rom Monitor
|
|       This is the first thing in the PROM.  It provides the RESET
|       vector which starts everything on power-up, as well as assorted
|       information about where to find things in the ROMs and in low
|       memory.
|
| ==>   WHEN ADDING FIELDS PLEASE INCREMENT THE VERSION NUMBER!!!
|
|       to make life easier for people who have to write programs
|       that work whether or not the new field is there.
|
|       @(#)romvec.s 1.20 85/04/24 Copyright (c) 1985 by Sun Microsystems, Inc.
|

#include "../sun3/assym.h"
#include "../h/fbio.h"

//#define SHORT

#ifdef SHORT
 .long  INITSP          | Initial SSP for hardware RESET
 .long  _hardreset      | Initial PC  for hardware RESET
#else
 .long  INITSP          | Initial SSP for hardware RESET
 .long  _hardreset      | Initial PC  for hardware RESET
 .long  _diag_berr      | Bus error handler for diagnostics
 .long  bootaddr        | Addr of addr of boot parameters
 .long  g_memorysize    | Physical onboard memory size
 .long  getchar         | Get char from cur input source
 .long  putchar         | Put char to current output sink
 .long  mayget          | Maybe get char from current input source
 .long  mayput          | Maybe put char to current output sink
 .long  g_echo          | Should getchar echo its input?
 .long  g_insource      | Input source selector
 .long  g_outsink       | Output sink selector
 .long  getkey          | Get next translated key if one exists
 .long  initgetkey      | Initialize before first getkey
 .long  g_translation   | Up/down keyboard translation selector
 .long  g_keybid        | Up/down keyboard ID byte
 .long  g_ax            | V2: R/O value of current X posn on screen
 .long  g_ay            | V2: R/O value of current Y posn on screen
 .long  g_keybuf        | Up/down keycode buffer
 .long  monrev          | New location of monitor revision information
 .long  fwritechar      | Write a character to FB "terminal"
 .long  fbaddr          | Address of frame buffer
 .long  g_font          | Address of current font definition
 .long  fwritestr       | Write a string to FB terminal - faster
 .long  boot_me         | Boot with the specified parameter (like "b" command.)
 .long  g_linebuf       | The line input buffer
 .long  g_lineptr       | Current pointer into g_linebuf
 .long  g_linesize      | Total length of line in g_linebuf
 .long  getline         | Fill g_linebuf from current input source
 .long  getone          | Get next char from g_linebuf
 .long  peekchar        | Peek at next char without reading it
 .long  g_fbthere       | Is frame buffer physically there?
 .long  getnum          | Get next numerics and xlate to binary
 .long  printf          | Print a null-terminated string
 .long  printhex        | Print N digits of a longword in hex
 .long  g_leds          | RAM copy of LED register value
 .long  set_leds        | Sets LED register and RAM copy to argument value
 .long  nmi             | Address that oughta be in level 7 vector
 .long  _abortent       | Monitor entry point from keyboard abort
 .long  g_nmiclock      | Refresh routines's millisecond count
 .long  g_fbtype        | Which type of frame buffer do we have at runtime?
 .long  2               | Version number of sunromvec.
 .long  gp              | Pointer to global data structure
 .long  g_keybzscc      | Pointer to current keyboard's zscc
 .long  g_keyrinit      | ms to wait before repeating a held key
 .long  g_keyrtick      | ms to wait between repetitions ditto
 .long  g_memoryavail   | WAS: ptr to table of strings gen'd by keyboard
 .long  g_resetaddr     | vector address for watchdog resets
 .long  g_resetmap      | page map entry for watchdog resets
 .long  exit_to_mon     | Exit-to-monitor entry point
 .long  g_memorybitmap  | Ptr to Ptr to memory bit map or 0
 .long  setcxsegmap     | Routine to set seg in any context.
 .long  g_vector_cmd    | V2: Handler for 'v' and low 'g' commands
 .long  0               | dummy1z
 .long  0               | dummy2z
 .long  0               | dummy3z
 .long  0               | dummy4z
#endif

|
| Indirect pointer to boot parameters, for historical reasons
|
bootaddr:
 .long  g_bootparam     | Temporary home of boot parameters

# Idiotic junk to avoid complaints from the Gnu linker
.section        .note.GNU-stack,"",%progbits

/* THE END */
