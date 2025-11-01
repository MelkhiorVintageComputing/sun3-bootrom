# 1 "../sys/memcpy.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/m68k-linux-gnu/include/stdc-predef.h" 1 3
# 1 "<command-line>" 2
# 1 "../sys/memcpy.S"
# 16 "../sys/memcpy.S"
# 1 "../sys/m68kasm.h" 1
# 17 "../sys/memcpy.S" 2







 .text
 .align 4

 .globl memcpy
 .type memcpy, @function
# 42 "../sys/memcpy.S"
memcpy:
 move.l 4(%sp),%a0 | dest ptr
 move.l 8(%sp),%a1 | src ptr
 move.l 12(%sp),%d1 | len
 cmp.l #8,%d1 | if fewer than 8 bytes to transfer,
 blo .Lresidue | do not optimise
# 59 "../sys/memcpy.S"
 move.l %a0,%d0 | copy of dest
 neg.l %d0
 and.l #3,%d0 | look for the lower two only
 beq 2f | is aligned?
 sub.l %d0,%d1
 lsr.l #1,%d0 | word align needed?
 bcc 1f
 move.b (%a1)+,(%a0)+
1:
 lsr.l #1,%d0 | long align needed?
 bcc 2f
 move.w (%a1)+,(%a0)+
2:



 move.l %d1,%d0
 and.l #3,%d1 | byte residue
 lsr.l #3,%d0
 bcc 1f | carry set for 4-byte residue
 move.l (%a1)+,(%a0)+
1:
 lsr.l #1,%d0 | number of 16-byte transfers
 bcc .Lcopy | carry set for 8-byte residue
 bra .Lcopy8

1:
 move.l (%a1)+,(%a0)+
 move.l (%a1)+,(%a0)+
.Lcopy8:
 move.l (%a1)+,(%a0)+
 move.l (%a1)+,(%a0)+
.Lcopy:

 dbra %d0,1b
 sub.l #0x10000,%d0



 bpl 1b
 bra .Lresidue

1:
 move.b (%a1)+,(%a0)+ | move residue bytes

.Lresidue:

 dbra %d1,1b | loop until done




 move.l 4(%sp),%d0 | return value
 rts
