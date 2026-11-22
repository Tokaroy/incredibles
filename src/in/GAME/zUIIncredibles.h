#ifndef ZUIINCREDIBLES_H
#define ZUIINCREDIBLES_H

#include "zUICustom.h"
#include "xCounter.h"

namespace stats
{
    void add(const char* name, F32 value);
}

class zUICustomSceneImage : public zUICustom
{
public:
    zUICustomSceneImage(zUI* ui);

    virtual void Reset();
    virtual bool HandleEvent(xBase* from, U32 toEvent, const F32* toParam, xBase* toParamWidget, U32 toParamWidgetID);

    void UpdateTexture();

private:
    xCounter* counter;
};

S32 zUIIncrediblesGetPauseType();

#endif
