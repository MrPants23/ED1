#pragma once

#include "Collision.h"
#include <list>

class OldBrokeDownEntity;

namespace EDCollision
{
    class Collidable;
    class SPS_CollidableInfo;
    class SPS_Sector;

    typedef void (*SPS_PerSectorResponseFunc)( SPS_CollidableInfo* collidableInfoPtr, SPS_Sector* sectorPtr );
    typedef void (*SPS_PerEntityResponseFunc)( SPS_CollidableInfo* collidableInfoPtr, OldBrokeDownEntity* entityPtr );

    class SPS_CollidableInfo
    {
    public:
        Collidable *collidablePtr;
        SPS_PerSectorResponseFunc perSectorFunc;
        SPS_PerEntityResponseFunc perEntityFunc;

        SPS_CollidableInfo(	Collidable *pCollidable = 0,
            SPS_PerSectorResponseFunc secFunc = 0, 
            SPS_PerEntityResponseFunc entFunc = 0):
        collidablePtr(pCollidable),
            perSectorFunc(secFunc),
            perEntityFunc(entFunc) {}
    };

    class SPS
    {
    public:
        virtual bool InsertEntity(OldBrokeDownEntity *entityPtr) = 0;
        virtual void RemoveEntity(OldBrokeDownEntity *entityPtr) = 0;
        virtual void CheckCollidableAgainst( SPS_CollidableInfo* collidableInfoPtr ) = 0;
        virtual const BoundingVolume* GetBounds(void) const = 0;
    };

    class SPS_Sector
    {
    protected:
        std::list<OldBrokeDownEntity *> containedEntities;
    public:
        void AddEntity(OldBrokeDownEntity *entityPtr){ containedEntities.push_back( entityPtr ); }
        std::list<OldBrokeDownEntity *> &GetEntities(void){ return containedEntities; }
        virtual BoundingVolume* GetBounds(void) = 0;
    };
}