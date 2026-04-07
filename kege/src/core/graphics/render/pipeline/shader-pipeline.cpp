//
//  shader-pipeline.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/21/25.
//

#include "shader-pipeline.hpp"

namespace kege{

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

//    const kege::ShadingModel ShaderPipeline::getShadingModel()const
//    {
//        return _shading_model;
//    }
//
//    const kege::MeshType ShaderPipeline::getMeshType()const
//    {
//        return _renderer_type;
//    }

    const kege::FeaturesBitmask ShaderPipeline::getFeatures()const
    {
        return _features;
    }

    const kege::ref::ShaderLayout& ShaderPipeline::getShaderLayout()const
    {
        return _shader_layout;
    }

    kege::ref::ShaderLayout ShaderPipeline::getShaderLayout()
    {
        return _shader_layout;
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

    ShaderPipeline::ShaderPipeline( const kege::ShaderPipelineDesc& info, const ref::ShaderLayout& shader_layout )
    :   _shader_layout( shader_layout )
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
}
