#include "xCommon.h"
#include "zParticleSystems.h"

#include "zParticleSystemWaterfall.h"

class zParticleSystemDummy : public zParticleSystem
{
public:
    zParticleSystemDummy() : zPARTICLESYSTEM(zParticleAsset::SYSTEM_DUMMY, 0) {}

    virtual zParticleGenerator* create_generator(zParticleAsset&) { return NULL; }
    virtual S32 get_asset_size() const { return 0; }
};

zParticleSystem** zParticleSystemCreateAll()
{
    zParticleSystem** systems = new (eMemStaticType_0, 0) zParticleSystem*[zParticleAsset::MAX_SYSTEM];

    systems[zParticleAsset::SYSTEM_DUMMY] = new (eMemStaticType_0, 0) zParticleSystemDummy;
    systems[zParticleAsset::SYSTEM_WATERFALL] = new (eMemStaticType_0, 0) zParticleSystemWaterfall;
    systems[zParticleAsset::SYSTEM_WATERFALL_MIST] = new (eMemStaticType_0, 0) zParticleSystemWaterfallMist;
    systems[zParticleAsset::SYSTEM_WATERFALL_SPLASH] = new (eMemStaticType_0, 0) zParticleSystemWaterfallSplash;

    return systems;
}
