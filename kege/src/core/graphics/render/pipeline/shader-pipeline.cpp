//
//  shader-pipeline.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/21/25.
//

#include "shader-pipeline.hpp"

namespace kege{


    const kege::InputAssemblyStateDesc& ShaderPipeline::getInputAssemblyState()const
    {
        return _input_assembly;
    }
    const kege::RasterizationStateDesc& ShaderPipeline::getRasterizationState()const
    {
        return _rasterization;
    }
    const kege::DepthStencilStateDesc& ShaderPipeline::getDepthStencilStateDesc()const
    {
        return _depth_stencil;
    }
    const kege::ColorBlendStateDesc& ShaderPipeline::getColorBlendStateDesc()const
    {
        return _color_blend;
    }
    const kege::MultisampleStateDesc& ShaderPipeline::getMultisampleStateDesc()const
    {
        return _multisample;
    }
    const kege::VertexBufferLayout& ShaderPipeline::getVertexBufferLayout()const
    {
        return _vertex_input;
    }
    const kege::PipelineOutputs& ShaderPipeline::getPipelineOutputs()const
    {
        return _outputs;
    }
    const kege::ShaderLayout* ShaderPipeline::getShaderLayout()const
    {
        return _shader_layout.ref();
    }
    const std::vector< std::string > ShaderPipeline::getGlobalBinds()const
    {
        return _global_resources;
    }
    const std::string& ShaderPipeline::getName()const
    {
        return _name;
    }

    kege::PipelineType ShaderPipeline::getPipelineType()const
    {
        return _pipeline_type;
    }

    bool ShaderPipeline::checkSupport( uint32_t mask )const
    {
        return _support[ mask ];
    }
    
    ShaderPipeline::ShaderPipeline( const kege::PipelineCreateInfo& info, const ref::ShaderLayout& shader_layout )
    :   _shader_layout( shader_layout )
    ,   _name( info.name )
    ,   _vertex_input( info.vertex_input )
    ,   _input_assembly( info.input_assembly )
    ,   _rasterization( info.rasterization )
    ,   _depth_stencil( info.depth_stencil )
    ,   _color_blend( info.color_blend )
    ,   _multisample( info.multisample )
    ,   _outputs( info.outputs )
    ,   _support( info.support )
    ,   _global_resources( info.global_resources )
    ,   _specialization_constants( info.specialization_constants )
    ,   _pipeline_type( info.pipeline_type )
    {}
}
