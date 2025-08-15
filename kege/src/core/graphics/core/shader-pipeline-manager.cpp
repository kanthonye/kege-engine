//
//  shader-pipeline-manager.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/22/25.
//

#include "../../graphics/core/graphics.hpp"
#include "../../graphics/core/shader-pipeline-manager.hpp"

namespace kege{

    kege::PipelineHandle ShaderPipelineManager::load( const std::string& name_id, const std::string& filename )
    {
        if ( _graphics == nullptr )
        {
            return {-1};
        }

        auto itr = _name_index_map.find( name_id );
        if ( itr != _name_index_map.end() )
        {
            return _pipelines[ itr->second ];
        }

        kege::PipelineHandle pipeline = _graphics->loadGraphicsPipeline( filename );
        set( name_id, pipeline );
        return pipeline;
    }
    kege::PipelineHandle ShaderPipelineManager::load( const std::string& filename )
    {
        if ( _graphics == nullptr )
        {
            return {-1};
        }

        kege::Json json = kege::JsonParser::load( filename.data() );
        if ( !json )
        {
            KEGE_LOG_ERROR <<"fail to open file -> " << filename;
            return {};
        }
        std::string path = kege::getFilePath( filename );
        std::string name_id = json[ "name" ].value();
        auto itr = _name_index_map.find( name_id );
        if ( itr != _name_index_map.end() )
        {
            return _pipelines[ itr->second ];
        }

        kege::PipelineHandle pipeline = _graphics->loadGraphicsPipeline( filename, &name_id );
        set( name_id, pipeline );
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
