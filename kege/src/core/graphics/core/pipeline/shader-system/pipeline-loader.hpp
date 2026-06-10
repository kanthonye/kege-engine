//
//  pipeline-loader.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/5/25.
//

#ifndef pipeline_loader_hpp
#define pipeline_loader_hpp

#include "../../../../utils/json-parser.hpp"
#include "../../../../graphics/core/device/core/device.hpp"

namespace kege::glsl{

    struct Output
    {
        std::string name;
        std::string semantic;
        Format format;
        int location;
    };

    struct Binding
    {
        int binding;
        int count;
        int block_ref;
        std::string name;
        kege::BindingUsage usage;
        kege::ShaderStageFlag stages;
    };

    struct SetLayout
    {
        int set;
        std::vector< Binding > bindings;
    };

    struct PushLayout
    {
        int size;
        int count;
        int offset;
        int block_ref;
        std::string name;
        kege::ShaderStageFlag stages;
    };

    struct PipelineLayout
    {
        std::string name;
        std::vector< SetLayout > set_layouts;
        std::vector< PushLayout > push_layouts;
    };

    struct Pipeline
    {
        std::string name;
        int rasterizer = -1;
        int depth_stencil = -1;
        int color_blend = -1;
        int vertex_layout = -1;
        int pipeline_layout_ref = -1;
        std::vector< Output > outputs;
        std::vector< int > stages;
        kege::PrimitiveTopology topology;
        kege::PipelineType type;
    };

    struct LibraryContext
    {
        std::vector< glsl::PipelineLayout > pipeline_layouts;
        std::vector< glsl::Pipeline > pipelines;

        std::vector< RasterizerState > rasterizer_states;
        std::vector< DepthStencil > depth_stencil_states;
        std::vector< ColorBlendState > color_blend_states;
        std::vector< VertexLayout > vertex_layouts;
        std::vector< Ref< kege::StructBlockDesc > > struct_blocks;
        std::vector< ref::Shader > shaders;
    };
}

namespace kege{

    bool parseShaderPipelineLib
    (
        kege::Json& json,
        const std::string& path,
        kege::GraphicsDevice* graphics,
        glsl::LibraryContext* context
    );

    ref::ShaderPipeline createShaderPipeline
    (
        kege::GraphicsDevice* graphics,
        glsl::LibraryContext& context,
        int pipeline_index
    );
    bool loadTextFile( std::vector< char >& source, const char* filename );
}
#endif /* pipeline_loader_hpp */
