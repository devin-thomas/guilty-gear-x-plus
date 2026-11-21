/* main() from src/ggxeuc/main.c, split for the Core host's fixed-step clock.
 *
 * main() boots the game and then loops forever, one pass per field, waiting
 * for the next vblank inside GraphEnd. The Core host instead calls
 * GameBoot() once and GameField() once per field, so control returns to the
 * host between fields with nothing on the stack. That is the point where
 * the host can step, hash, save and load the Core (ADR-013, ADR-014).
 *
 * build.py refuses extra functions in a game TU, so this copy lives here.
 * main.c is fully matched and does not change; keep the two in step if
 * that ever stops being true.
 */

extern int se_req_temp[20];
extern int se_max_temp;
extern int se_num_temp[20][20];
extern unsigned int TotalCounter;
extern int JSTokorotenFlag;
extern int JobMode;

void debugf(char *format, ...);
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
void MemoryInit(void);
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
void TextPutD(unsigned int x, unsigned int y, long int data, unsigned int num);
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

/* main() up to its main loop. */
void GameBoot(void)
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
    /* main.c passes the memory card directory, "BKSLPM-64525GGX" or
       "BISLPS-20137GGX"; memory.c's MemoryInit takes no argument. */
    MemoryInit();
    MeterInit();
    JobInit();
    TotalCounter = 0;
    JSTokorotenFlag = 0;
    heap_alloc();
    debugf("Initialize Over\n");
    while (!sceGsSyncV(0)) {
    }
}

/* One pass of main()'s loop: one field. */
void GameField(void)
{
    int i;
    int j;

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
