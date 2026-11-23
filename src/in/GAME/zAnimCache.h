#ifndef ZANIMCACHE_H
#define ZANIMCACHE_H

#include "xMath3.h"

struct xAnimFile;
struct xAnimPlay;
struct zAttackTableState;

struct zAnimCacheEntry
{
    xVec3* data;
    xAnimFile* animFile;
    U16 bone;
};

void zAnimCacheClear();
void zAnimCacheCreateData(zAttackTableState* attackTableStates, U32 attackTableCount, xAnimPlay* animPlay);

#endif
