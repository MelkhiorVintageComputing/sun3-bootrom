/*
 * Copyright (c) 1986 by Sun Microsystems, Inc.
 */

// tjt - this entire file was originally conditional on M25
//  I decided not to do things this way.

/*****************************************************************************
 * 10-Dec-85
 *      ADD signal user that we have lost carrier (or couldn't find it).
 *          This usually means that the cable is bad
 *
 *****************************************************************************/

/*
 * For Sun-3/25 debugging we want DEBUG on.  For production, turn it off.
 */
/* #define DEBUG1 */
/* #define DEBUG        1 */

#define DEBUG
#define DEBUG1

#define LANCEBUG 1      /* Rev C Lance chip bug */
#define PROTOSCRATCH 8192

#if defined(FPGA) && defined(FPGA_WISHBONE)

#include "../sun3/cpu.addrs.h"

#ifndef CSR_BASE
#error "error, no CSR_BASE"
#endif

#if defined(WB_LITTLE_ENDIAN)
#define __optional_bswap32(x) __builtin_bswap32(x)
#else
#define __optional_bswap32(x) (x)
#endif

#define uint8_t unsigned char
#define uint16_t unsigned short
#define uint32_t unsigned int
#define uint64_t unsigned long long

#include "sun3_fpga_csr_zz02_mdio.h"
#include "mii.h"

#define CSR0_EXPECTED (BMCR_ANENABLE | BMCR_FULLDPLX | BMCR_SPEED1000)
#define CSR0_WANTED   (BMCR_ANENABLE | BMCR_FULLDPLX | BMCR_SPEED100)
#define CSR0_RESET    (CSR0_WANTED | BMCR_RESET)

static uint32_t read_phycsr(uint32_t adr);
static uint32_t write_phycsr(uint32_t adr, uint32_t val);
void reset_phy(void);

#endif FPGA

/* 
 * Parameters controlling TIMEBOMB action: times out if chip hangs.
 *
 */
#define TIMEBOMB        1000000 /* One million times around is all... */

/*
 * Sun Lance Ethernet Controller interface
 */
#include "../dev/saio.h"
#include "../h/param.h"
#include "../h/socket.h"
#include "../dev/if_lereg.h"
#include "../dev/if.h"
#include "../h/in.h"
#include "../dev/if_ether.h"
#include "../h/idprom.h"
#include "../sun3/cpu.map.h"
#include "../h/pixrect.h"

#include "../h/protos.h"

//char checker(int);
//char checkSize[sizeof(struct le_init_block)]={checker(&checkSize)};

// in sys/inet.c
void myetheraddr ( struct ether_addr * );

/* Determine whether we are PROM or not. */
/* #define PROM 1 */
// tjt
#define PROM 1

#define LANCERBUFSIZ    1600
#define LANCETBUFSIZ    1600

struct lance_softc {
/*      char            es_scrat[PROTOSCRATCH]; /* work space for nd  */
        struct le_device        *es_lance;      /* Device register address */ /* 0+4 */
        struct ether_addr       es_enaddr;      /* Our Ethernet address */ /* 4+6 */
	u_short                 padding1; // align es_ib on 32-bits /* 10+2 */
        struct le_init_block    es_ib;          /* Initialization block */ /* 12+24 */
        u_char                  fill[6]; /* 36 + 6 */
	u_short                 padding2[3]; // aling es_rmd sur 64 bits /* 42 + 6 */
        struct le_md            es_rmd[2];      /* Receive Descriptor Ring */ /* 48+8 */
        struct le_md            es_tmd;         /* Transmit Descriptor Ring */ /* 56+8 */
        vu_8                    es_rbuf[2][LANCERBUFSIZ]; /* Receive Buffers */ /* 64+3200 */
#ifndef PROM
        vu_8                    es_tbuf[LANCETBUFSIZ];  /* Transmit Buffer */
#endif  PROM
        int                     es_next_rmd;    /* Next descriptor in ring */ /* 6464+4 */
};

// int lancexmit(), lancepoll(), lancereset();
// int lanceprobe(), tftpboot(), lanceopen(), lanceclose(), etherstrategy();
// int nullsys ( null );

static int lanceinit ( struct saioreq * );

static int lanceprobe ( struct saioreq * );
static int lanceopen ( struct saioreq * );
static void lanceclose ( struct saioreq * );
static void install_buf_in_rmd ( vu_8 *, struct le_md *);

static int lancexmit ( struct lance_softc *, char *, int );
static int lancepoll ( struct lance_softc *, char * );
static int lancereset ( struct lance_softc * );//, struct saioreq * );

struct saif leif = {
        (void *) lancexmit,
        (void *) lancepoll,
        (void *) lancereset
};

/*
 * Need to initialize the Ethernet control reg to:
 *      Reset is active
 *      Loopback is NOT active
 *      Interrupt enable is not active.
 */
static u_long lancestd[] = {VIOPG_AMD_ETHER << BYTES_PG_SHIFT};

static struct devinfo lanceinfo = {
        sizeof (struct le_device),
        sizeof (struct lance_softc),
        0,                              /* Local bytes (we use dma) */
        1,                              /* Standard addr count */
        lancestd,                       /* Standard addrs */
        MAP_OBIO,                       /* Device type */
        0,                              /* transfer size handled by ND */
};

struct boottab ledriver = {
        "le",
		(void *) lanceprobe,
		tftpboot,
		(void *) lanceopen,
		(void *) lanceclose,
        (void *) etherstrategy,
		"le: Sun/Lance Ethernet", &lanceinfo,
};

extern struct ether_addr etherbroadcastaddr;

/*
 * Probe for device.
 * Must return -1 for failure for monitor probe
 */
static int
lanceprobe ( struct saioreq *sip )
{
        register short *sp;
        struct idprom id;

        if (IDFORM_1 == idprom(IDFORM_1, &id)
            && id.id_machine == IDM_SUN3_M25)
                return (0);
        else
                return (-1);
}

/*
 * Open Lance Ethernet nd connection, return -1 for errors.
 */
static int
lanceopen ( struct saioreq *sip )
{
        register int result;

#if defined(DEBUG1)
        printf("le: lanceopen (%x)\n", sip);
#endif DEBUG1

        sip->si_sif = &leif;
        if ( lanceinit(sip) || (result = etheropen(sip)) < 0 ) {
                lanceclose(sip);                /* Make sure we kill chip */
#if defined(DEBUG1)
                printf("le: lanceopen --> -1\n");
#endif DEBUG1
                return (-1);
        }

#if defined(DEBUG1)
        printf("le: lanceopen --> %x\n", result);
#endif DEBUG1
        return (result);
}

/*
 * Set up memory maps and Ethernet chip.
 * Returns 1 for error (after printing message), 0 for ok.
 */
static int
lanceinit ( struct saioreq *sip )
{
        register struct lance_softc *es;
        int paddr;
        int i;
        struct idprom id;
        
        /* 
         * Our locals were obtined from DMA space, now put the locals
         * pointer in the standard place.  This is OK since close()
         * will only deallocate from devdata if its size in devinfo is >0.
         */
        es = (struct lance_softc *)sip->si_dmaaddr;
        sip->si_devdata = (caddr_t)es;
        es->es_lance = (struct le_device *) sip->si_devaddr;

        if (IDFORM_1 != idprom(IDFORM_1, &id)) {
                printf("le: No ID PROM\n");
                return 1;
        }

        return lancereset(es);//, sip);
}

/*
 * Basic Lance initialization
 * Returns 1 for error (after printing message), 0 for ok.
 */
static int
lancereset ( struct lance_softc *es) //, struct saioreq *sip )
{
        register struct le_device *le = es->es_lance;
        register struct le_init_block *ib = &es->es_ib;
        int timeout = TIMEBOMB;
        int i;

#if defined(DEBUG1)
        printf("le: lancereset(%x => %x)\n", es, ib);
#endif DEBUG1

	
#if defined(FPGA) && defined(FPGA_WISHBONE)
	reset_phy();
#endif FPGA

        /* Reset the chip */
	asm volatile("": : :"memory");
        le->le_rap = LE_CSR0;
	asm volatile("": : :"memory");
        le->le_csr = LE_STOP;
	asm volatile("": : :"memory");

        /* Perform the basic initialization */
        
        /* Construct the initialization block */
        bzero((caddr_t)&es->es_ib, sizeof (struct le_init_block));

        /* Leave the mode word 0 for normal operating mode */
	
        myetheraddr(&es->es_enaddr);

        /* Oh, for a consistent byte ordering among processors */
        ib->ib_padr[0] = es->es_enaddr.ether_addr_octet[1];
        ib->ib_padr[1] = es->es_enaddr.ether_addr_octet[0];
        ib->ib_padr[2] = es->es_enaddr.ether_addr_octet[3];
        ib->ib_padr[3] = es->es_enaddr.ether_addr_octet[2];
        ib->ib_padr[4] = es->es_enaddr.ether_addr_octet[5];
        ib->ib_padr[5] = es->es_enaddr.ether_addr_octet[4];

        /* Leave address filter 0 -- we don't want Multicast packets */

        ib->ib_rdrp.drp_laddr = (long)&es->es_rmd[0];
        ib->ib_rdrp.drp_haddr = (long)&es->es_rmd[0] >> 16;
        ib->ib_rdrp.drp_len  = 1 << 5;   /* 2 to the 1 power = 2 */

#ifdef FPGA
	printf("ib->ib_rdrp is %x (%x, %x, %x)\n", *(long*)&ib->ib_rdrp, ib->ib_rdrp.drp_laddr, ib->ib_rdrp.drp_haddr, ib->ib_rdrp.drp_len);
#endif FPGA
        
        ib->ib_tdrp.drp_laddr = (long)&es->es_tmd;
        ib->ib_tdrp.drp_haddr = (long)&es->es_tmd >> 16;
        ib->ib_tdrp.drp_len  = 0 << 5;   /* 2 to the 0 power = 1 */

#ifdef FPGA
	printf("ib->ib_tdrp is %x (%x, %x, %x)\n", *(long*)&ib->ib_tdrp, ib->ib_tdrp.drp_laddr, ib->ib_tdrp.drp_haddr, ib->ib_tdrp.drp_len);
#endif FPGA

        /* Clear all the descriptors */
        bzero((caddr_t)es->es_rmd, 2 * sizeof (struct le_md));
        bzero((caddr_t)&es->es_tmd, sizeof (struct le_md));

        /* Give the init block to the chip */
	asm volatile("": : :"memory");
        le->le_rap = LE_CSR1;   /* select the low address register */
	asm volatile("": : :"memory");
        le->le_rdp = (long)ib & 0xffff; // this assumes low-order bit is 0? (test code uses 0xfffe)

	asm volatile("": : :"memory");
        le->le_rap = LE_CSR2;   /* select the high address register */
	asm volatile("": : :"memory");
        le->le_rdp = ((long)ib >> 16) & 0xff;

	asm volatile("": : :"memory");
        le->le_rap = LE_CSR3;   /* Bus Master control register */
	asm volatile("": : :"memory");
        le->le_rdp = LE_BSWP;

	asm volatile("": : :"memory");
        le->le_rap = LE_CSR0;   /* main control/status register */
	asm volatile("": : :"memory");
        le->le_csr = LE_INIT;

	asm volatile("": : :"memory");
        while( ! (le->le_csr & LE_IDON) ) {
                if (timeout-- <= 0) {
                    printf("le: cannot initialize\n");
#if defined(FPGA) && defined(FPGA_WISHBONE)
		    printf("    last dma addr is %x\n", zz02_mdio_cst_debug1_read(0));
		    printf("    last dma value is %x\n", zz02_mdio_cst_debug2_read(0));
		    printf("    word 0 of ivec %x\n", zz02_mdio_csr_iv_0_read(0));
		    printf("    word 1 of ivec %x\n", zz02_mdio_csr_iv_1_read(0));
		    printf("    word 2 of ivec %x\n", zz02_mdio_csr_iv_2_read(0));
		    printf("    word 3 of ivec %x\n", zz02_mdio_csr_iv_3_read(0));
		    printf("    word 4 of ivec %x\n", zz02_mdio_csr_iv_4_read(0));
		    printf("    word 5 of ivec %x\n", zz02_mdio_csr_iv_5_read(0));
#endif
                    return (1);
                }
        }
#if defined(FPGA) && defined(FPGA_WISHBONE)
	printf("le: initialized @ %x\n", ib);
	printf("    last dma addr is %x\n", zz02_mdio_cst_debug1_read(0));
	printf("    last dma value is %x\n", zz02_mdio_cst_debug2_read(0));
	printf("    word 0 of ivec %x\n", zz02_mdio_csr_iv_0_read(0));
	printf("    word 1 of ivec %x\n", zz02_mdio_csr_iv_1_read(0));
	printf("    word 2 of ivec %x\n", zz02_mdio_csr_iv_2_read(0));
	printf("    word 3 of ivec %x\n", zz02_mdio_csr_iv_3_read(0));
	printf("    word 4 of ivec %x\n", zz02_mdio_csr_iv_4_read(0));
	printf("    word 5 of ivec %x\n", zz02_mdio_csr_iv_5_read(0));
#endif
	asm volatile("": : :"memory");
        le->le_csr = LE_IDON;   /* Clear the indication */
	asm volatile("": : :"memory");

        /* Hang out the receive buffers */
        es->es_next_rmd = 0;

        install_buf_in_rmd(es->es_rbuf[0], &es->es_rmd[0]);
        install_buf_in_rmd(es->es_rbuf[1], &es->es_rmd[1]);

	asm volatile("": : :"memory");
        le->le_csr = LE_STRT;
	asm volatile("": : :"memory");

#if defined(DEBUG1)
        printf("le: lancereset returns OK\n");
#endif DEBUG1
        return 0;               /* It all worked! */
}

static void
install_buf_in_rmd ( vu_8 *buffer, struct le_md *rmd )
{
        rmd->lmd_ladr = ((u_32) buffer) & 0xffff;
        rmd->lmd_hadr = (u_32) buffer >> 16;
        rmd->lmd_bcnt = -LANCERBUFSIZ;
        rmd->lmd_mcnt = 0;
        rmd->lmd_flags = LMD_OWN;
	
#if defined(DEBUG1)
        printf("le: install_buf_in_rmd (%x into %x - %d)\n", buffer, rmd, sizeof(struct le_md));
        printf("    %x %x %x %x %x\n", rmd->lmd_ladr, rmd->lmd_hadr, rmd->lmd_bcnt, rmd->lmd_mcnt, rmd->lmd_flags);
        printf("    [%x %x]\n", ((long*)rmd)[0], ((long*)rmd)[1]);
#endif DEBUG1
}

/*
 * Transmit a packet.
 * If possible, just points to the packet without copying it anywhere.
 */
static int
lancexmit ( struct lance_softc *es, char *buf, int count )
{
        register struct le_device *le = es->es_lance;
        struct le_md *tmd = &es->es_tmd; /* Transmit Msg. Descriptor */
        caddr_t tbuf;
        int timeout = TIMEBOMB;
	u_32 addr;	// tjt

#ifdef FPGA
	printf("lancexmit: %x, %x, %d\n", es, buf, count);
	printf("   %x %x %x %x\n", ((long*)buf)[0], ((long*)buf)[1], ((long*)buf)[2], ((long*)buf)[3]);
	printf("   %x %x %x %x\n", ((long*)buf)[4], ((long*)buf)[5], ((long*)buf)[6], ((long*)buf)[7]);
	printf("   %x %x %x %x\n", ((long*)buf)[8], ((long*)buf)[9], ((long*)buf)[10], ((long*)buf)[11]);
	printf("   %x %x %x %x\n", ((long*)buf)[12], ((long*)buf)[13], ((long*)buf)[14], ((long*)buf)[15]);
#endif

#if defined(DEBUG2)
        printf( "xmit np_blkno %x\n",
                ((struct ndpack *)(buf+14))->np_blkno);
#endif DEBUG2
        /*
         * We assume the buffer is in an area accessible by the chip.
         * The caller of xmit is currently ndxmit(), which only sends
         * an nd structure, which we happen to know is allocated in the
         * right area (in fact, it's part of the struct nd which
         * is the first thing in our own es structure).  If we wish to
         * use xmit for some other purpose, the buffer might not be
         * accessible by the chip, so to be general we ought to copy
         * into some accessible place.  HOWEVER, PROM space is really tight,
         * so this generality is not free.
         */
#ifdef PROM
        // tbuf = buf;
        // if ( ((int)tbuf & 0x00F00000) == 0 )
        //         (int)tbuf |= 0x00F00000;
		addr = (u_32) buf;
		if ( addr & 0x00f00000 == 0 )
			addr |= 0x00f00000;
		tbuf = (caddr_t) addr;
#else  PROM
/* FIXME, constant address masks here! */
        if ( ((int)buf & 0x0F000000) == 0x0F000000) { /* we can point to it */
            tbuf = buf;
        } else {
            tbuf = (caddr_t)es->es_tbuf;
            bcopy((caddr_t)buf, tbuf, count);
        }
#endif PROM
        
        tmd->lmd_hadr = (int)tbuf >> 16;
        // tmd->lmd_ladr = (u_short) tbuf;
        tmd->lmd_ladr = ((u_32) tbuf) & 0xffff;
        tmd->lmd_bcnt = -count;
        
#ifdef notdef
        if (tmd->lmd_bcnt < -MINPACKET)
            tmd->lmd_bcnt = -MINPACKET;
#endif notdef
        tmd->lmd_flags3 = 0;
        tmd->lmd_flags = LMD_STP | LMD_ENP | LMD_OWN;
        
	asm volatile("": : :"memory");
        le->le_csr = LE_TDMD;
	asm volatile("": : :"memory");

        do {
            if ( le->le_csr & LE_TINT ) {
                le->le_csr = LE_TINT; /* Clear interrupt */
                break;
            }
        } while ( --timeout > 0);

/*****************************************************************************
 * 10-Dec-85
 *      ADD signal user that we have lost carrier (or couldn't find it).
 *          This usually means that the cable is bad
 *
 *****************************************************************************/

        if (tmd->lmd_flags & LMD_ERR)
            {
            if (tmd->lmd_flags3 & TMD_LCAR)  /* (AMD Lance Transmit msg */
                                             /*  descriptor - 3 (TMD3)  */
                printf("le: No Carrier\n");
            }
/*****************************************************************************/

        if ( (tmd->lmd_flags & LMD_ERR)
        ||   (tmd->lmd_flags3 & TMD_BUFF)
        ||   (timeout <= 0) ) {
#if defined(DEBUG)
		u_short csr0 = le->le_csr;
		u_short csr3;
		asm volatile("": : :"memory");
		le->le_rap = LE_CSR3;
		asm volatile("": : :"memory");
		csr3 = le->le_csr;
		asm volatile("": : :"memory");
		le->le_rap = LE_CSR0;
		
                printf("le: xmit failed - tmd1 flags %x tmd3 %x csr0 %x csr3 %x timeout %x\n",
		       tmd->lmd_flags, tmd->lmd_flags3, csr0, csr3, timeout);
#if defined(FPGA) && defined(FPGA_WISHBONE)
		printf("    last dma addr is %x\n", zz02_mdio_cst_debug1_read(0));
		printf("    last dma value is %x\n", zz02_mdio_cst_debug2_read(0));
		printf("    word 0 of ivec %x\n", zz02_mdio_csr_iv_0_read(0));
		printf("    word 1 of ivec %x\n", zz02_mdio_csr_iv_1_read(0));
		printf("    word 2 of ivec %x\n", zz02_mdio_csr_iv_2_read(0));
		printf("    word 3 of ivec %x\n", zz02_mdio_csr_iv_3_read(0));
		printf("    word 4 of ivec %x\n", zz02_mdio_csr_iv_4_read(0));
		printf("    word 5 of ivec %x\n", zz02_mdio_csr_iv_5_read(0));
#endif
#endif DEBUG
                return (1);
        }
	
#if defined(DEBUG1)
	printf("le: xmit suceeded (?)\n");
#endif DEBUG1

        return (0);
}

static int
lancepoll ( struct lance_softc *es, char *buf )
{
        register struct le_device *le = es->es_lance;
        register struct le_md *rmd;
        register struct ether_header *header;
        int length;

#if defined(DEBUG1) && 0
        printf("le: poll\n");
#endif DEBUG1
        if ( ! (le->le_csr & LE_RINT)  )
                return (0);             /* No packet yet */

#if defined(DEBUG1)
        printf("le: received packet\n");
#endif DEBUG1

	asm volatile("": : :"memory");
        le->le_csr = LE_RINT;           /* Clear interrupt */
	asm volatile("": : :"memory");

        rmd = &es->es_rmd[es->es_next_rmd];

        if ( (rmd->lmd_flags & ~RMD_OFLO) != (LMD_STP|LMD_ENP) ) {
#if defined(DEBUG)
                printf("Receive packet error - rmd flags %x\n",rmd->lmd_flags);
#endif DEBUG
                length = 0;
                goto restorebuf;
        }

        /* Get input data length and a pointer to the ethernet header */

        length = rmd->lmd_mcnt - 4;     /* don't count the 4 CRC bytes */
        header = (struct ether_header *)es->es_rbuf[es->es_next_rmd];

#ifdef LANCEBUG
        /*
         * Check for unreported packet errors.  Rev C of the LANCE chip
         * has a bug which can cause "random" bytes to be prepended to
         * the start of the packet.  The work-around is to make sure that
         * the Ethernet destination address in the packet matches our
         * address.
         */
#define ether_addr_not_equal(a,b)       \
        (  ( *(long  *)(&a.ether_addr_octet[0]) != \
             *(long  *)(&b.ether_addr_octet[0]) )  \
        || ( *(short *)(&a.ether_addr_octet[4]) != \
             *(short *)(&b.ether_addr_octet[4]) )  \
        )

        if( ether_addr_not_equal(header->ether_dhost, es->es_enaddr)
        &&  ether_addr_not_equal(header->ether_dhost, etherbroadcastaddr) ) {
                printf("le: LANCE Rev C Extra Byte(s) bug; Packet punted\n");
                length = 0;
                /* Don't return directly; restore the buffer first */
#if defined(DEBUG1)
                printf("    %x %x <= mine\n", *(long  *)(&es->es_enaddr.ether_addr_octet[0]), *(short  *)(&es->es_enaddr.ether_addr_octet[4]));
                printf("    %x %x <= recvd\n", *(long  *)(&header->ether_dhost.ether_addr_octet[0]), *(short  *)(&header->ether_dhost.ether_addr_octet[4]));
		printf("    rmd => %x, length => %d, header => %x\n", rmd, length, header);
		printf("    rmd content: %x %x\n", ((long*)rmd)[0], ((long*)rmd)[1]);
		printf("    header content: %x %x %x %x\n", ((long*)header)[0], ((long*)header)[1], ((long*)header)[2], ((long*)header)[3]);
		printf("                    %x %x %x %x\n", ((long*)header)[4], ((long*)header)[5], ((long*)header)[6], ((long*)header)[7]);
		printf("                    %x %x %x %x\n", ((long*)header)[8], ((long*)header)[9], ((long*)header)[10], ((long*)header)[11]);
		printf("                    %x %x %x %x\n", ((long*)header)[12], ((long*)header)[13], ((long*)header)[14], ((long*)header)[15]);
#if defined(FPGA) && defined(FPGA_WISHBONE)
		printf("    last dma addr is %x\n", zz02_mdio_cst_debug1_read(0));
		printf("    last dma value is %x\n", zz02_mdio_cst_debug2_read(0));
		printf("    word 0 of ivec %x\n", zz02_mdio_csr_iv_0_read(0));
		printf("    word 1 of ivec %x\n", zz02_mdio_csr_iv_1_read(0));
		printf("    word 2 of ivec %x\n", zz02_mdio_csr_iv_2_read(0));
		printf("    word 3 of ivec %x\n", zz02_mdio_csr_iv_3_read(0));
		printf("    word 4 of ivec %x\n", zz02_mdio_csr_iv_4_read(0));
		printf("    word 5 of ivec %x\n", zz02_mdio_csr_iv_5_read(0));
		printf("    word 6         %x\n", zz02_mdio_csr_iv_6_read(0));
		printf("    word 7         %x\n", zz02_mdio_csr_iv_7_read(0));
#endif
#endif DEBUG1
        }
#endif LANCEBUG

#if defined(DEBUG2)
        if( header->ether_dhost.ether_addr_octet[0] == 0xff )
                printf("Broadcast packet\n");
        else    printf("recv np_blkno %x\n",
                        ((struct ndpack *)(buf+14))->np_blkno);
#endif DEBUG2
        
        /* Copy packet to user's buffer */
        if ( length > 0 )
                bcopy((caddr_t)header, buf, length);

restorebuf:
        rmd->lmd_mcnt = 0;
        rmd->lmd_flags = LMD_OWN;

        /* Get ready to use the other buffer next time */
        /* What about errors ? */
        es->es_next_rmd = 1 - es->es_next_rmd;

        return (length);
}

/*
 * Close down Lance Ethernet device.
 * On the Model 25, we reset the chip and take it off the wire, since
 * it is sharing main memory with us (occasionally reading and writing),
 * and most programs don't know how to deal with that -- they just assume
 * that main memory is theirs to play with.
 */
static void
lanceclose ( struct saioreq *sip )
{
        struct lance_softc *es = (struct lance_softc *) sip->si_devdata;
        struct le_device *le = es->es_lance;

        /* Reset the chip */
	asm volatile("": : :"memory");
        le->le_rap = LE_CSR0;
	asm volatile("": : :"memory");
        le->le_csr = LE_STOP;
}

#if defined(FPGA) && defined(FPGA_WISHBONE)

static uint32_t read_phycsr(uint32_t adr) {
	const uint32_t a32 = 0;
	int timeout = 1000;
	zz02_mdio_reg_addr_write(a32, adr); // target CSR
	zz02_mdio_mdio_command_write(a32, 0x1); // read reg
	while (((zz02_mdio_mdio_status_read(a32) & 0x1) != 0x1) &&
	       (--timeout > 0))
		// wait until the FSM has finished reading
		;
	if (!timeout) {
		printf("ERROR: MDIO read access failed\n");
		return -1;
	}

	return zz02_mdio_mdio_read_read(a32);
}
static uint32_t write_phycsr(uint32_t adr, uint32_t val) {
	const uint32_t a32 = 0;
	int timeout = 1000;
	zz02_mdio_reg_addr_write(a32, adr); // target CSR
	zz02_mdio_mdio_write_write(a32, val); // push value
	zz02_mdio_mdio_command_write(a32, 0x2); // write reg
	while (((zz02_mdio_mdio_status_read(a32) & 0x1) != 0x1) &&
	       (--timeout > 0))
		// wait until the FSM has finished reading
		;
	if (!timeout) {
		printf("ERROR: MDIO write access failed\n");
		return -1;
	}

	return 0;
}

/* everyone else does it ... */
#define millitime() (*romp->v_nmiclock)

void reset_phy(void) {
	const uint32_t a32 = 0;
	uint16_t val;
	int time;
#ifdef DEBUG1
	printf("Reseting ETH PHY\n");
#endif
	zz02_mdio_phy_addr_write(a32, 0); // phy, once and for all, just in case
	val = read_phycsr(0); // read CSR0
	if ((val != CSR0_EXPECTED) && (val != CSR0_WANTED)){
		printf("WARNING: CSR0 is 0x%x\n", val);
	}
	write_phycsr(0, CSR0_RESET); // reset the chip
	time = millitime();
	do {
		DELAY(1000);
		val = read_phycsr(0);
	} while (((val & BMCR_RESET) != 0) && (millitime() < (time + 10000000)));
	if (val & BMCR_RESET) {
		printf("ERROR: MII PHY RESET taking too long\n");
	}
	printf("INFO: PHY control  is 0x%x\n", read_phycsr(0x00));
	printf("      PHY status   is 0x%x\n", read_phycsr(0x01));
	printf("      PHY specific is 0x%x\n", read_phycsr(0x11));
}
#endif
/* THE END */
