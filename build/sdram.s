#NO_APP
	.file	"sdram.c"
	.text
	.align	2
	.type	cdelay, @function
cdelay:
	move.l 4(%sp),%d0
.L2:
	subq.l #1,%d0
	jne .L2
	rts
	.size	cdelay, .-cdelay
	.align	2
	.type	popcount, @function
popcount:
	move.l 4(%sp),%d0
	move.l %d0,%d1
	lsr.l #1,%d1
	and.l #1431655765,%d1
	sub.l %d1,%d0
	move.l %d0,%d1
	and.l #858993459,%d1
	lsr.l #2,%d0
	and.l #858993459,%d0
	add.l %d0,%d1
	move.l %d1,%d0
	lsr.l #4,%d0
	add.l %d1,%d0
	and.l #252645135,%d0
	move.l %d0,%d1
	lsr.l #8,%d1
	add.l %d1,%d0
	move.l %d0,%d1
	clr.w %d1
	swap %d1
	add.l %d1,%d0
	moveq #63,%d1
	and.l %d1,%d0
	rts
	.size	popcount, .-popcount
	.align	2
	.type	command_p0, @function
command_p0:
	move.l 4(%sp),%a0
	lea (2052,%a0),%a0
	move.l 8(%sp),%d0
	ror.w #8,%d0
	swap %d0
	ror.w #8,%d0
	move.l %d0,(%a0)
	move.l 4(%sp),%a0
	lea (2056,%a0),%a0
	move.l #16777216,(%a0)
	rts
	.size	command_p0, .-command_p0
	.align	2
	.type	sdram_read_leveling_rst_delay, @function
sdram_read_leveling_rst_delay:
	move.l 4(%sp),%a0
	lea (4,%a0),%a1
	moveq #1,%d0
	move.l 8(%sp),%d1
	lsl.l %d1,%d0
	ror.w #8,%d0
	swap %d0
	ror.w #8,%d0
	move.l %d0,(%a1)
	lea (20,%a0),%a0
	move.l #16777216,(%a0)
	clr.l (%a1)
	rts
	.size	sdram_read_leveling_rst_delay, .-sdram_read_leveling_rst_delay
	.align	2
	.type	sdram_read_leveling_rst_bitslip, @function
sdram_read_leveling_rst_bitslip:
	move.l 4(%sp),%a0
	lea (4,%a0),%a1
	move.b 11(%sp),%d1
	extb.l %d1
	moveq #1,%d0
	lsl.l %d1,%d0
	ror.w #8,%d0
	swap %d0
	ror.w #8,%d0
	move.l %d0,(%a1)
	lea (28,%a0),%a0
	move.l #16777216,(%a0)
	clr.l (%a1)
	rts
	.size	sdram_read_leveling_rst_bitslip, .-sdram_read_leveling_rst_bitslip
	.align	2
	.type	command_px, @function
command_px:
	move.l %d2,-(%sp)
	move.l 8(%sp),%d1
	move.l 16(%sp),%d0
	move.b 15(%sp),%d2
	cmp.b #2,%d2
	jeq .L9
	cmp.b #3,%d2
	jeq .L10
	cmp.b #1,%d2
	jne .L14
	move.l %d1,%a0
	lea (2076,%a0),%a0
	jra .L15
.L10:
	move.l %d1,%a0
	lea (2124,%a0),%a0
.L15:
	ror.w #8,%d0
	swap %d0
	ror.w #8,%d0
	move.l %d0,(%a0)
	addq.l #4,%a0
	move.l #16777216,(%a0)
	move.l (%sp)+,%d2
	rts
.L9:
	move.l %d1,%a0
	lea (2100,%a0),%a0
	jra .L15
.L14:
	move.l %d0,12(%sp)
	move.l (%sp)+,%d2
	jra command_p0
	.size	command_px, .-command_px
	.align	2
	.type	sdram_read_leveling_inc_delay, @function
sdram_read_leveling_inc_delay:
	move.l 4(%sp),%a0
	lea (4,%a0),%a1
	moveq #1,%d0
	move.l 8(%sp),%d1
	lsl.l %d1,%d0
	ror.w #8,%d0
	swap %d0
	ror.w #8,%d0
	move.l %d0,(%a1)
	lea (24,%a0),%a0
	move.l #16777216,(%a0)
	clr.l (%a1)
	rts
	.size	sdram_read_leveling_inc_delay, .-sdram_read_leveling_inc_delay
	.align	2
	.type	sdram_read_leveling_inc_bitslip, @function
sdram_read_leveling_inc_bitslip:
	move.l 4(%sp),%a0
	lea (4,%a0),%a1
	move.b 11(%sp),%d1
	extb.l %d1
	moveq #1,%d0
	lsl.l %d1,%d0
	ror.w #8,%d0
	swap %d0
	ror.w #8,%d0
	move.l %d0,(%a1)
	lea (32,%a0),%a0
	move.l #16777216,(%a0)
	clr.l (%a1)
	rts
	.size	sdram_read_leveling_inc_bitslip, .-sdram_read_leveling_inc_bitslip
	.align	2
	.type	lfsr.constprop.2, @function
lfsr.constprop.2:
	move.l %d2,-(%sp)
	move.l 8(%sp),%d1
	move.l %d1,%d0
	lsr.l #1,%d0
	moveq #1,%d2
	and.l %d2,%d1
	neg.l %d1
	and.l #-2145386493,%d1
	eor.l %d1,%d0
	move.l (%sp)+,%d2
	rts
	.size	lfsr.constprop.2, .-lfsr.constprop.2
	.align	2
	.type	sdram_dfii_pix_address_write.constprop.8, @function
sdram_dfii_pix_address_write.constprop.8:
	move.l 4(%sp),%d0
	move.b 11(%sp),%d1
	cmp.b #2,%d1
	jeq .L20
	cmp.b #3,%d1
	jeq .L21
	cmp.b #1,%d1
	jne .L25
	move.l %d0,%a0
	lea (2084,%a0),%a0
	jra .L26
.L21:
	move.l %d0,%a0
	lea (2132,%a0),%a0
.L26:
	clr.l (%a0)
	rts
.L20:
	move.l %d0,%a0
	lea (2108,%a0),%a0
	jra .L26
.L25:
	move.l %d0,%a0
	lea (2060,%a0),%a0
	jra .L26
	.size	sdram_dfii_pix_address_write.constprop.8, .-sdram_dfii_pix_address_write.constprop.8
	.align	2
	.type	sdram_dfii_pix_baddress_write.constprop.10, @function
sdram_dfii_pix_baddress_write.constprop.10:
	move.l 4(%sp),%d0
	move.b 11(%sp),%d1
	cmp.b #2,%d1
	jeq .L28
	cmp.b #3,%d1
	jeq .L29
	cmp.b #1,%d1
	jne .L33
	move.l %d0,%a0
	lea (2088,%a0),%a0
	jra .L34
.L29:
	move.l %d0,%a0
	lea (2136,%a0),%a0
.L34:
	clr.l (%a0)
	rts
.L28:
	move.l %d0,%a0
	lea (2112,%a0),%a0
	jra .L34
.L33:
	move.l %d0,%a0
	lea (2064,%a0),%a0
	jra .L34
	.size	sdram_dfii_pix_baddress_write.constprop.10, .-sdram_dfii_pix_baddress_write.constprop.10
	.align	2
	.type	sdram_write_read_check_test_pattern, @function
sdram_write_read_check_test_pattern:
	lea (-20,%sp),%sp
	movem.l #16190,-(%sp)
	move.l 68(%sp),%a2
	move.l 76(%sp),%d0
	lea (48,%sp),%a4
	moveq #4,%d2
	move.l %a4,%d3
	lea (lfsr.constprop.2),%a5
.L36:
	sub.l %a3,%a3
.L37:
	move.l %d0,-(%sp)
	jsr (%a5)
	addq.l #4,%sp
	move.b %d0,(%a4,%a3.l)
	addq.l #1,%a3
	moveq #4,%d1
	cmp.l %a3,%d1
	jne .L37
	subq.l #1,%d2
	addq.l #4,%a4
	jne .L36
	lea (2060,%a2),%a6
	clr.l (%a6)
	lea (2064,%a2),%a5
	clr.l (%a5)
	pea 9.w
	move.l %a2,-(%sp)
	lea command_p0,%a4
	jsr (%a4)
	pea 15.w
	lea cdelay,%a3
	jsr (%a3)
	move.l 60(%sp),%d0
	ror.w #8,%d0
	swap %d0
	ror.w #8,%d0
	move.l %d0,2068(%a2)
	move.l 64(%sp),%d0
	ror.w #8,%d0
	swap %d0
	ror.w #8,%d0
	move.l %d0,2092(%a2)
	move.l 68(%sp),%d0
	ror.w #8,%d0
	swap %d0
	ror.w #8,%d0
	move.l %d0,2116(%a2)
	move.l 72(%sp),%d0
	ror.w #8,%d0
	swap %d0
	ror.w #8,%d0
	move.l %d0,2140(%a2)
	moveq #48,%d5
	lea (%a2,%d5.l),%a0
	move.l (%a0),%d0
	ror.w #8,%d0
	swap %d0
	ror.w #8,%d0
	moveq #0,%d1
	not.b %d1
	and.l %d0,%d1
	move.l %d1,-(%sp)
	move.l %a2,-(%sp)
	move.l #sdram_dfii_pix_address_write.constprop.8,%d4
	move.l %d4,%a0
	jsr (%a0)
	lea (%a2,%d5.l),%a0
	move.l (%a0),%d0
	ror.w #8,%d0
	swap %d0
	ror.w #8,%d0
	moveq #0,%d1
	not.b %d1
	and.l %d0,%d1
	move.l %d1,-(%sp)
	move.l %a2,-(%sp)
	move.l #sdram_dfii_pix_baddress_write.constprop.10,%d7
	move.l %d7,%a0
	jsr (%a0)
	lea (%a2,%d5.l),%a0
	move.l (%a0),%d0
	ror.w #8,%d0
	swap %d0
	ror.w #8,%d0
	pea 23.w
	moveq #0,%d1
	not.b %d1
	and.l %d0,%d1
	move.l %d1,-(%sp)
	move.l %a2,-(%sp)
	move.l #command_px,%d5
	move.l %d5,%a0
	jsr (%a0)
	lea (40,%sp),%sp
	pea 15.w
	jsr (%a3)
	moveq #44,%d6
	lea (%a2,%d6.l),%a0
	move.l (%a0),%d0
	ror.w #8,%d0
	swap %d0
	ror.w #8,%d0
	moveq #0,%d1
	not.b %d1
	and.l %d0,%d1
	move.l %d1,-(%sp)
	move.l %a2,-(%sp)
	move.l %d4,%a0
	jsr (%a0)
	lea (%a2,%d6.l),%a0
	move.l (%a0),%d0
	ror.w #8,%d0
	swap %d0
	ror.w #8,%d0
	moveq #0,%d1
	not.b %d1
	and.l %d0,%d1
	move.l %d1,-(%sp)
	move.l %a2,-(%sp)
	move.l %d7,%a0
	jsr (%a0)
	lea (%a2,%d6.l),%a0
	move.l (%a0),%d0
	ror.w #8,%d0
	swap %d0
	ror.w #8,%d0
	pea 37.w
	moveq #0,%d1
	not.b %d1
	and.l %d0,%d1
	move.l %d1,-(%sp)
	move.l %a2,-(%sp)
	move.l %d5,%a0
	jsr (%a0)
	lea (32,%sp),%sp
	pea 15.w
	jsr (%a3)
	clr.l (%a6)
	clr.l (%a5)
	pea 11.w
	move.l %a2,-(%sp)
	jsr (%a4)
	pea 15.w
	jsr (%a3)
	lea (2072,%a2),%a3
	moveq #1,%d5
	sub.l 88(%sp),%d5
	move.l %d3,%a4
	add.l %d5,%a4
	lea (2168,%a2),%a2
	lea (16,%sp),%sp
	lea popcount,%a5
	moveq #3,%d4
	sub.l 72(%sp),%d4
.L39:
	move.l (%a3),%d0
	ror.w #8,%d0
	swap %d0
	ror.w #8,%d0
	move.l %d0,44(%sp)
	move.b 44(%sp,%d5.l),%d0
	move.b (%a4),%d1
	eor.b %d1,%d0
	and.l #255,%d0
	move.l %d0,-(%sp)
	jsr (%a5)
	addq.l #4,%sp
	move.l %d0,%d3
	move.b 44(%sp,%d4.l),%d0
	move.b 2(%a4),%d1
	eor.b %d1,%d0
	and.l #255,%d0
	move.l %d0,-(%sp)
	jsr (%a5)
	addq.l #4,%sp
	add.l %d3,%d0
	add.l %d0,%d2
	lea (24,%a3),%a3
	addq.l #4,%a4
	cmp.l %a3,%a2
	jne .L39
	move.l %d2,%d0
	movem.l (%sp)+,#31996
	lea (20,%sp),%sp
	rts
	.size	sdram_write_read_check_test_pattern, .-sdram_write_read_check_test_pattern
	.align	2
	.type	sdram_read_leveling_scan_module.isra.0, @function
sdram_read_leveling_scan_module.isra.0:
	movem.l #15416,-(%sp)
	move.l 32(%sp),%d2
	move.l 36(%sp),%d3
	move.l %d3,-(%sp)
	move.l %d2,-(%sp)
	jsr sdram_read_leveling_rst_delay
	addq.l #8,%sp
	moveq #32,%d5
	sub.l %a2,%a2
	lea sdram_write_read_check_test_pattern,%a3
	lea sdram_read_leveling_inc_delay,%a4
.L43:
	pea 42.w
	move.l %d3,-(%sp)
	move.l %d2,-(%sp)
	jsr (%a3)
	move.l %d0,%d4
	pea 84.w
	move.l %d3,-(%sp)
	move.l %d2,-(%sp)
	jsr (%a3)
	add.l %d4,%d0
	lea (24,%sp),%sp
	seq %d1
	extb.l %d1
	and.l #16384,%d1
	sub.l %d0,%a2
	lea 512(%a2,%d1.l),%a2
	move.l %d3,-(%sp)
	move.l %d2,-(%sp)
	jsr (%a4)
	subq.l #1,%d5
	addq.l #8,%sp
	jne .L43
	move.l %a2,%d0
	movem.l (%sp)+,#7228
	rts
	.size	sdram_read_leveling_scan_module.isra.0, .-sdram_read_leveling_scan_module.isra.0
	.align	2
	.type	sdram_leveling_center_module.isra.1, @function
sdram_leveling_center_module.isra.1:
	movem.l #16188,-(%sp)
	move.l 44(%sp),%d3
	move.l 48(%sp),%d4
	move.l %d4,-(%sp)
	move.l %d3,-(%sp)
	lea sdram_read_leveling_rst_delay,%a3
	jsr (%a3)
	addq.l #8,%sp
	clr.l %d2
	lea sdram_write_read_check_test_pattern,%a5
	move.l %a5,%a4
	lea sdram_read_leveling_inc_delay,%a2
.L47:
	pea 42.w
	move.l %d4,-(%sp)
	move.l %d3,-(%sp)
	jsr (%a5)
	move.l %d0,%d5
	pea 84.w
	move.l %d4,-(%sp)
	move.l %d3,-(%sp)
	jsr (%a5)
	move.l %d2,%d6
	addq.l #1,%d6
	add.l %d5,%d0
	lea (24,%sp),%sp
	jeq .L54
	moveq #32,%d0
	cmp.l %d6,%d0
	jeq .L55
	move.l %d4,-(%sp)
	move.l %d3,-(%sp)
	jsr (%a2)
	addq.l #8,%sp
	move.l %d6,%d2
	jra .L47
.L54:
	move.l %d2,%d5
.L46:
	addq.l #1,%d2
	move.l %d4,-(%sp)
	move.l %d3,-(%sp)
	jsr (%a2)
	addq.l #8,%sp
	moveq #-1,%d6
.L50:
	pea 42.w
	move.l %d4,-(%sp)
	move.l %d3,-(%sp)
	jsr (%a4)
	move.l %d0,%d7
	pea 84.w
	move.l %d4,-(%sp)
	move.l %d3,-(%sp)
	jsr (%a4)
	add.l %d7,%d0
	lea (24,%sp),%sp
	jeq .L48
	moveq #-1,%d0
	cmp.l %d6,%d0
	jne .L48
	move.l %d2,%d6
.L48:
	addq.l #1,%d2
	moveq #31,%d0
	cmp.l %d2,%d0
	jlt .L49
	move.l %d4,-(%sp)
	move.l %d3,-(%sp)
	jsr (%a2)
	addq.l #8,%sp
	jra .L50
.L55:
	moveq #32,%d2
	moveq #-1,%d5
	jra .L46
.L49:
	moveq #-1,%d0
	cmp.l %d6,%d0
	jne .L51
	move.l %d2,%d6
.L51:
	add.l %d6,%d5
	move.l %d5,%d5
	jpl .L59
	addq.l #1,%d5
.L59:
	bfextu %d5{#26:#5},%d5
	move.l %d4,-(%sp)
	move.l %d3,-(%sp)
	jsr (%a3)
	pea 100.w
	lea cdelay,%a3
	jsr (%a3)
	lea (12,%sp),%sp
	clr.l %d2
.L52:
	cmp.l %d5,%d2
	jne .L53
	movem.l (%sp)+,#15612
	rts
.L53:
	move.l %d4,-(%sp)
	move.l %d3,-(%sp)
	jsr (%a2)
	pea 100.w
	jsr (%a3)
	addq.l #1,%d2
	lea (12,%sp),%sp
	jra .L52
	.size	sdram_leveling_center_module.isra.1, .-sdram_leveling_center_module.isra.1
	.align	2
	.globl	sdram_init
	.type	sdram_init, @function
sdram_init:
	subq.l #4,%sp
	movem.l #16190,-(%sp)
	move.l 52(%sp),%a2
	lea (44,%a2),%a0
	move.l #33554432,(%a0)
	lea (48,%a2),%a0
	move.l #50331648,(%a0)
	lea (2048,%a2),%a3
	move.l (%a3),%d0
	ror.w #8,%d0
	swap %d0
	ror.w #8,%d0
	moveq #14,%d1
	cmp.l %d0,%d1
	jeq .L61
	move.l #234881024,(%a3)
.L61:
	move.l #16777216,(%a2)
	pea 1000.w
	lea cdelay,%a6
	jsr (%a6)
	clr.l (%a2)
	pea 1000.w
	jsr (%a6)
	lea (2060,%a2),%a5
	clr.l (%a5)
	lea (2064,%a2),%a4
	clr.l (%a4)
	move.l #201326592,(%a3)
	move.l #50000,-(%sp)
	jsr (%a6)
	clr.l (%a5)
	clr.l (%a4)
	move.l #234881024,(%a3)
	pea 10000.w
	jsr (%a6)
	move.l #131072,(%a5)
	move.l #33554432,(%a4)
	pea 15.w
	move.l %a2,-(%sp)
	move.l #command_p0,%d2
	move.l %d2,%a0
	jsr (%a0)
	clr.l (%a5)
	move.l #50331648,(%a4)
	pea 15.w
	move.l %a2,-(%sp)
	move.l %d2,%a0
	jsr (%a0)
	move.l #100663296,(%a5)
	move.l #16777216,(%a4)
	lea (32,%sp),%sp
	pea 15.w
	move.l %a2,-(%sp)
	move.l %d2,%a0
	jsr (%a0)
	move.l #537460736,(%a5)
	clr.l (%a4)
	pea 15.w
	move.l %a2,-(%sp)
	move.l %d2,%a0
	jsr (%a0)
	pea 200.w
	jsr (%a6)
	move.l #262144,(%a5)
	clr.l (%a4)
	pea 3.w
	move.l %a2,-(%sp)
	move.l %d2,%a0
	jsr (%a0)
	pea 200.w
	jsr (%a6)
	move.l (%a3),%d0
	ror.w #8,%d0
	swap %d0
	ror.w #8,%d0
	lea (32,%sp),%sp
	moveq #14,%d1
	cmp.l %d0,%d1
	jeq .L62
	move.l #234881024,(%a3)
.L62:
	clr.l -(%sp)
	move.l %a2,-(%sp)
	lea sdram_read_leveling_rst_delay,%a5
	jsr (%a5)
	clr.l -(%sp)
	move.l %a2,-(%sp)
	lea sdram_read_leveling_rst_bitslip,%a4
	jsr (%a4)
	pea 1.w
	move.l %a2,-(%sp)
	jsr (%a5)
	pea 1.w
	move.l %a2,-(%sp)
	jsr (%a4)
	lea (32,%sp),%sp
	clr.l %d2
	lea (40,%a2),%a0
	move.l %a0,44(%sp)
	move.l #sdram_read_leveling_inc_bitslip,%d4
	move.l %d4,%a5
.L63:
	moveq #1,%d7
	lsl.l %d2,%d7
	ror.w #8,%d7
	swap %d7
	ror.w #8,%d7
	clr.l %d3
.L67:
	lea (4,%a2),%a6
	move.l %d7,(%a6)
	moveq #36,%d5
	add.l %a2,%d5
	move.l %d5,%a0
	move.l #16777216,(%a0)
	clr.l %d0
.L64:
	cmp.l %d0,%d3
	jne .L65
	clr.l (%a6)
	move.l %d2,-(%sp)
	move.l %a2,-(%sp)
	jsr (%a4)
	addq.l #8,%sp
	moveq #8,%d6
.L66:
	move.l %d2,-(%sp)
	move.l %a2,-(%sp)
	jsr (sdram_read_leveling_scan_module.isra.0)
	move.l %d2,-(%sp)
	move.l %a2,-(%sp)
	move.l %d4,%a0
	jsr (%a0)
	subq.l #1,%d6
	lea (16,%sp),%sp
	jne .L66
	addq.l #2,%d3
	moveq #8,%d0
	cmp.l %d3,%d0
	jne .L67
	move.l %d7,(%a6)
	move.l %d5,%a0
	move.l #16777216,(%a0)
	clr.l (%a6)
	subq.l #1,%d2
	jeq .L77
	moveq #1,%d2
	jra .L63
.L65:
	move.l 44(%sp),%a0
	move.l #16777216,(%a0)
	addq.l #1,%d0
	jra .L64
.L77:
	clr.l %d2
	lea (sdram_leveling_center_module.isra.1),%a6
.L68:
	move.l %d2,-(%sp)
	move.l %a2,-(%sp)
	jsr (%a4)
	addq.l #8,%sp
	clr.l %d5
	clr.l %d4
	clr.l %d3
.L69:
	move.l %d2,-(%sp)
	move.l %a2,-(%sp)
	jsr (sdram_read_leveling_scan_module.isra.0)
	move.l %d0,%d7
	move.l %d2,-(%sp)
	move.l %a2,-(%sp)
	jsr (%a6)
	lea (16,%sp),%sp
	cmp.l %d7,%d5
	jcc .L72
	move.l %d3,%d4
	move.l %d7,%d5
.L72:
	moveq #7,%d1
	cmp.l %d3,%d1
	jeq .L70
	move.l %d2,-(%sp)
	move.l %a2,-(%sp)
	jsr (%a5)
	addq.l #1,%d3
	addq.l #8,%sp
	jra .L69
.L78:
	moveq #1,%d2
	jra .L68
.L70:
	move.l %d2,-(%sp)
	move.l %a2,-(%sp)
	jsr (%a4)
	addq.l #8,%sp
	clr.l %d3
.L73:
	cmp.l %d3,%d4
	jne .L74
	move.l %d2,-(%sp)
	move.l %a2,-(%sp)
	jsr (%a6)
	addq.l #8,%sp
	subq.l #1,%d2
	jne .L78
	move.l (%a3),%d0
	ror.w #8,%d0
	swap %d0
	ror.w #8,%d0
	subq.l #1,%d0
	jeq .L75
	move.l #16777216,(%a3)
.L75:
	move.l (%a3),%d0
	ror.w #8,%d0
	swap %d0
	ror.w #8,%d0
	subq.l #1,%d0
	jeq .L76
	move.l #16777216,(%a3)
.L76:
	moveq #1,%d0
	movem.l (%sp)+,#31996
	addq.l #4,%sp
	rts
.L74:
	move.l %d2,-(%sp)
	move.l %a2,-(%sp)
	jsr (%a5)
	addq.l #1,%d3
	addq.l #8,%sp
	jra .L73
	.size	sdram_init, .-sdram_init
	.ident	"GCC: (Debian 8.3.0-2) 8.3.0"
	.section	.note.GNU-stack,"",@progbits
