* ASM is mostly M25, with a few differences (need to size memory, ...)
* C isn't always...
* memory\_test (in diagmenus.c) is similar to non-PRISM, but with some code extracted into functions
* rectcopy (in fwritestr.c) is from PRISM with the two planes, rect\_enable is present, delline includes the rect\_enable call, inschar is a bit different?
* the xxboot function (normally in xxboot.c) referenced in the sddriver structure (from sd.c) doesn't call devlopen ?!? (is is open in xxprobe in both cases). It seems other probe function didn't call devopen, but now do (lanceprobe from if\_le.c for instance, though it oculd b ea different function referenced in ledriver...)
* the endtest function in diagmenus.c has lost parameters, which have become global variables... tests are quite different
* of course FERRARI use the AMD Ethernet functions in diagmenus.c
* machdep.c::map has dropped setpgreg() and is using setpgmap() instead ; some other funcitons in diagmenus have also switched to that family of functions
* in scsi\_test (diagmenus.c), the 'F' option is gone ('FIFO/UDC DMA' test)

* cmp.s code is apparently not in the binary
* db.s code is apparently not in the binary
* if\_ie.c code is apparently not in the binary (as expected, as the 3/60 as 'le' not 'ie')
* patt.s code is apparently not in the binary (it seems mem.h has a structure mem\_call that would indirectly ference those functions, but nothing seem to be using it)

* FERRARI has both memory\_test (non-prism & prism), modified so that (at least) the memory test is moved from prism to non-prism. The colormap test is also more complex (to test the Brooktree DAC).
  Also, it seems those tests are a bit weird - the memory\_test from PRISM has lost test\_type == 0 (the initial switch falls to default, if Ghidra's decompiling is correct, and the actual calls from menutests don't use 0 either), but some stuff are activated for test\_type == 0, including the call to NEW::computeBadSIMMModuleNumber (though that is also called from Test10). The actual memory testing, from the non-prism function, is dispatch to a new set of functions.
  They also use global values for error and errors instead of locals.
