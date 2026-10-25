#include "generated/main.h"

void debugf(char *format, ...) {
}

void fdebugf(FILE *stream, char *format, ...) {
}

int sceSifInitRpc(unsigned int mode);
int sceCdInit(int init_mode);
int sceCdMmode(int media);
int sceSifRebootIop(const char *img);
int sceSifSyncIop(void);
int sceFsReset(void);
int sceGsSyncV(int mode);
void FileIRXLoad(char *fname);
void GraphInit(void);
void TextRead(void);
void PadReadInit(void);
void FileInit(void);
void FadeInit(void);
void MemoryInit();
void MeterInit(void);
void JobInit(void);
void heap_alloc(void);
void GS_runtime_preset(void);
void MeterReset(void);
void GraphBegin(void);
void PadReadMain(void);
void SnDrvMain(void);
void SpriteInit(void);
void TextColor(int rgb);
void TextPutD(u_int x, u_int y, long int data, u_int num);
int MeterColorChange(int col);
int CreditExecute(void);
void JobMain(void);
void SpriteDisplay(void);
void FadeExecute(void);
int SongContExe(void);
void FileReadMain(void);
void MemoryMain(void);
void GraphEnd(void);
void soft_reset_check(void);
extern u_int TotalCounter;
extern int JSTokorotenFlag;
extern int JobMode;

int main(int argc, char **argv)
{
    int i;
    int j;

    se_max_temp = 0;
    for (i = 0; i < 20; i++) {
        se_req_temp[i] = 0;
        for (j = 0; j < 20; j++) {
            se_num_temp[i][j] = 0;
        }
    }
    sceSifInitRpc(0);
    sceCdInit(0);
    sceCdMmode(1);
#ifdef VERSION_KR
    debugf("%s\n", "IOPRP250.IMG");
#else
    debugf("%s\n", "IOPRP23.IMG");
#endif
#ifdef VERSION_KR
    while (!sceSifRebootIop("cdrom0:\\IOPRP250.IMG;1"))
#else
    while (!sceSifRebootIop("cdrom0:\\IOPRP23.IMG;1"))
#endif
    {
    }
    while (!sceSifSyncIop()) {
    }
    sceSifInitRpc(0);
    sceCdInit(0);
    sceCdMmode(1);
    sceFsReset();
    debugf("CD Init Over\n");
    FileIRXLoad("SIO2MAN.IRX");
    FileIRXLoad("MCMAN.IRX");
    FileIRXLoad("MCSERV.IRX");
    FileIRXLoad("PADMAN.IRX");
    FileIRXLoad("LIBSD.IRX");
    FileIRXLoad("SNDRV.IRX");
    debugf("IRX Init Over\n");
    GraphInit();
    TextRead();
    PadReadInit();
    FileInit();
    FadeInit();
#ifdef VERSION_KR
    MemoryInit("BKSLPM-64525GGX");
#else
    MemoryInit("BISLPS-20137GGX");
#endif
    MeterInit();
    JobInit();
    TotalCounter = 0;
    JSTokorotenFlag = 0;
    heap_alloc();
    debugf("Initialize Over\n");
    while (!sceGsSyncV(0)) {
    }
    while (1) {
        for (i = 18; i >= 0; i--) {
            for (j = 0; j < 20; j++) {
                se_num_temp[i + 1][j] = se_num_temp[i][j];
            }
            se_req_temp[i + 1] = se_req_temp[i];
        }
        for (j = 0; j < 20; j++) {
            se_num_temp[0][j] = 0;
        }
        se_req_temp[0] = 0;
        GS_runtime_preset();
        MeterReset();
        GraphBegin();
        PadReadMain();
#ifdef VERSION_KR
        soft_reset_check();
#endif
        SnDrvMain();
        SpriteInit();
        TextColor(0xFFFFFF);
        TextPutD(0x28, 5, JobMode, 4);
        MeterColorChange(0xFF);
        CreditExecute();
        JobMain();
        SpriteDisplay();
        FadeExecute();
        SongContExe();
        FileReadMain();
        MemoryMain();
        MeterColorChange(0xFFFFFF);
        GraphEnd();
        TotalCounter++;
    }
}
