/* libgraph.a(sceGsResetPath's member), written from the GGX discs (the same
   code on SDK 2.3 and 2.5); nathanialf/ico (MIT) keeps it as assembly. Reset
   VIF1 and the GIF and send the two-quadword VIF1 reset packet. */
#include "libgraph.h"
#include "eeregs.h"

typedef int u_long128 __attribute__((mode(TI)));

/* STATIC_EXTERN: STCYCL 4/4, STMASK 0, NOP, STMOD 0, MSKPATH3 0, BASE 0,
   OFFSET 0, ITOP 0 (the member's .data), kept in the bins */
extern u_long128 sceGsResetPathPacket[2];

void sceGsResetPath(void)
{
    register int vif __asm__("$4") = 0;

    *(volatile int *)0x10003C10 = 1;
    *(volatile int *)0x10003C20 = 2;
    __asm__ __volatile__("sync.l");
    __asm__ __volatile__("cfc2 %0, $vi28" : "+r"(vif));
    vif |= 0x200;
    __asm__ __volatile__("ctc2 %0, $vi28" : : "r"(vif));
    __asm__ __volatile__("sync.p");
    *(volatile u_long128 *)0x10005000 = sceGsResetPathPacket[0];
    *(volatile u_long128 *)0x10005000 = sceGsResetPathPacket[1];
    *(volatile int *)0x10003000 = 1;
}
