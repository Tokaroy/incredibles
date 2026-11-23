#include "xCommon.h"
#include "zUIIncredibles.h"

#include "xEvent.h"
#include "xGlobals.h"
#include "xMath.h"
#include "xRand.h"
#include "xSnd.h"
#include "xstransvc.h"
#include "xString.h"
#include "xTextAsset.h"
#include "zBase.h"
#include "zGlobals.h"
#include "zRenderState.h"
#include "zScene.h"
#include "zUI.h"
#include "zUIImage.h"
#include "zUIText.h"

#include <stdio.h>

#define MAX_CHECKPOINTS 8

struct Scene
{
    char levelCode[5];
    U8 pad0;
    U8 mapX;
    U8 mapY;
    const char* levelName;
    const char* levelDescription;
    U16 levelNameLength;
    U16 levelDescriptionLength;
    U32 imageID;
    U32 flags;
};

namespace
{
    Scene scenes[32];
    S32 numScenes;
    char sceneBaseImage[32];
    U32 sceneCurrent;
    U32 sceneCheckPointTotal;
    U32 sceneCheckPointHit;

    void RenderQuadList(RwIm2DVertex* vertex, U32 count);
}

zUICustomSceneImage::zUICustomSceneImage(zUI* ui) : zUICustom(ui), counter(NULL)
{
}

void zUICustomSceneImage::Reset()
{
    counter = NULL;

    if (ui->asset->customWidget) {
        xBase* b = zSceneFindObject(ui->asset->customWidget);
        if (b && b->baseType == eBaseTypeCounter) {
            counter = (xCounter*)b;
        }
    }
}

bool zUICustomSceneImage::HandleEvent(xBase* from, U32 toEvent, const F32* toParam, xBase* toParamWidget, U32 toParamWidgetID)
{
    switch (toEvent) {
    case eEventUIChangeTexture:
        UpdateTexture();
        return false;
    }

    return zUICustom::HandleEvent(from, toEvent, toParam, toParamWidget, toParamWidgetID);
}

void zUICustomSceneImage::UpdateTexture()
{
    U32 scene = counter ? counter->count : sceneCurrent;
    if (scene > numScenes) {
        scene = 0;
    }

    if (scene == 0) {
        char full[64], sceneStr[8];
        SCENEIDSTR(xglobals->sceneCur->sceneID, sceneStr);
        sprintf(full, "%s %s", sceneBaseImage, sceneStr);
        U32 imageID = xStrHash(full);
        ((zUIImage*)ui)->SetTexture(imageID, false);
    } else {
        ((zUIImage*)ui)->SetTexture(scenes[scene].imageID, false);
    }
}

zUICustomCheckpoints::zUICustomCheckpoints(zUI* ui) : zUICustom(ui)
{
}

void zUICustomCheckpoints::Reset()
{
    texture = (RwTexture*)xSTFindAsset(ui->asset->customWidget, NULL);
}

bool zUICustomCheckpoints::PreRender()
{
    if (sceneCheckPointTotal == 0) {
        return false;
    }

    RwRaster* raster = RwTextureGetRaster(texture);

    zRenderState(SDRS_Default);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, raster);
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
    RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
    RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODEGOURAUD);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESSU, (void*)rwTEXTUREADDRESSWRAP);
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESSV, (void*)rwTEXTUREADDRESSWRAP);

    U32 effectiveCheckpoints = MIN(MAX_CHECKPOINTS, sceneCheckPointTotal);

    RwIm2DVertex vertex[MAX_CHECKPOINTS * 4];
    F32 widthPerItem = ui->current.width / MAX_CHECKPOINTS;
    for (U32 i = 0; i < effectiveCheckpoints; i++) {
        F32 leftX = (ui->current.x + widthPerItem * i) * FB_XRES * (1.0f / 640.0f),
            bottomY = (ui->current.y + ui->current.height) * FB_YRES * (1.0f / 480.0f),
            topY = (ui->current.y + (i >= sceneCheckPointHit ? 0.6f * ui->current.height : 0.0f)) * FB_YRES * (1.0f / 480.0f),
            topV = i >= sceneCheckPointHit ? 0.75f : 1.0f;

        RwIm2DVertexSetScreenX(&vertex[i * 4 + 0], leftX);
        RwIm2DVertexSetScreenY(&vertex[i * 4 + 0], topY);
        RwIm2DVertexSetU(&vertex[i * 4 + 0], 0.0f, 0.0f);
        RwIm2DVertexSetV(&vertex[i * 4 + 0], topV, 0.0f);
        RwIm2DVertexSetScreenX(&vertex[i * 4 + 1], leftX);
        RwIm2DVertexSetScreenY(&vertex[i * 4 + 1], bottomY);
        RwIm2DVertexSetU(&vertex[i * 4 + 1], 0.0f, 0.0f);
        RwIm2DVertexSetV(&vertex[i * 4 + 1], 1.0f, 0.0f);
        RwIm2DVertexSetScreenX(&vertex[i * 4 + 2], leftX + 0.75f * widthPerItem * FB_XRES * (1.0f / 640.0f));
        RwIm2DVertexSetScreenY(&vertex[i * 4 + 2], topY);
        RwIm2DVertexSetU(&vertex[i * 4 + 2], 1.0f, 0.0f);
        RwIm2DVertexSetV(&vertex[i * 4 + 2], topV, 0.0f);
        RwIm2DVertexSetScreenX(&vertex[i * 4 + 3], leftX + 0.75f * widthPerItem * FB_XRES * (1.0f / 640.0f));
        RwIm2DVertexSetScreenY(&vertex[i * 4 + 3], bottomY);
        RwIm2DVertexSetU(&vertex[i * 4 + 3], 1.0f, 0.0f);
        RwIm2DVertexSetV(&vertex[i * 4 + 3], 1.0f, 0.0f);
    }

    for (S32 i = 0; i < (S32)effectiveCheckpoints * 4; i++) {
        RwIm2DVertexSetIntRGBA(&vertex[i], ui->current.color.r, ui->current.color.g, ui->current.color.b, ui->current.color.a);
    }

    RenderQuadList(vertex, effectiveCheckpoints);

    return false;
}

zUICustomSoundMode::zUICustomSoundMode(zUI* ui) : zUICustom(ui)
{
}

void zUICustomSoundMode::Reset()
{
    for (U32 i = 0; i < xSndMode_Count; i++) {
        names[i] = xTextFindString(ui->GetAsset()->customWidget + i, NULL);
    }
}

void zUICustomSoundMode::PreUpdate(F32)
{
    ((zUIText*)ui)->SetText(names[gSnd.sndMode]);
}

bool zUICustomSoundMode::HandleEvent(xBase*, U32 toEvent, const F32*, xBase*, U32)
{
    switch (toEvent) {
    case eEventIncrement:
        if (globals.ForceMono) {
            ui->Signal(eEventIncrementFailed);
            return false;
        }

        if (gSnd.sndMode != xSndMode_Mono) {
            S32 oldMode = gSnd.sndMode, newMode = gSnd.sndMode + 1;
            if (newMode >= xSndMode_Count)
                newMode = xSndMode_Count - 1;
            if (newMode != gSnd.sndMode)
                xSndSetMode((xSndMode)newMode);
            ui->Signal(oldMode != newMode ? eEventIncrementSuccess : eEventIncrementFailed);
        } else {
            ui->Signal(eEventIncrementFailed);
        }
        break;

    case eEventDecrement:
        if (globals.ForceMono) {
            ui->Signal(eEventDecrementFailed);
            return false;
        }

        if (gSnd.sndMode != xSndMode_Mono && gSnd.sndMode != xSndMode_Stereo) {
            S32 oldMode = gSnd.sndMode, newMode = gSnd.sndMode - 1;
            if (newMode < 0)
                newMode = 0;
            if (newMode != gSnd.sndMode)
                xSndSetMode((xSndMode)newMode);
            ui->Signal(oldMode != newMode ? eEventDecrementSuccess : eEventDecrementFailed);
        } else {
            ui->Signal(eEventDecrementFailed);
        }
        break;
    }

    return true;
}

xTWEAKABLE F32 POWER_EFFECT_MIN_WAIT = 3.0f;
xTWEAKABLE F32 POWER_EFFECT_MAX_WAIT = 20.0f;
xTWEAKABLE F32 POWER_EFFECT_MIN_SCROLL = 3.0f;
xTWEAKABLE F32 POWER_EFFECT_MAX_SCROLL = 4.0f;

zUICustomPowerEffect::zUICustomPowerEffect(zUI* ui) : zUICustom(ui)
{
}

void zUICustomPowerEffect::Reset()
{
    waitTime = xrand_RandomFloatRange(POWER_EFFECT_MIN_WAIT, POWER_EFFECT_MAX_WAIT);
    scrollTime = xrand_RandomFloatRange(POWER_EFFECT_MIN_SCROLL, POWER_EFFECT_MAX_SCROLL);
    scrollTotal = scrollTime;
}

void zUICustomPowerEffect::PreUpdate(F32 dt)
{
    if (waitTime >= 0.0f)
        waitTime -= dt;
    if (waitTime <= 0.0f) {
        waitTime = 0.0f;
        scrollTime -= dt;
        if (scrollTime < 0.0f) {
            waitTime = xrand_RandomFloatRange(POWER_EFFECT_MIN_WAIT, POWER_EFFECT_MAX_WAIT);
            scrollTime = xrand_RandomFloatRange(POWER_EFFECT_MIN_SCROLL, POWER_EFFECT_MAX_SCROLL);
            scrollTotal = scrollTime;
        }
    }
}

bool zUICustomPowerEffect::PreRender()
{
    RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDZERO);
    return false;
}

void zUICustomPowerEffect::PostRender()
{
}

void zUICustomPowerEffect::PreApplyMotionFrame(zUIMotionFrame* frame)
{
    if (waitTime == 0.0f) {
        const zUIImageAsset* asset = ((zUIImage*)ui)->GetAsset();
        F32 span = asset->u2 - asset->u1;
        if (span < 0.0f)
            span -= 1.0f;
        else
            span += 1.0f;
        frame->offsetU = span * ((scrollTotal - scrollTime) / scrollTotal);
    } else {
        frame->offsetU = 0.0f;
    }
}
