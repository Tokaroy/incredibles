#ifndef ZFIRE_H
#define ZFIRE_H

#include "xBound.h"
#include "iSnd.h"
#include "xSpacePartitionTree.h"

struct xEnt;

namespace xScreenWarp {
struct warper;
}

namespace zFire {

struct property_set
{
    F32 damage;
    F32 knockback;
};

struct spawn_data
{
    U32 flags;
    xVec3 loc;
    xVec3 up;
    F32 radius;
    F32 fuel;
    F32 heat;
    F32 heat_absorb;
    xEnt* driver;
    property_set* props;
};

struct handle
{
    xSpacePartitionTree2D::entry_data* node;
};

void scene_enter();
void scene_exit();
void setup();
void reset();
void update(F32 dt);
void render();

}

struct zFlame
{
    S32 flags;
    F32 radius;
    F32 fuel;
    F32 heat;
    xMat4x3 mat;
    F32 heat_absorb;
    F32 heat_nearby;
    F32 effects_heat;
    F32 alpha;
    xVec2 scale;
    F32 smoke_emitted;
    F32 wisps_emitted;
    xScreenWarp::warper* warp;
    zFire::property_set* props;
    zFire::handle h;
    U8 nearby_size;
    U8 pad1;
    U8 face_frame;
    U8 floor_frame;
    zFlame* nearby[6];
    iSndHandle sound;
    S8 face_flip;
    S8 floor_flip;
    U8 pad2;
    U8 pad3;

    F32 get_damage() const;
    bool hits_bound(const xBound& bound) const { return xSphereHitsBound(mat.pos, radius, bound); }
    F32 get_knockback() const { return props->knockback; }
    void get_bound(xBound& bound) const { xBoundFromSphere(bound, mat.pos, radius); }
    void refuel(F32 fuel, S32 flags);
    void update(F32 dt, F32 heat_decay);
    void update_effects(F32 dt);
    bool create(const zFire::spawn_data& info);
    void destroy();
    F32 absorb_heat(F32 dh);
    void emit_smoke(F32 dheat, F32 dt);
    void unlink_nearby();
    void relink_nearby(zFlame* old_addr);
    void refresh_nearby();
};

#endif
