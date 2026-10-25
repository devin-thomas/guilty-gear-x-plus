/* libdev.a(devvu0.o): VU0 control through the COP2 control registers,
   written from the GGX discs (the same code on SDK 2.3 and 2.5). */
typedef unsigned int u_int;
typedef unsigned short u_short;
typedef int u_long128 __attribute__((mode(TI)));

typedef struct {
    u_long128 vf[32];
    u_int status;   /* $vi16 */
    u_int mac;      /* $vi17 */
    u_int clipping; /* $vi18 */
    u_int r;        /* $vi20 */
    u_int i;        /* $vi21 */
    u_int q;        /* $vi22 */
    u_short vi[16];
} sceDevVu0Cnd;

#define CFC2(dst, vi) __asm__ __volatile__("cfc2 %0, $vi" #vi : "=r"(dst))
#define CFC2_NV(dst, vi) __asm__("cfc2 %0, $vi" #vi : "=r"(dst))
#define CTC2(src, vi) __asm__ __volatile__("ctc2 %0, $vi" #vi : : "r"(src))

int sceDevVu0Sync(void);
int sceDevVu0GetTpc(void);
void sceDevVu0Exec(int addr);

int sceDevVu0CheckBusy(void)
{
    int fbrst;
    CFC2(fbrst, 29);
    return (fbrst & 0xF) == 1;
}

void sceDevVu0Reset(void)
{
    int fbrst;
    CFC2(fbrst, 28);
    fbrst |= 2;
    CTC2(fbrst, 28);
}

int sceDevVu0Pause(void)
{
    int ret = 0;
    int fbrst;

    switch (sceDevVu0Sync()) {
    case 4:
        CFC2(fbrst, 28);
        fbrst |= 1;
        CTC2(fbrst, 28);
    case 0:
    case 3:
        ret = 1;
        break;
    case 1:
    case 2:
        break;
    }
    return ret;
}

int sceDevVu0Continue(void)
{
    int ret = 0;
    int s = sceDevVu0Sync();

    if (s < 4) {
        if (s > 0) {
            ret = 1;
            sceDevVu0Exec(sceDevVu0GetTpc());
        }
    }
    return ret;
}

void sceDevVu0PutDBit(int bit)
{
    u_int fbrst;
    CFC2(fbrst, 28);
    if (bit == 1) {
        fbrst |= 4;
    } else {
        fbrst &= ~4;
    }
    CTC2(fbrst, 28);
}

void sceDevVu0PutTBit(int bit)
{
    u_int fbrst;
    CFC2(fbrst, 28);
    if (bit == 1) {
        fbrst |= 8;
    } else {
        fbrst &= ~8;
    }
    CTC2(fbrst, 28);
}

int sceDevVu0GetDBit(void)
{
    u_int fbrst;
    CFC2(fbrst, 28);
    return (fbrst >> 2) & 1;
}

int sceDevVu0GetTBit(void)
{
    u_int fbrst;
    CFC2(fbrst, 28);
    return (fbrst >> 3) & 1;
}

int sceDevVu0Sync(void)
{
    int stat;
    CFC2(stat, 29);
    if (stat & 2) {
        return 1;
    }
    if (stat & 4) {
        return 2;
    }
    if (stat & 8) {
        return 3;
    }
    return (stat & 1) << 2;
}

void sceDevVu0Exec(int addr)
{
    __asm__ __volatile__("ctc2 %0, $vi27\n\tvcallmsr $vi27" : : "r"(addr & 0xFFFF));
}

int sceDevVu0GetTpc(void)
{
    int tpc;
    CFC2(tpc, 26);
    return tpc & 0xFFFF;
}

int sceDevVu0GetCnd(sceDevVu0Cnd *cnd)
{
    int v;

    if (sceDevVu0CheckBusy()) {
        return 0;
    }
    CFC2_NV(v, 16);
    cnd->status = v;
    CFC2_NV(v, 17);
    cnd->mac = v;
    CFC2_NV(v, 18);
    cnd->clipping = v;
    CFC2_NV(v, 20);
    cnd->r = v;
    CFC2_NV(v, 21);
    cnd->i = v;
    CFC2_NV(v, 22);
    cnd->q = v;
    __asm__("cfc2 %0, $vi0" : "=r"(v));
    cnd->vi[0] = v;
    __asm__("cfc2 %0, $vi1" : "=r"(v));
    cnd->vi[1] = v;
    __asm__("cfc2 %0, $vi2" : "=r"(v));
    cnd->vi[2] = v;
    __asm__("cfc2 %0, $vi3" : "=r"(v));
    cnd->vi[3] = v;
    __asm__("cfc2 %0, $vi4" : "=r"(v));
    cnd->vi[4] = v;
    __asm__("cfc2 %0, $vi5" : "=r"(v));
    cnd->vi[5] = v;
    __asm__("cfc2 %0, $vi6" : "=r"(v));
    cnd->vi[6] = v;
    __asm__("cfc2 %0, $vi7" : "=r"(v));
    cnd->vi[7] = v;
    __asm__("cfc2 %0, $vi8" : "=r"(v));
    cnd->vi[8] = v;
    __asm__("cfc2 %0, $vi9" : "=r"(v));
    cnd->vi[9] = v;
    __asm__("cfc2 %0, $vi10" : "=r"(v));
    cnd->vi[10] = v;
    __asm__("cfc2 %0, $vi11" : "=r"(v));
    cnd->vi[11] = v;
    __asm__("cfc2 %0, $vi12" : "=r"(v));
    cnd->vi[12] = v;
    __asm__("cfc2 %0, $vi13" : "=r"(v));
    cnd->vi[13] = v;
    __asm__("cfc2 %0, $vi14" : "=r"(v));
    cnd->vi[14] = v;
    __asm__("cfc2 %0, $vi15" : "=r"(v));
    cnd->vi[15] = v;
    return 1;
}

int sceDevVu0PutCnd(sceDevVu0Cnd *cnd)
{
    u_long128 *vf = cnd->vf;
    int v;

    if (sceDevVu0CheckBusy()) {
        return 0;
    }
#ifdef VERSION_JP
    /* SDK 2.3 stores the VF registers here; 2.5 loads them */
    __asm__ __volatile__("sqc2 $vf0, 0x0(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf1, 0x10(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf2, 0x20(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf3, 0x30(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf4, 0x40(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf5, 0x50(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf6, 0x60(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf7, 0x70(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf8, 0x80(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf9, 0x90(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf10, 0xa0(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf11, 0xb0(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf12, 0xc0(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf13, 0xd0(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf14, 0xe0(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf15, 0xf0(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf16, 0x100(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf17, 0x110(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf18, 0x120(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf19, 0x130(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf20, 0x140(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf21, 0x150(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf22, 0x160(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf23, 0x170(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf24, 0x180(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf25, 0x190(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf26, 0x1a0(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf27, 0x1b0(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf28, 0x1c0(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf29, 0x1d0(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf30, 0x1e0(%0)" : : "r"(vf));
    __asm__ __volatile__("sqc2 $vf31, 0x1f0(%0)" : : "r"(vf));
#else
    __asm__ __volatile__("lqc2 $vf0, 0x0(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf1, 0x10(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf2, 0x20(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf3, 0x30(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf4, 0x40(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf5, 0x50(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf6, 0x60(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf7, 0x70(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf8, 0x80(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf9, 0x90(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf10, 0xa0(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf11, 0xb0(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf12, 0xc0(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf13, 0xd0(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf14, 0xe0(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf15, 0xf0(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf16, 0x100(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf17, 0x110(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf18, 0x120(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf19, 0x130(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf20, 0x140(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf21, 0x150(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf22, 0x160(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf23, 0x170(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf24, 0x180(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf25, 0x190(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf26, 0x1a0(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf27, 0x1b0(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf28, 0x1c0(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf29, 0x1d0(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf30, 0x1e0(%0)" : : "r"(vf));
    __asm__ __volatile__("lqc2 $vf31, 0x1f0(%0)" : : "r"(vf));
#endif
    v = cnd->status;
    CTC2(v, 16);
    v = cnd->mac;
    CTC2(v, 17);
    v = cnd->clipping;
    CTC2(v, 18);
    v = cnd->r;
    CTC2(v, 20);
    v = cnd->i;
    CTC2(v, 21);
    v = cnd->q;
    CTC2(v, 22);
    v = cnd->vi[0];
    CTC2(v, 0);
    v = cnd->vi[1];
    CTC2(v, 1);
    v = cnd->vi[2];
    CTC2(v, 2);
    v = cnd->vi[3];
    CTC2(v, 3);
    v = cnd->vi[4];
    CTC2(v, 4);
    v = cnd->vi[5];
    CTC2(v, 5);
    v = cnd->vi[6];
    CTC2(v, 6);
    v = cnd->vi[7];
    CTC2(v, 7);
    v = cnd->vi[8];
    CTC2(v, 8);
    v = cnd->vi[9];
    CTC2(v, 9);
    v = cnd->vi[10];
    CTC2(v, 10);
    v = cnd->vi[11];
    CTC2(v, 11);
    v = cnd->vi[12];
    CTC2(v, 12);
    v = cnd->vi[13];
    CTC2(v, 13);
    v = cnd->vi[14];
    CTC2(v, 14);
    v = cnd->vi[15];
    CTC2(v, 15);
    return 1;
}