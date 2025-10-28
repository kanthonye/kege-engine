//
//  shader-pipeline-manager.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/22/25.
//

#include "graphics.hpp"
#include "shader-pipeline-manager.hpp"
#include "pipeline-loader.hpp"

namespace kege{

    kege::ShaderPipeline ShaderPipelineManager::load( const std::string& name, const std::string& filename )
    {
        auto itr = _name_index_map.find( name );
        if ( itr != _name_index_map.end() )
        {
            return kege::ShaderPipeline( this, itr->second );
        }

        kege::Json json = kege::JsonParser::load( filename.data() );
        if ( !json )
        {
            kege::Log::error <<"fail to open file -> " << filename << Log::nl;
            return {};
        }

        kege::CreateShaderPipelineInfo info;
        std::string path = getFilePath( filename );
        if( !parseShaderPipelineInfo( _graphics, &info, json, path ) )
        {
            return {};
        }
        std::vector< kege::PipelineHandle > pipelines = _graphics->createGraphicsPipeline( info );
        return kege::ShaderPipeline( this, set( name, info, pipelines ) );
    }

    kege::ShaderPipeline ShaderPipelineManager::load( const std::string& filename )
    {
        kege::Json json = kege::JsonParser::load( filename.data() );
        if ( !json )
        {
            kege::Log::error <<"fail to open file -> " << filename << Log::nl;
            return {};
        }

        Json asset = json[ "asset" ];
        if ( !asset )
        {
            kege::Log::error <<"missing asset json block -> " << filename << Log::nl;
            return {};
        }

        const std::string name = asset[ "name" ].toStr();
        auto itr = _name_index_map.find( name );
        if ( itr != _name_index_map.end() )
        {
            return kege::ShaderPipeline( this, itr->second );
        }

        std::string path = getFilePath( filename );

        kege::CreateShaderPipelineInfo info;
        if( !parseShaderPipelineInfo( _graphics, &info, json, path ) )
        {
            return {};
        }
        std::vector< kege::PipelineHandle > pipelines = _graphics->createGraphicsPipeline( info );
        if ( pipelines.empty() )
        {
            return {};
        }
        return kege::ShaderPipeline( this, set( name, info, pipelines ) );
    }

    int ShaderPipelineManager::set( const std::string& name_id, const kege::CreateShaderPipelineInfo& info, const std::vector< kege::PipelineHandle >& pipelines )
    {
        int index = int(_pipeline_objects.size());
        _pipeline_objects.push_back({});


        _name_index_map[ name_id ] = index;
        _pipeline_objects[ index ].duplicates = 0;
        _pipeline_objects[ index ].pipeline_contexts.resize( pipelines.size() );
        for (int i=0; i<_pipeline_objects[ index ].pipeline_contexts.size(); ++i)
        {
            PipelineContext& context = _pipeline_objects[ index ].pipeline_contexts[i];
            context.pipeline = pipelines[ i ];
            context.name = info.pipelines[ i ].name;
            //context.layouts = info.pipelines[ i ].layouts;
            context.push_constants = info.pipelines[ i ].push_constants;
            context.states = info.pipelines[ i ].states;
            context.vertex_input = info.pipelines[ i ].vertex_input;
            context.specialization_constants = info.pipelines[ i ].specialization_constants;
            context.global_resources = info.pipelines[ i ].global_resources;
            context.outputs = info.pipelines[ i ].outputs;
        }
        return index;
    }

    kege::ShaderPipeline ShaderPipelineManager::get( const std::string& name_id )
    {
        auto itr = _name_index_map.find( name_id );
        if ( itr != _name_index_map.end() )
        {
            return kege::ShaderPipeline( this, itr->second );
        }
        return {};
    }
    
    kege::ShaderPipeline ShaderPipelineManager::get( int index_id )
    {
        if ( index_id >= _pipeline_objects.size() )
        {
            return kege::ShaderPipeline( this, index_id );
        }
        return {};
    }

    void ShaderPipelineManager::destroy( const kege::ShaderPipeline& shader )
    {}
    
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
            for (int i=0; i<_pipeline_objects.size(); ++i)
            {
                for (int k=0; k<_pipeline_objects[i].pipeline_contexts.size(); ++k)
                {
                    _graphics->destroyGraphicsPipeline( _pipeline_objects[i].pipeline_contexts[k].pipeline );
                }
            }
            _pipeline_objects.clear();
            _graphics = nullptr;
        }
    }

    ShaderPipelineManager::ShaderPipelineManager()
    :   _graphics( nullptr )
    {}

}
