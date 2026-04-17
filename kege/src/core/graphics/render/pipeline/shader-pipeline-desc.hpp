//
//  shader-pipeline.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/21/25.
//

#ifndef shader_pipeline_desc_hpp
#define shader_pipeline_desc_hpp

#include "shader.hpp"
#include "shader-data.hpp"
#include "pipeline-key.hpp"

namespace kege{

    /**
     * @brief Describes the output configuration of a graphics pipeline.
     */
    struct PipelineRendering
    {
        std::vector< Format > color_attachment_formats;         // → pColorAttachmentFormats
        Format depth_attachment_format = Format::depth_32;      // → depthAttachmentFormat
        Format stencil_attachment_format = Format::undefined;   // → stencilAttachmentFormat
        SampleCount sample_count = SampleCount::Count1;  // → rasterizationSamples
    };

}

namespace kege{

    struct ShaderSource
    {
        std::string source;
        std::string entry;
        std::string name;
        ShaderStageFlag stage = ShaderStageFlag::Vertex;
    };
    typedef std::vector< ShaderSource > ShaderSources;

    
//    struct PipelineSupport
//    {
//        enum Flag{ Camera, Lighting, Material, ObjectPushConst };
//        constexpr friend PipelineSupport& operator <<( PipelineSupport& flags, uint32_t mask )
//        {
//            flags.state = flags.state | mask;
//            return flags;
//        }
//        constexpr friend PipelineSupport operator |=( PipelineSupport& flags, uint32_t mask )
//        {
//            uint32_t s = flags.state | mask;
//            return PipelineSupport{s};
//        }
//        constexpr friend PipelineSupport operator &=( PipelineSupport& flags, uint32_t mask )
//        {
//            uint32_t s = flags.state & mask;
//            return PipelineSupport{s};
//        }
//        constexpr bool operator[]( uint32_t mask )const{ return (state & mask) == mask; }
//        uint32_t state;
//    };

    struct PipelineLayoutInfo
    {
        std::string name;
        kege::BindSetDescs layouts;
        kege::PushBlockDescs push_constants;
    };

    /**
     * @brief Describes a complete graphics or compute pipeline.
     */
    struct ShaderPipelineDesc
    {
        std::string                 name;
        kege::PipelineType          pipeline_type;
        kege::ShaderLayoutDesc      shader_layout;

        kege::VertexLayout          vertex_layout;

        kege::PipelineRendering     pipeline_rendering;
        // Fixed-function states (CRITICAL for pipeline caching!)
        kege::InputAssembly         input_assembly;
        kege::DepthStencil          depth_stencil;
        kege::RasterizerState       rasterizer;
        kege::ColorBlendState       color_blend;
        kege::Multisample           multisample;
        kege::DynamicState          dynamic_states;

        kege::FeaturesBitmask       features;

        std::vector< ref::Shader >  shaders;
    };

}
#endif /* shader_pipeline_hpp */
