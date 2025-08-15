//
//  landscape.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/21/24.
//

#include "landscape.hpp"

namespace kege{

    bool Landscape::initialize( const kege::LandscapeSettings& settings )
    {
        _settings = settings;

        for ( Layers::iterator itr = _layers.begin(); itr != _layers.end(); ++itr )
        {
            if ( !(*itr)->initialize() )
            {
                return false;
            }
        }
        return true;
    }

    void Landscape::notify( void* sender, int event, void* data )
    {
        stats.total_memory = 0;

        for ( Layers::iterator itr = _layers.begin(); itr != _layers.end(); ++itr )
        {
            if ( sender == *itr) continue;
            (*itr)->notify( event, data );
        }
    }

    void Landscape::render( kege::CommandBuffer& command_buffer )
    {
        for ( Layers::iterator itr = _layers.begin(); itr != _layers.end(); ++itr )
        {
            (*itr)->beginRender( command_buffer );
        }
        for ( Layers::iterator itr = _layers.begin(); itr != _layers.end(); ++itr )
        {
            (*itr)->render( command_buffer );
        }
        for ( Layers::iterator itr = _layers.begin(); itr != _layers.end(); ++itr )
        {
            (*itr)->endRender( command_buffer );
        }
    }

    void Landscape::addLayer( LandscapeLayer* layer )
    {
        _layers.push_back( layer );
        layer->_landscape = this;
    }

    void Landscape::update( const kege::dvec3& eye )
    {
        for ( Layers::iterator itr = _layers.begin(); itr != _layers.end(); ++itr )
        {
            (*itr)->update( eye );
        }
    }

    const kege::LandscapeSettings* Landscape::settings()const
    {
        return &_settings;
    }
    kege::ImageLayerManager* Landscape::imageLayerManager()
    {
        return &_image_layer_manager;
    }
    Landscape::~Landscape()
    {
        for ( Layers::iterator itr = _layers.begin(); itr != _layers.end(); ++itr )
        {
            delete (*itr);
        }
        _layers.clear();
    }


}







