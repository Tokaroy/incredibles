#include "xCommon.h"
#include "zUIIncredibles.h"

#include "xEvent.h"
#include "xGlobals.h"
#include "xRand.h"
#include "xString.h"
#include "zBase.h"
#include "zScene.h"
#include "zUI.h"
#include "zUIImage.h"

#include <stdio.h>

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
