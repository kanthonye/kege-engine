//
//  terrain.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 8/30/25.
//

#ifndef terrain_hpp
#define terrain_hpp

#include "quadtree.h"
#include "physical-terrain.hpp"
#include "image-layer-manager.hpp"
#include "terrain-renderer.hpp"
#include "terrain-material.hpp"

namespace kege{

    class Terrain : public RefCounter
    {
    public:


        virtual TerrainTile* getTerrainTile( const kege::dvec3& position )const;
        virtual double getHeight( const kege::dvec3& position )const;
        
        virtual bool initialize( const kege::SphericalTerrainSettings& settings );
        virtual void notify( void* sender, int event, void* data );

        virtual void update( const kege::dvec3& eye );
        virtual void submitVisibleGeometries();

        void setPhysicalTerrain( kege::Ref< PhysicalTerrain > eye );
        const kege::PhysicalTerrain* getPhysicalTerrain()const;

        void setOrientation( const kege::dquat& orientation );
        void setPosition( const kege::dvec3& position );

        TerrainRenderer* getTerrainRenderer();
        MaterialSource* getTerrainMaterial();

        virtual ~Terrain();
        Terrain();

    protected:

        kege::Ref< PhysicalTerrain > _physical_terrain;
        bool _init;
    };

}

#endif /* terrain_hpp */
