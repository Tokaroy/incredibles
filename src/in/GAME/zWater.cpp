#include "xCommon.h"
#include "zWater.h"

#include "xDynAsset.h"
#include "xEvent.h"
#include "xIni.h"
#include "zBase.h"

struct zWaterBody::asset_type : xDynAsset
{
    // zWaterBody::asset_type::flags
    enum
    {
        FLAG_VISIBLE = 0x1,
        FLAG_NO_OVERFILL = 0x2
    };

    U32 flags;
    U32 motion_type;
    U32 body;
    U32 facade_refract;
    U32 facade_reflect;
    U32 light_dir;
};

namespace {

struct motion_config
{
    char name[64];
    U32 hash_id;
    S32 motion_type;
    F32 lod_dist_min;
    F32 lod_dist_interval;
    S32 shimmer;
    S32 refract_glow;
    U32 random_seed;
    F32 amplitude_min;
    F32 gravity;
    F32 wind_speed;
    F32 wind_dir;
    F32 scale_amplitude;
    F32 scale_speed;
    F32 scale_wave_length;
    struct
    {
        F32 color_mul_red;
        F32 color_mul_green;
        F32 color_mul_blue;
        F32 light_ambient;
        F32 light_diffuse;
        F32 light_transmissive;
        F32 reflectance_min;
        F32 reflect_intensity;
        F32 reflect_magnitude;
        F32 refract_min;
        F32 refract_max;
        F32 refract_dist_min;
        F32 refract_dist_max;
        F32 refract_ymax;
    } shading;
};

void stay_hidden_dammit(xEnt* ent); // UNUSED
void load_motion_config(motion_config& cfg, const xIniValue* values, S32 values_size); // UNUSED
void load_motion_configs(); // UNUSED

} // namespace

void zWaterBody::load(xBase& data, xDynAsset& asset, size_t asset_size)
{
    ((zWaterBody&)data).load((asset_type&)asset);
}

void zWaterBody::show()
{
    visible = true;
}

void zWaterBody::hide()
{
    visible = false;
}

void zWaterBody::load(const asset_type& a)
{
    asset = (asset_type*)&a;
    xBaseInit(this, asset);
    baseType = eBaseTypeWaterBody;
    eventFunc = cb_dispatch;
    if (linkCount) link = (xLinkAsset*)(asset + 1);

    body_id = -1;
    visible = asset->flags & asset_type::FLAG_VISIBLE;
}

U32 zWaterBody::get_bodyID() { return asset->body; }

void zWaterBody::cb_dispatch(xBase*, xBase* to, U32 event, const F32*, xBase*, U32)
{
    zWaterBody& e = *(zWaterBody*)to;

    switch (event) {
    case eEventVisible:
        e.show();
        break;
    case eEventInvisible:
        e.hide();
        break;
    }
}
