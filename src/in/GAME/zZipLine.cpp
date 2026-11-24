#include "xCommon.h"
#include "zZipLine.h"

#include "xEvent.h"
#include "xstransvc.h"
#include "zScene.h"

void zZipLineInit(void* plat, void* asset)
{
    zZipLineInit((zZipLine*)plat, (zZipLineAsset*)asset);
}

void zZipLineInit(zZipLine* zipline, zZipLineAsset* asset)
{
    xBaseInit(zipline, asset);

    zipline->asset = asset;
    zipline->eventFunc = zZipLineEventCB;

    zipline->link = (xLinkAsset*)(asset + 1);

    zipline->enabled = true;
    zipline->gotDismount = false;
}

void zZipLineSetup(zZipLine* zipline, zScene*)
{
    if (zipline->asset->boundEntityID) {
        U32 id = zipline->asset->boundEntityID;
        zipline->asset->boundEntity = (xEnt*)zSceneFindObject(id);
    }

    U32 id = zipline->asset->startSplineID;
    zipline->asset->spline = (x::NURBS*)xSTFindAsset(id, NULL);
}

void zZipLineEventCB(xBase*, xBase* to, U32 toEvent, const F32*, xBase*, U32)
{
    zZipLine* zipline = (zZipLine*)to;

    switch (toEvent) {
    case eEventOn:
        zipline->enabled = true;
        break;

    case eEventOff:
        zipline->enabled = false;
        break;

    case eEventZipLineDismount:
        zipline->gotDismount = true;
        break;
    }
}

void zZipLineCam::load_cfg()
{
    cfg_common.priority = 128;
    cfg_common.blend_time = 1.0f;

    cfg.speed_move = 10.0f;
    cfg.speed_turn = 10.0f;
    cfg.speed_zone_face = 10.0f;

    cfg.zone_rest.offset.assign(0.0f, -1.5f, 7.0f);
    cfg.zone_rest.face.assign(0.0f, 0.5f, 0.0f);
    cfg.zone_above.offset.assign(0.0f, 2.0f, 5.0f);
    cfg.zone_above.face.assign(0.0f, -1.0f, 0.0f);
    cfg.zone_below.offset.assign(0.0f, 1.0f, 2.0f);
    cfg.zone_below.face.assign(0.0f, 2.5f, 0.0f);
}

void zZipLineCam::create()
{
    xCam::create();

    flags |= 0x20;
    coord_type = XCAM_COORD_SPHERE;
    orient_type = XCAM_ORIENT_QUAT;
    load_cfg();
}

void zZipLineCam::start()
{
    xCam::start();

    end_zone = zone = cfg.zone_rest;
    vel_zone.face = vel_zone.offset = 0.0f;
    refresh_end_coord();
    coord.sphere = end_coord;
    vel_dist = vel_turn = 0.0f;
    coord_to_world(mat.pos, coord.sphere);
    face_target();
}

void zZipLineCam::update(xScene&, F32 dt)
{
    refresh_rest();
    refresh_end_coord();
    apply_spring(dt);
    coord.sphere.origin = end_coord.origin;
    coord_to_world(mat.pos, coord.sphere);
    face_target();
}

void zZipLineCam::refresh_rest()
{
    blend3(end_zone, cfg.zone_above, cfg.zone_rest, cfg.zone_below, analog.offset.y);
    zone.offset = end_zone.offset;
}

void zZipLineCam::refresh_end_coord()
{
    xVec3& A = end_coord.origin;
    A = basis.pos;

    xVec3& AB = end_offset;
    AB.x = basis.up.x * zone.offset.y - basis.at.x * zone.offset.z;
    AB.y = basis.up.y * zone.offset.y - basis.at.y * zone.offset.z;
    AB.z = basis.up.z * zone.offset.y - basis.at.z * zone.offset.z;

    xVec3 BA = { -AB.x, -AB.y, -AB.z };
    xMat3x3 AB_mat;
    end_coord.dist = xMat3x3LookVec(&AB_mat, &BA);
    xQuatFromMat(&end_coord.dir, &AB_mat);
}

void zZipLineCam::apply_spring(F32 dt)
{
    spring_interp(coord.sphere.dist, vel_dist, coord.sphere.dist, end_coord.dist, dt, cfg.speed_move);
    spring_interp(zone.face, vel_zone.face, zone.face, end_zone.face, dt, cfg.speed_zone_face);
    spring_interp(coord.sphere.dir, vel_turn, coord.sphere.dir, end_coord.dir, dt, cfg.speed_turn);
}

void zZipLineCam::face_target()
{
    const xVec3& A = basis.pos;
    const xVec3& C = mat.pos;

    xVec3 dir = { A.x - C.x + basis.up.x * zone.face.y - basis.at.x * zone.face.z,
                  A.y - C.y + basis.up.y * zone.face.y - basis.at.y * zone.face.z,
                  A.z - C.z + basis.up.z * zone.face.y - basis.at.z * zone.face.z };

    F32 dir_dist2 = dir.length2();
    if (dir_dist2 >= 0.001f) {
        dir /= xsqrt(dir_dist2);
        xMat3x3LookVec(&mat, &dir.negate());
        xQuatFromMat(&orient.quat, &mat);
    }
}
