        .globl  _sdram_preinit
        .text

|--------------------------------------------------------------------------
| Virtual Pages assigned to Type 3 I/O
|
DDR3_CSR_BASE     =     0xFFF8000       | virtual page, for DDR3 CSR
SCRATCH_SRAM_BASE =     0xFFFA000       | virtual page, for scratch sram

	
_sdram_preinit:
	// setup a stack in the SRAM
	lea SCRATCH_SRAM_BASE + 0xF00,sp
	// push a32 on the stack, but we're hard-wiring the value in the offset so 0
	pea 0x0
	// jump to the C init code
	jbsr sdram_init
	// add #4,sp // quite useless, we're returning to code setting the SP!
	jra a6@
