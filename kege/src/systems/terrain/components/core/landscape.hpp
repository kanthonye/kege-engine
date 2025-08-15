//
//  landscape.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/21/24.
//

#ifndef landscape_hpp
#define landscape_hpp

#include "quadtree.h"
#include "landscape-layer.hpp"
#include "landscape-settings.h"
#include "image-layer-manager.hpp"
#include "shader-pipeline-library.hpp"

namespace kege{

    struct Stats
    {
        uint32_t total_landscape_node = 0;
        uint32_t total_terrain_node = 0;
        uint32_t total_terrain = 0;
        uint32_t total_memory = 0;
        uint32_t drawcount = 0;
        uint32_t instances = 0;
    };


    class Landscape : public RefCounter
    {
    public:

        virtual bool initialize( const kege::LandscapeSettings& settings );
        virtual void notify( void* sender, int event, void* data );
        virtual void render( kege::CommandBuffer& command_buffer );
        virtual void update( const kege::dvec3& eye );

        const kege::LandscapeSettings* settings()const;
        kege::ImageLayerManager* imageLayerManager();

        void addLayer( LandscapeLayer* layer );

        virtual ~Landscape();

    protected:

        typedef std::vector< LandscapeLayer* > Layers;

        kege::ImageLayerManager _image_layer_manager;
        kege::LandscapeSettings _settings;
        Layers _layers;
        bool _init;

    public:

        Stats stats;
    };

}

#endif /* landscape_hpp */
