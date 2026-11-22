#include "xCommon.h"
#include "zUIIncredibles.h"

#include "xEvent.h"
#include "xGlobals.h"
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
