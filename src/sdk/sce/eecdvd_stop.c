/* libcdvd.a(eecdvd.c built for sceCdStop), written from the GGX discs (the
   same code on SDK 2.3 and 2.5) in the style of sceCdRead, adapted from
   nathanialf/ico (MIT). */
#include "eekernel.h"
#include "stdio.h"
#include "sifrpc.h"
#include "libcdvd_internal.h"
#include "libcdvd.h"

extern volatile int _sceCd_c_cb_sem;

int sceCdStop(void)
{
    if (sceCdNcmdDiskReady() == 6) {
        return 0;
    }
    if (_sceCd_ncmd_prechk(11) == 0) {
        return 0;
    }
    sceCdCbfunc_num = 6;
    _sceCd_c_cb_sem = 1;
    if (sceSifCallRpc(&_sceCd_cd_ncmd, 7, 1, 0, 0, 0, 0, _sceCd_cd_callback, (void *)&sceCdCbfunc_num) < 0) {
        sceCdCbfunc_num = 0;
        _sceCd_c_cb_sem = 0;
        SignalSema(*(volatile int *)&_sceCd_ncmd_semid);
        return 0;
    }
    return 1;
}
