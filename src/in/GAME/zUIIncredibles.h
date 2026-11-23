#ifndef ZUIINCREDIBLES_H
#define ZUIINCREDIBLES_H

#include "zUICustom.h"
#include "xCounter.h"
#include "xSnd.h"

struct RwTexture;

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

class zUICustomCheckpoints : public zUICustom
{
public:
    zUICustomCheckpoints(zUI* ui);

    virtual void Reset();
    virtual bool PreRender();

private:
    RwTexture* texture;
};

class zUICustomSoundMode : public zUICustom
{
public:
    zUICustomSoundMode(zUI* ui);

    virtual void Reset();
    virtual bool HandleEvent(xBase* from, U32 toEvent, const F32* toParam, xBase* toParamWidget, U32 toParamWidgetID);
    virtual void PreUpdate(F32 dt);

private:
    const char* names[xSndMode_Count];
};

class zUICustomStatic : public zUICustom
{
public:
    zUICustomStatic(zUI* ui);

    virtual void Reset();
    virtual void PreUpdate(F32 dt);
    virtual void PreApplyMotionFrame(zUIMotionFrame* frame);

private:
    enum Stage
    {
        Stage_Off,
        Stage_FadeIn,
        Stage_On,
        Stage_FadeOut
    };

    Stage stage;
    F32 timer;
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
