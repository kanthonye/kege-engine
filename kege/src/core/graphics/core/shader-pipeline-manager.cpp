//
//  shader-pipeline-manager.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/22/25.
//

#include "../../graphics/core/graphics.hpp"
#include "../../graphics/core/shader-pipeline-manager.hpp"

namespace kege{

    PipelineHandle ShaderPipelineManager::load( const std::string& name, const std::string& filename )
    {
        PipelineHandle pipeline = PipelineLoader::load( _graphics, filename.c_str() );
        if( !pipeline )
        {
            return {};
        }
        this->set( name, pipeline );
        return pipeline;
    }

    void ShaderPipelineManager::set( const std::string& name_id, kege::PipelineHandle pipeline )
    {
        if ( !pipeline )
        {
            return;
        }
        _name_index_map[ name_id ] = static_cast< int >( _pipelines.size() );
        _pipelines.push_back( pipeline );
    }

    kege::PipelineHandle ShaderPipelineManager::get( const std::string& name_id )
    {
        auto itr = _name_index_map.find( name_id );
        if ( itr != _name_index_map.end() )
        {
            return _pipelines[ itr->second ];
        }
        return {-1};
    }
    kege::PipelineHandle ShaderPipelineManager::get( int index_id )
    {
        return _pipelines[ index_id ];
    }

    int ShaderPipelineManager::getIndexOf( const std::string& name_id )
    {
        auto itr = _name_index_map.find( name_id );
        if ( itr != _name_index_map.end() )
        {
            return itr->second;
        }
        return -1;
    }
    void ShaderPipelineManager::initalize( kege::Graphics* graphics )
    {
        _graphics = graphics;
    }

    void ShaderPipelineManager::shutdown()
    {
        if ( _graphics )
        {
            for (int i=0; i<_pipelines.size(); ++i)
            {
                _graphics->destroyGraphicsPipeline( _pipelines[i] );
            }
            _pipelines.clear();
            _graphics = nullptr;
        }
    }

    ShaderPipelineManager::ShaderPipelineManager()
    :   _graphics( nullptr )
    {}

}
