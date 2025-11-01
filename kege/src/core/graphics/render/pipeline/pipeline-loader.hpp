//
//  pipeline-loader.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/5/25.
//

#ifndef pipeline_loader_hpp
#define pipeline_loader_hpp

#include "../../render/core/graphics.hpp"
#include "../../render/core/string-to-enum-types.hpp"
#include "../../../utils/json-parser.hpp"
#include "shader-pipeline.hpp"

namespace kege{

    struct PipelinInfo
    {
        struct Output
        {
            std::string name;
            std::string semantic;
            Format format;
            int location;
        };

        struct Layout
        {
            int set;
            int binding;
            int count;
            int uniform;
            std::string name;
            kege::BindingUsage usage;
            kege::ShaderStageFlag stages;
        };

        struct Push
        {
            int size;
            int count;
            int offset;
            int uniform;
            std::string name;
            kege::ShaderStageFlag stages;
        };

        std::string name;
        int rasterizer;
        int depth_stencil;
        int color_blend;
        int vertex_layout;
        std::vector< Output > outputs;
        std::vector< int > stages;
        kege::PrimitiveTopology topology;
        kege::PipelineType type;

        std::vector< Push > push_constants;
        std::map< int, std::vector< Layout > > sets;
    };

    struct ShaderPipelineLibContext
    {
        std::vector< ref::ShaderStructBlock > uniforms;
        std::vector< RasterizationStateDesc > rasterizer_states;
        std::vector< DepthStencilStateDesc > depth_stencil_states;
        std::vector< ColorBlendStateDesc > color_blend_states;
        std::vector< VertexBufferLayout > vertex_layouts;
        std::vector< ref::Shader > shaders;

    };

    bool parseShaderPipelineLib
    (
        const std::string& path,
        kege::Graphics* graphics,
        kege::Json& shader_pipeline_library,
        ShaderPipelineLibContext* context,
        std::vector< PipelinInfo >* pipelines
    );

    ref::ShaderPipeline createShaderPipeline
    (
        kege::Graphics* graphics, ShaderPipelineLibContext& context, const PipelinInfo& info
    );
}
#endif /* pipeline_loader_hpp */
