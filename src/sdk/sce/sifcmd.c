/* Adapted from nathanialf/ico (MIT; e0881204, sce/libkernl/sifcmd.c): SDK 2.2 source checked against GGX's SDK 2.3 (JP) and 2.5 (KR). */
/* libkernl.a(sifcmd.o) */

#include "eekernel.h"
#include "sifrpc.h"
#include "string.h"
#include "sifcmd.h"
#include "eeregs.h"
#include "libkernl_internal.h"

/* eekernel.h's spelling: FlushCache returns nothing. */

/* the two system commands sceSifInitCmd installs handlers for: the IOP moving
   its command buffer, and the IOP setting one of the software registers */
typedef struct { /* field names derived */
    SifCmdHeader header;
    int newaddr;        /* the IOP buffer address, the number iopbuf holds */
} SifCmdChangeAddrData; /* derived name */

typedef struct { /* field names derived */
    SifCmdHeader header;
    int rno;
    int value;
} SifCmdSRegData; /* derived name */

/* The 32-entry SIF command handler table, a
   handler function and the data pointer handed to it. */
typedef struct { /* field names derived */
    void (*fn)();
    void *data;
} SifCmdEntry; /* derived name */

/* The SIF command data record (fields 3/4 and 5/6
   are what sceSifSetSysCmdBuffer and sceSifSetCmdBuffer swap). */
typedef struct { /* field names derived */
    int sendbuf; /* the uncached alias of cmdSendBuf, its address OR 0x20000000 */
    int ackbuf;  /* the same for cmdAckBuf */
    int iopbuf;  /* the IOP's buffer address, read from and written to a SIF register */
    SifCmdEntry *systbl;
    int nsys;
    SifCmdEntry *usrtbl;
    int nusr;
    int *sreg;
} SifCmdData; /* derived name */

void _set_sreg(void *pkt, void *data)
{
    SifCmdSRegData *sr = pkt;
    SifCmdData *cd = data;

    cd->sreg[sr->rno] = sr->value;
}

void _change_addr(void *pkt, void *data)
{
    SifCmdChangeAddrData *ca = pkt;
    SifCmdData *cd = data;

    cd->iopbuf = ca->newaddr;
}

/* the member's .bss: the uncached send and ack buffers and the init packet on
   64-byte DMA lines, the DMAC handler id, the command data record, the system
   handler table (at least 16-byte aligned) and the software registers */
extern int cmdSendBuf[32] __attribute__((aligned(64))); /* STATIC_EXTERN */ /* derived name */

extern int cmdAckBuf[16] __attribute__((aligned(64))); /* STATIC_EXTERN */ /* derived name */

extern int cmdInitPkt[5] __attribute__((aligned(64))); /* STATIC_EXTERN */ /* derived name */

extern int cmdDmacId; /* STATIC_EXTERN */ /* derived name */

extern SifCmdData cmdData; /* STATIC_EXTERN */ /* derived name */

extern SifCmdEntry sysCmdTable[32] __attribute__((aligned(16))); /* STATIC_EXTERN */ /* derived name */

extern int cmdSreg[32]; /* STATIC_EXTERN */ /* derived name */

int sceSifGetSreg(int reg)
{
    return cmdSreg[reg];
}

int sceSifSetSreg(int reg, int val)
{
    cmdSreg[reg] = val;
    return val;
}

void *sceSifGetDataTable(void)
{
    return &cmdData;
}

/* sifcmd.o's .data: set once sceSifInitCmd has run, cleared by sceSifExitCmd */
static int cmd_inited = 0; /* derived name */

void sceSifInitCmd(void)
{
    SifCmdEntry *h;
    int i;

    DIntr();
    if (cmd_inited != 0) {
        EIntr();
        return;
    }
    cmd_inited = 1;
    cmdData.sendbuf = (int)cmdSendBuf | 0x20000000;
    cmdData.ackbuf = (int)cmdAckBuf | 0x20000000;
    cmdData.iopbuf = 0;
    cmdData.systbl = sysCmdTable;
    cmdData.nsys = 0x20;
    cmdData.usrtbl = 0;
    cmdData.nusr = 0;
    cmdData.sreg = cmdSreg;
    /* Both loops count up with the one i, which ends at 32 for the DMAC
       status write below. */
    h = sysCmdTable;
    for (i = 0; i < 32; i++) {
        h->fn = 0;
        h->data = 0;
        h++;
    }
    for (i = 0; i < 32; i++) {
        cmdSreg[i] = 0;
    }
    (sysCmdTable)[0].fn = _change_addr;
    (sysCmdTable)[0].data = &cmdData;
    (sysCmdTable)[1].fn = _set_sreg;
    (sysCmdTable)[1].data = &cmdData;
    EIntr();
    FlushCache(0);
    if (*D_STAT & 0x20) {
        *D_STAT = 0x20;
    }
    if ((*D5_CHCR & 0x100) == 0) {
        sceSifSetDChain();
    }
    cmdDmacId = AddDmacHandler(5, _sceSifCmdIntrHdlr, 0);
    EnableDmac(5);
    cmdData.iopbuf = sceSifGetReg(0x80000000);
    if (cmdData.iopbuf != 0) {
        cmdInitPkt[4] = (int)cmdSendBuf;
        sceSifSendCmd(0x80000000, cmdInitPkt, 0x14, 0, 0, 0);
        return;
    }
    while ((sceSifGetReg(4) & 0x20000) == 0) {
        ;
    }
    cmdData.iopbuf = sceSifGetReg(2);
    sceSifSetReg(0x80000000, cmdData.iopbuf);
    sceSifSetReg(0x80000001, (int)&cmdData);
    cmdInitPkt[4] = (int)cmdSendBuf;
    cmdInitPkt[3] = 0;
    sceSifSendCmd(0x80000002, cmdInitPkt, 0x14, 0, 0, 0);
}

void sceSifExitCmd(void)
{
    DisableDmac(5);
    RemoveDmacHandler(5, cmdDmacId);
    cmd_inited = 0;
}

SifCmdEntry *sceSifSetCmdBuffer(SifCmdEntry *tbl, int n)
{
    SifCmdEntry *old = cmdData.usrtbl;
    cmdData.usrtbl = tbl;
    cmdData.nusr = n;
    return old;
}

SifCmdEntry *sceSifSetSysCmdBuffer(SifCmdEntry *tbl, int n)
{
    SifCmdEntry *old = cmdData.systbl;
    cmdData.systbl = tbl;
    cmdData.nsys = n;
    return old;
}

/* The handler slot's address is worked as a byte offset into the table the
   id's sign selects, and cid itself then carries that table's address. */
void sceSifAddCmdHandler(int cid, void (*fn)(), void *data)
{
    int off = cid * 8;
    SifCmdEntry *h;
    if (cid >= 0)
        goto pos;
    cid = (int)cmdData.systbl;
    goto done;
pos:
    cid = (int)cmdData.usrtbl;
done:
    off += cid;
    h = (SifCmdEntry *)off;
    h->fn = fn;
    h->data = data;
}

void sceSifRemoveCmdHandler(int cid)
{
    int off = cid * 8;
    if (cid < 0) {
        cid = (int)cmdData.systbl;
    } else {
        cid = (int)cmdData.usrtbl;
    }
    off += cid;
    ((SifCmdEntry *)off)->fn = 0;
}

int _sceSifSendCmd(int cid, int mode, void *pkt, int pktsize, void *src, void *dest, int size)
{
    sceSifDmaData dmat[2];
    SifCmdHeader *header;
    int count;

    if (pktsize < 16 || pktsize > 112) {
        return 0;
    }
    header = pkt;
    count = 0;
    if (size > 0) {
        header->dsize = size;
        dmat[0].src = (unsigned int)src;
        dmat[0].dest = (unsigned int)dest;
        dmat[0].size = size;
        header->dest = dest;
        dmat[0].u.attr = 0;
        count = 1;
        if (mode & 4) {
            sceSifWriteBackDCache(src, size);
        }
    } else {
        header->dsize = 0;
        header->dest = 0;
    }
    dmat[count].src = (unsigned int)pkt;
    dmat[count].dest = cmdData.iopbuf;
    dmat[count].size = pktsize;
    header->cid = cid;
    header->psize = pktsize;
    dmat[count].u.attr = 0x44;
    count++;
    sceSifWriteBackDCache(pkt, pktsize);
    if (mode & 1) {
        return isceSifSetDma(dmat, count);
    }
    return sceSifSetDma(dmat, count);
}

unsigned int sceSifSendCmd(int cid, void *pkt, int pktsize, void *src, void *dest, int size)
{
    return _sceSifSendCmd(cid, 0, pkt, pktsize, src, dest, size);
}

unsigned int isceSifSendCmd(int cid, void *pkt, int pktsize, void *src, void *dest, int size)
{
    return _sceSifSendCmd(cid, 1, pkt, pktsize, src, dest, size);
}
/* The SIF command DMA interrupt handler: copy the packet the IOP sent into a
   local buffer, re-arm the receive chain, then call the system (cid bit 31)
   or user handler for its command id. */
typedef int SifQword __attribute__((mode(TI)));
int _sceSifCmdIntrHdlr(int ch)
{
    SifQword pkt[7];
    SifCmdData *cd;
    volatile SifCmdHeader *h;
    SifQword *src;
    int size; /* the packet size, then its count of quadwords, then the command id */
    int i;
    void (*fn)();

    /* the send buffer is uncached: its header is read through a volatile
       pointer, and so is the copied packet's */
    EIntr();
    cd = &cmdData;
    h = (volatile SifCmdHeader *)cd->sendbuf;
    size = h->psize;
    if (size == 0) {
        return 0;
    }
    h->psize = 0;
    size = (size + 15) / 16;
    src = (SifQword *)h;
    for (i = 0; i < size; i++) {
        pkt[i] = *src++;
    }
    isceSifSetDChain();
    h = (volatile SifCmdHeader *)pkt;
    if (h->cid & 0x80000000) {
        size = h->cid & 0x7FFFFFFF;
        if (size < cd->nsys) {
            fn = cd->systbl[size].fn;
            if (fn != 0) {
                fn(pkt, cd->systbl[size].data);
            }
        }
    } else {
        size = h->cid;
        if (size < cd->nusr) {
            fn = cd->usrtbl[size].fn;
            if (fn != 0) {
                fn(pkt, cd->usrtbl[size].data);
            }
        }
    }
    __asm__ __volatile__("sync.l\n\tei");
    return 0;
}


/* sceSifWriteBackDCache: hand-written assembly, kept in the split */


/* The jr that closes the block above has no delay-slot instruction of its
   own: the next object, libcdvd.a(cdvd000), starts with CB_DelayTh, whose
   first word sits in that slot. */
