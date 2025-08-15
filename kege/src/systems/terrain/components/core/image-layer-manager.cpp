//
//  image-layer-manager.cpp
//  kege
//
//  Created by Kenneth Esdaile on 12/2/24.
//

#include "image-layer-manager.hpp"

namespace kege{

    void ImageLayer::setImageLayerData( const void* data[2] )
    {
        if( manager )
        {
            manager->setImageLayerData( *this, data );
        }
    }
    int32_t ImageLayer::imageIndex()const
    {
        return image_index;
    }
    int32_t ImageLayer::imageLayer()const
    {
        return image_layer;
    }
    ImageLayer::operator bool()const
    {
        return image_index >= 0;
    }
    void ImageLayer::clear()
    {
        if( manager )
        {
            manager->freeHeightmap( *this );
            manager = nullptr;
        }
    }
    ImageLayer::ImageLayer(): manager(nullptr), image_index(-1), image_layer(-1) {}
}


namespace kege{

    void ImageLayerManager::setImageLayerData( const ImageLayer& layer, const void* data[2] )
    {
        if ( layer.image_index < 0 || data == nullptr ) return;

        size_t size = sq( _image_width );
        _resource_binding_sets[ _current_image_index ].resources[0].image->copyFrom(sizeof(float) * size, data[0], layer.image_layer, 0 );
        _resource_binding_sets[ _current_image_index ].resources[1].image->copyFrom(4 * size, data[1], layer.image_layer, 0 );
    }

    const ResourceSet& ImageLayerManager::getShaderResourceLayout()const
    {
        return _resource_set;
    }

    void ImageLayerManager::freeHeightmap( ImageLayer& layer )
    {
        if ( 0 <= layer.image_index ) 
        {
            _freed_layers.push_back( layer );
            layer.image_index = -1;
            layer.image_layer = -1;
        }
    }

    ResourceBindings ImageLayerManager::createNewImageArray( int width, int height, int layers )
    {
        size_t size = sq( _image_width ) * _image_layers;
        ResourceBindings bindings =
        {
            Graphics::createImage2DArray
            (
                IMAGE_ASPECT_COLOR,
                R_32_FLOAT,
                _image_width,
                _image_width,
                _image_layers,
                1, size * sizeof(float),
                nullptr
            ),
            Graphics::createImage2DArray
            (
                IMAGE_ASPECT_COLOR,
                RGBA_8_SRGB,
                _image_width,
                _image_width,
                _image_layers,
                1, size * 4,
                nullptr
            )
        };

        if ( !_sampler )
        {
            _sampler = Graphics::createSampler
            ({
                LINEAR, LINEAR,
                CLAMP_TO_EDGE,
                CLAMP_TO_EDGE,
                CLAMP_TO_EDGE
            });
        }

        bindings[0].image->setSampler( _sampler );
        bindings[1].image->setSampler( _sampler );
        return bindings;
    }

    bool ImageLayerManager::pushNewImageArray()
    {
        if ( _current_image_index >= _max_shader_resource_capacity )
        {
            return false;
        }

        _current_image_index = (uint32_t) _image_layer_counters.size();
        _image_layer_counters.push_back( 0 );

        ResourceBindings images = createNewImageArray( _image_width, _image_height, _image_layers );

        _resource_binding_sets[ _current_image_index ].resources = images;
        kege::Graphics::updateResourceSet( _resource_set, _resource_binding_sets );

        return true;
    }

    ImageLayer ImageLayerManager::generateImageLayer()
    {
        ImageLayer layer;
        if ( !_freed_layers.empty() )
        {
            layer = _freed_layers.front();
            _freed_layers.pop_front();
            return layer;
        }

        if ( _image_layer_counters.empty() )
        {
            if ( !pushNewImageArray() )
            {
                return {};
            };
        }
        else
        {
            if( _image_layer_counters[ _current_image_index ] >= _image_layers )
            {
                if ( !pushNewImageArray() )
                {
                    std::cerr << "[ WARNING : CRITICAL ] : ImageLayerManager max_shader_resource_capacity exceeded.\n";
                    return {};
                }
            }
        }
        
        layer.manager = this;
        layer.image_index = _current_image_index;
        layer.image_layer = _image_layer_counters[ _current_image_index ]++;
        return layer;
    }

    bool ImageLayerManager::initialize( uint32_t image_width, uint32_t image_height, uint32_t image_layers )
    {
        _image_width  = image_width;
        _image_height = image_height;
        _image_layers = image_layers;

        ResourceBindings images = createNewImageArray( _image_width, _image_height, _image_layers );
        _resource_binding_sets.resize( _max_shader_resource_capacity );
        for (int i=0; i<_max_shader_resource_capacity; ++i)
        {
            _resource_binding_sets[i].starting_index = 0;
            _resource_binding_sets[i].resources = images;
        }

        _resource_set = kege::Graphics::allocateResourceSet
        ({
            {0, "HeightmapImageArray"},
            {1, "NormalmapImageArray"},
        });

        if ( !kege::Graphics::updateResourceSet( _resource_set, _resource_binding_sets ) )
        {
            return false;
        }



//        graphics::UniformDescriptors resource_descriptors =
//        {{
//            {
//                0, "HeightmapImageArray",
//                _max_shader_resource_capacity, graphicsDESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
//                0, graphicsVERTEX_SHADER | graphicsFRAGMENT_SHADER
//            },
//            {
//                1, "NormalmapImageArray",
//                _max_shader_resource_capacity, graphicsDESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
//                0, graphicsVERTEX_SHADER | graphicsFRAGMENT_SHADER
//            }
//        }};
//        _shader_resource_layout = ShaderResourceLayout::create( resource_descriptors, resources );
        return ( _sampler );
    }

    bool ImageLayerManager::empty()const
    {
        return _resource_binding_sets.empty();
    }

    void ImageLayerManager::purge()
    {
        _freed_layers.clear();
        _image_layer_counters.clear();
        _resource_binding_sets.clear();
    }

    ImageLayerManager::~ImageLayerManager()
    {
        purge();
    }

    ImageLayerManager::ImageLayerManager()
    :   _max_shader_resource_capacity( 4 )
    ,   _current_image_index( 0 )
    ,   _image_layers( 0 )
    ,   _image_height( 0 )
    ,   _image_width( 0 )
    {}
}
