#include "xCommon.h"
#include "zCombatSystem.h"

#include "zCombat.h"
#include "zGlobals.h"

#include <stdlib.h>

#define MAX_FILES 16

enum
{
    MAX_COMBAT = 96
};

namespace {
xEnt* combatList[MAX_COMBAT];
U32 currentCombat;
} // namespace

void zCombatSystemInit()
{
    zCombatSystemReset();
}

void zCombatSystemReset()
{
    currentCombat = 0;
    globals.boundUpdateTime = 0;
}

void zCombatSystemFrameStart()
{
    currentCombat = 0;
}

void zCombatSystemUpdate(F32 dt)
{
    if (currentCombat == 0) {
        return;
    }

    globals.boundUpdateTime++;

    for (U32 i = 0; i < currentCombat; i++) {
        xEnt* ent = combatList[i];
        zCombat* combat = zCombatGetFrom(ent);

        combat->PostUpdate(ent, dt);
    }
}

void zCombatSystemRenderEffects()
{
    if (currentCombat == 0) {
        return;
    }

    for (U32 i = 0; i < currentCombat; i++) {
        xEnt* ent = combatList[i];
        zCombat* combat = zCombatGetFrom(ent);

        combat->RenderAfterEffect(ent);
    }
}

void zCombatSystemUpdateEntity(xEnt* ent, F32 dt)
{
    if (!ent->model->Anim || !ent->model->Anim->Single->State) {
        return;
    }

    zCombat* combat = zCombatGetFrom(ent);

    combat->PreUpdate(ent, dt);

    if (currentCombat < MAX_COMBAT)
        combatList[currentCombat++] = ent;
}

void zCombatHitChooser(xAnimPlay*, xAnimState* state, void* object)
{
    xEnt* ent = (xEnt*)object;
    zCombat* combat = zCombatGetFrom(ent);
    zHitSource source = combat->lastHitSource;
    zHitTarget target = combat->lastHitTarget;

    if (!state->MultiFile) {
        return;
    }

    xAnimMultiFile* multiFile = state->MultiFile;

    xAnimFile* files[MAX_FILES];
    S32 currentFile = 0;
    zHitSource bestSource = zHS_GENERAL;
    zHitTarget bestTarget = zHT_GENERAL;

    for (U32 i = 0; i < multiFile->Count; i++) {
        U32 id = multiFile->Files[i].ID;
        xAnimFile* file = multiFile->Files[i].File;

        U32 idSource = id & 0xFFFF0000;
        zHitSource testSource;

        if (idSource == zHitSourceCCs[zHS_GENERAL]) {
            if (bestSource != zHS_GENERAL) {
                continue;
            }

            testSource = zHS_GENERAL;
        } else if (idSource == zHitSourceCCs[source]) {
            testSource = source;
        } else {
            continue;
        }

        U32 idTarget = id & 0xFF00;
        zHitTarget testTarget;

        if (idTarget == zHitTargetCCs[zHT_GENERAL]) {
            if (bestSource == testSource && bestTarget != zHT_GENERAL) {
                continue;
            }

            testTarget = zHT_GENERAL;
        } else if (idTarget == zHitTargetCCs[target]) {
            testTarget = target;
        } else {
            continue;
        }

        if (bestSource != testSource || bestTarget != testTarget) {
            bestSource = testSource;
            bestTarget = testTarget;
            currentFile = 0;
        }

        if (currentFile < MAX_FILES) {
            files[currentFile++] = file;
        }
    }

    if (currentFile == 0) {
        state->Data = multiFile->Files[0].File;
    } else {
        state->Data = files[rand() % currentFile];
    }
}
