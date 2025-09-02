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

namespace kege{

//    struct Stats
//    {
//        uint32_t total_landscape_node = 0;
//        uint32_t total_terrain_node = 0;
//        uint32_t total_terrain = 0;
//        uint32_t total_memory = 0;
//        uint32_t drawcount = 0;
//        uint32_t instances = 0;
//    };

    class Terrain : public RefCounter
    {
    public:

        virtual bool initialize( const kege::TerrainSettings& settings );
        virtual void notify( void* sender, int event, void* data );
        virtual void render( kege::TerrainRenderer& renderer );
        virtual void update( const kege::dvec3& eye );

        void setOrientation( const kege::dquat& orientation );
        void setPosition( const kege::dvec3& position );
        kege::ImageLayerManager* imageLayerManager();

        //void addLayer( LandscapeLayer* layer );

        void setPhysicalTerrain( kege::Ref< PhysicalTerrain > eye );
        const kege::PhysicalTerrain* getPhysicalTerrain()const;

        virtual ~Terrain();
        Terrain();

    protected:

        kege::Ref< PhysicalTerrain > _physical_terrain;
        kege::ImageLayerManager _image_layer_manager;
        bool _init;

    public:

//        Stats stats;
    };

}

#endif /* terrain_hpp */
