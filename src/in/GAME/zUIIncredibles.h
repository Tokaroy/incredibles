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

class zUICustomPowerEffect : public zUICustom
{
public:
    zUICustomPowerEffect(zUI* ui);

    virtual void Reset();
    virtual void PreUpdate(F32 dt);
    virtual bool PreRender();
    virtual void PostRender();
    virtual void PreApplyMotionFrame(zUIMotionFrame* frame);

private:
    F32 scrollTime;
    F32 scrollTotal;
    F32 waitTime;
};

S32 zUIIncrediblesGetPauseType();

#endif
