#ifndef ZWATER_H
#define ZWATER_H

#include "xBase.h"

struct xDynAsset;
struct xEnt;
struct xWaterShading;

class zWaterBody : private xBase
{
public:
    struct asset_type;

    S32 motion_type;

private:
    asset_type* asset;
    xEnt* body_ent;
    xEnt** refract_ents;
    S32 refract_ents_size;
    xEnt** reflect_ents;
    S32 reflect_ents_size;
    S32 body_id;
    bool visible;

public:
    static void load(xBase& data, xDynAsset& asset, size_t asset_size);
    static void setup_all(); // UNUSED
    static void reset_all(); // UNUSED
    static void update_all(F32 dt); // UNUSED
    void show();
    void hide();
    void load(const asset_type& a);
    void setup(); // UNUSED
    S32 count_ents(xBase* base, const char* type, S32 depth) const; // UNUSED
    xEnt** load_ents(xEnt** ents, xBase* base) const; // UNUSED
    xEnt** load_ents(S32& size, U32 id, const char* type) const; // UNUSED
    U32 get_bodyID();
    void get_shading(xWaterShading& shading) const; // UNUSED
    static void cb_dispatch(xBase*, xBase* to, U32 event, const F32*, xBase*, U32);
};

void zWaterStartup(); // UNUSED
void zWaterSceneEnter(); // UNUSED
void zWaterSceneExit(); // UNUSED

#endif
