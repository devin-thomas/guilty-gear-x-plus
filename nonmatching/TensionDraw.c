short int GetTensionVal(CHARACTER_WORK *offset);
void polydraw_FBoxOPQ(float x1, float y1, float x2, float y2, float z, unsigned int color1);

void TensionDraw(CHARACTER_WORK *offset, int side)
{
    static short int basepos[2] = { 54, 587 };
    int base = basepos[side];
    int val = GetTensionVal(offset);
    int ten = offset->ply->TensionBarLength * 0xC1 / 10000;
    int a = (side == 0) ? 1 : -1;
    unsigned int color;
    float b;
    int i;
    static float cnt[2];
    int offl;

    offl = userinfo.offs_cockpit_lower;
    if (offset->ply->OverKillFlag == 1) {
        if (side == 0) {
            CockpitDraw(0x2B, 0x1C, offl + 0x1C0, 49.0f, 0);
        } else {
            CockpitDraw(0x2B, 0x266, offl + 0x1C0, 49.0f, 1);
        }
        color = gStripGetFadeColor(0xFFFF0000);
        polydraw_FBoxOPQ(base, offl + 0x1CC, a * ten + base, offl + 0x1D8, 50.0f, color);
        return;
    }
    if (offset->ply->OverKillFlag != 0) {
        return;
    }
    if (offset->ply->NegativeVal > 6000) {
        FontDraw("DANGER", cpsenterpos[side] - 0x24, offl + 0x1C0, 49.0f, 0xFF);
    }
    if (side == 0) {
        CockpitDraw(0x28, 0x10, offl + 0x1C0, 60.0f, 0);
    } else {
        CockpitDraw(0x29, 0x172, offl + 0x1C0, 60.0f, 0);
    }
    if (offset->ply->TensionBalance < -10000) b = 0.05f;
    else if (offset->ply->TensionBalance < -6000) b = 0.15f;
    else if (offset->ply->TensionBalance < -3000) b = 0.3f;
    else if (offset->ply->TensionBalance < -1500) b = 0.6f;
    else if (offset->ply->TensionBalance < 3000) b = 1.0f;
    else b = 2.0f;
    cnt[side] -= b;
    if (ten == 0) {
        cnt[side] = 0;
    }
    for (i = 0; i < 0xC1; i += 8) {
        int h;
        int index;
        float hl;

        if (i < 0x30) {
            h = 0x1D2;
        } else if (i < 0x60) {
            h = 0x1D0;
        } else if (i <= 0x8F) {
            h = 0x1CE;
        } else {
            h = 0x1CC;
        }
        index = cnt[side] + (float)(i / 8);
        if (index & 0x10) {
            index = 0x10 - (index & 0xF);
        } else {
            index &= 0xF;
        }
        hl = (float)index * (1.0f - (float)offset->ply->NegativeVal / 10000.0f);
        if (val < 5000) color = gStripGetFadeColor(0xFF000000 | ((u_int)(hl * 50.0f * 0.0625f + 30.0f) << 16) | ((u_int)(hl * 50.0f * 0.0625f + 30.0f) << 8) | (u_int)(hl * 125.0f * 0.0625f + 105.0f));
        else if (val < 10000) color = gStripGetFadeColor(0xFF000000 | ((u_int)(hl * 100.0f * 0.0625f + 155.0f) << 16) | ((u_int)(hl * 80.0f * 0.0625f + 55.0f) << 8) | (u_int)(hl * 80.0f * 0.0625f + 55.0f));
        else color = gStripGetFadeColor(0xFF000000 | ((u_int)(hl * 100.0f * 0.0625f + 155.0f) << 16) | ((u_int)(hl * 100.0f * 0.0625f + 120.0f) << 8) | (u_int)(hl * 50.0f * 0.0625f + 55.0f));
        if (ten - i >= 8) {
            polydraw_FBoxOPQ(a * i + base, h + offl, a * (i + 8) + base, offl + 0x1D8, 50.0f, color);
        } else if (ten - i > 0) {
            polydraw_FBoxOPQ(a * i + base, h + offl, a * ten + base, offl + 0x1D8, 50.0f, color);
        } else {
            break;
        }
    }
}
