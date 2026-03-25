//
//  terrain.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 8/30/25.
//

#ifndef terrain_hpp
#define terrain_hpp

#include "terrain-settings.hpp"
#include "physical-terrain.hpp"
#include "terrain-renderer.hpp"
#include "terrain-material.hpp"

namespace kege{

    class Terrain : public RefCounter
    {
    public:


        virtual TerrainTile* getTerrainTile( const kege::fvec3& position )const;
        virtual double getHeight( const kege::fvec3& position )const;

        void setOrientation( const kege::fquat& orientation );
        void setPosition( const kege::fvec3& position );

        virtual bool initialize( const kege::SphericalTerrainSettings& settings );
        virtual void notify( void* sender, int event, void* data );

        virtual void update( const kege::fvec3& eye );
        virtual void submitVisibleGeometries();

        void setPhysicalTerrain( kege::Ref< PhysicalTerrain > eye );
        const kege::PhysicalTerrain* getPhysicalTerrain()const;

        TerrainRenderer* getTerrainRenderer();
        kege::Material* getTerrainMaterial();

        virtual ~Terrain();
        Terrain();

    protected:

        kege::Ref< PhysicalTerrain > _physical_terrain;
        bool _init;
    };

}

#endif /* terrain_hpp */
