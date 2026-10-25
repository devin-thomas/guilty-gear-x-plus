#include "generated/fade.h"

static int FadeOutStatus = 0;
static float fadeval;
static int fade_bg_id;
static int FadeSpeed;
static int FadeEndCommand;
static int FadeCockpitFlag;
static int fade_endflag;

int GetFadeCockpitFlag(void)
{
    return FadeCockpitFlag;
}

int GetFadeFlag(void)
{
    return FadeOutStatus;
}

int IsFadeEnd(void)
{
    return fade_endflag;
}

void SetFadeCockpitFlag(int val)
{
    FadeCockpitFlag = val;
}

void SetFadeEndCommand(int val)
{
    FadeEndCommand = val;
}

void SetFadeSpeed(int frame)
{
    FadeSpeed = frame;
}

void SetFadeFlag(void)
{
    FadeOutStatus = 1;
}

void gFadeValSet(float val);

void FadeoutExecute(void)
{
    if (FadeOutStatus) {
        gFadeValSet(fadeval);
        return;
    }
    fadeval = 1.0f;
    gFadeValSet(fadeval);
}

void Rq_Fadein(void)
{
    fade_endflag = 0;
    FadeOutStatus = 1;
    fadeval = 0;
    FadeCockpitFlag = 0;
    FadeoutExecute();
}

void Rq_Fadeout(void)
{
    FadeOutStatus = 2;
    fadeval = 1.0f;
    fade_endflag = 0;
    FadeCockpitFlag = 0;
    FadeoutExecute();
}

int GetFadeVal(void)
{
    return fadeval * 255.0;
}

void SetFadeVal(int val)
{
    fadeval = (float)val / 100.0;
}

void FadeInit(void)
{
    fade_bg_id = 0;
    fade_endflag = 1;
    FadeEndCommand = 1;
    fadeval = 1.0f;
    FadeSpeed = 60;
    FadeCockpitFlag = 0;
    FadeOutStatus = 0;
    FadeoutExecute();
}

void FadeExecute(void)
{
    int status;
    int mode;

    status = FadeOutStatus;
    mode = status;
    if (mode == 1) {
        fadeval = fadeval + 1.0 / (float)FadeSpeed;
        if (fadeval >= 1.0) {
            if (FadeEndCommand) {
                FadeOutStatus = 0;
                status = 0;
            }
            fade_endflag = 1;
            fadeval = 1.0f;
        }
    } else if (mode == 2) {
        fadeval = fadeval - 1.0 / (float)FadeSpeed;
        if (fadeval <= 0.0) {
            if (FadeEndCommand) {
                FadeOutStatus = 0;
                status = 0;
            }
            fade_endflag = 1;
            fadeval = 0;
        }
    }
    if (status) {
        gFadeValSet(fadeval);
        return;
    }
    fadeval = 1.0f;
    gFadeValSet(fadeval);
}
