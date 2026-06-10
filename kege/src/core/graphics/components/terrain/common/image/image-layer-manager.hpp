//
//  image-layer-manager.hpp
//  kege
//
//  Created by Kenneth Esdaile on 12/2/24.
//

#ifndef image_layer_manager_hpp
#define image_layer_manager_hpp

#include <stdio.h>
#include <list>
#include "../dependencies.hpp"

namespace kege{

    struct ImageLayerManager;

    class ImageLayer
    {
    public:

        void setImageLayerData( const void* data[2] );
        int32_t imageIndex()const; // refer to the image
        int32_t imageLayer()const; // refer to the layer in the image
        operator bool()const;
        void clear();
        ImageLayer();
        
    private:

        ImageLayerManager* manager;
        friend ImageLayerManager;

        int32_t image_index; // refer to the image
        int32_t image_layer; // refer to the layer in the image
    };
    
}

namespace kege{

    struct ImageLayerManager
    {
    public:
        
        void setImageLayerData( const ImageLayer& layer, const void* data[2] );
        void bind( CommandBuffer& command_buffer );
        void freeHeightmap( ImageLayer& layer );

        //const ref::ShaderSet& getShaderResource()const;
        ImageLayer generateImageLayer();

        bool initialize( uint32_t image_width, uint32_t image_height, uint32_t image_layers );
        bool empty()const;
        void purge();

        ImageLayerManager();
        ~ImageLayerManager();

    private:

        kege::ImageBindings createNewImageArray( int width, int height, int layers );
        bool pushNewImageArray();

    private:

        //std::vector< kege::WriteDescriptorSet > _writes;
        std::vector< uint32_t > _image_layer_counters;
        std::list< ImageLayer > _freed_layers;

        //ref::ShaderSet _shader_resource;
        GraphicsDevice* _graphics;

        ref::Sampler _sampler;

        uint16_t _max_shader_resource_capacity;
        uint32_t _current_image_index;
        uint32_t _image_layers;
        uint32_t _image_height;
        uint32_t _image_width;
    };

}

#endif /* image_layer_manager_hpp */
