#include "xCommon.h"
#include "xCollideCache.h"

static F32 normalize(xVec3& v)
{
    F32 len2 = v.length2();
    if (len2 > 0.000001f) {
        F32 length = xsqrt(len2);
        F32 scale = 1.0f / length;
        xVec3ScaleC(v, scale);
        return length;
    }

    return 0.0f;
}

static bool ProcessTriangle1(xCollideProcessedTri* tri)
{
    xVec3Sub(tri->edges[0].segment.dir, tri->edges[1].segment.start, tri->edges[0].segment.start);
    xVec3Sub(tri->edges[1].segment.dir, tri->edges[2].segment.start, tri->edges[1].segment.start);
    xVec3Sub(tri->edges[2].segment.dir, tri->edges[0].segment.start, tri->edges[2].segment.start);
    if ((tri->edges[0].segment.length = normalize(tri->edges[0].segment.dir)) == 0.0f ||
        (tri->edges[1].segment.length = normalize(tri->edges[1].segment.dir)) == 0.0f ||
        (tri->edges[2].segment.length = normalize(tri->edges[2].segment.dir)) == 0.0f) {
        return false;
    }

    xVec3Cross(tri->surface.norm, tri->edges[1].segment.dir, tri->edges[0].segment.dir);
    if (normalize(tri->surface.norm) == 0.0f) {
        return false;
    }

    tri->surface.Init(tri->edges[0].segment.start);
    xVec3Cross(tri->edges[0].edge.norm, tri->surface.norm, tri->edges[0].segment.dir);
    xVec3Cross(tri->edges[1].edge.norm, tri->surface.norm, tri->edges[1].segment.dir);
    xVec3Cross(tri->edges[2].edge.norm, tri->surface.norm, tri->edges[2].segment.dir);
    tri->edges[0].edge.Init(tri->edges[0].segment.start);
    tri->edges[1].edge.Init(tri->edges[1].segment.start);
    tri->edges[2].edge.Init(tri->edges[2].segment.start);
    return true;
}

bool xCollideCacheProcessNonCached(const xVec3* pt0, const xVec3* pt1, const xVec3* pt2, xCollideProcessedTri& tri)
{
    tri.edges[0].segment.start = *pt0;
    tri.edges[1].segment.start = *pt1;
    tri.edges[2].segment.start = *pt2;
    return ProcessTriangle1(&tri) ? true : false;
}
