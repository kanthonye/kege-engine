//
//  shader-pipeline.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/21/25.
//

#include "shader-pipeline.hpp"

namespace kege{

    const kege::ShaderSetBindingDesc* ShaderPipeline::getBindingDesc( const kege::ShaderResrcIndexPair& pair )const
    {
        return _shader_pipeline_layout->getBindingDesc( pair.set, pair.binding );
    }

    const std::vector< kege::GlobalBinding >& ShaderPipeline::getGlobalBindings() const
    {
        return _global_bindings;
    }

    const kege::ShaderSetBindingDesc* ShaderPipeline::getBindingDesc( uint32_t set, uint32_t binding ) const
    {
        return _shader_pipeline_layout->getBindingDesc( set, binding );
    }

    const kege::ShaderResourceSetLayout* ShaderPipeline::getShaderResourceSetLayout( uint32_t set )const
    {
        return _shader_pipeline_layout->getShaderResourceSetLayout( set );
    }


    const kege::ShaderSetDesc* ShaderPipeline::getSetDesc( uint32_t set )const
    {
        return &_shader_pipeline_layout->getShaderResourceSetLayout( set )->getDesc();
    }

    void ShaderPipeline::insertGlobalBinding( uint32_t set, uint32_t binding, GlobalSemantic semantic )
    {
        if (_shader_pipeline_layout->getBindingDesc( set, binding ) == nullptr)
        {
            return;
        }
        for (const kege::GlobalBinding& gb : _global_bindings)
        {
            if (gb.set == set && gb.binding == binding && gb.semantic == semantic )
            {
                return;
            }
        }
        _global_bindings.push_back({ set, binding, semantic });
    }

    const std::vector< kege::GlobalBinding >& ShaderPipeline::getGlobalBinding() const
    {
        return _global_bindings;
    }

    kege::SetIndex ShaderPipeline::getBindLocationIndex( kege::UniformKey key )const
    {
        return _shader_pipeline_layout->getBindLocationIndex( key );
    }

    const kege::ShaderPipelineLayout* ShaderPipeline::getShaderPipelineLayout()const
    {
        return _shader_pipeline_layout.ref();
    }

    const kege::PipelineRendering& ShaderPipeline::getPipelineRendering()const
    {
        return _pipeline_rendering;
    }
    
    const kege::VertexLayout& ShaderPipeline::getVertexLayout()const
    {
        return _vertex_layout;
    }

    const kege::DepthStencil& ShaderPipeline::getDepthStencil()const
    {
        return _depth_state;
    }

    const kege::RasterizerState& ShaderPipeline::getRasterizerState()const
    {
        return _raster_state;
    }

    const kege::ColorBlendState& ShaderPipeline::getColorBlendState()const
    {
        return _color_blend_state;
    }

    // Additional render states
    const kege::PrimitiveTopology ShaderPipeline::getTopology()const
    {
        return _topology;
    }

    const kege::FeaturesBitmask ShaderPipeline::getFeatures()const
    {
        return _features;
    }

    bool ShaderPipeline::checkFeature( kege::FeatureFlag feature )const
    {
        return _features & feature;
    }

    kege::PipelineType ShaderPipeline::getType()const
    {
        return _type;
    }

    const std::string& ShaderPipeline::getName()const
    {
        return _name;
    }

    ShaderPipeline::ShaderPipeline
    (
        const kege::ShaderPipelineDesc& info,
        const ref::ShaderPipelineLayout& shader_layout
    )
    :   _shader_pipeline_layout( shader_layout )
    ,   _vertex_layout( info.vertex_layout )
    ,   _name( info.name )
    ,   _type( info.pipeline_type )
    ,   _pipeline_rendering( info.pipeline_rendering )
    ,   _depth_state( info.depth_stencil )
    ,   _raster_state( info.rasterizer )
    ,   _color_blend_state( info.color_blend )
    ,   _topology( info.input_assembly.topology )
    ,   _features( info.features )
//    ,   _shading_model( info.shading_model )
//    ,   _renderer_type( info.renderer_type )
//    ,   _alpha_to_coverage_enable( info.alpha_to_coverage_enable )
    {}


    kege::PipelineType stringToPipelineType( const std::string& name )
    {
        static std::map< std::string, kege::PipelineType > types;

        if ( types.empty() )
        {
            types[ "Graphics" ] = kege::PipelineType::Graphics;
            types[ "Compute" ] = kege::PipelineType::Compute;
            types[ "RayTracing" ] = kege::PipelineType::RayTracing;

            types[ "GRAPHICS" ] = kege::PipelineType::Graphics;
            types[ "COMPUTE" ] = kege::PipelineType::Compute;
            types[ "RAY_TRACING" ] = kege::PipelineType::RayTracing;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }
        kege::Log::error << "unsupported PipelineType -> " <<name <<kege::Log::nl;
        return kege::PipelineType::Graphics;
    }

}
