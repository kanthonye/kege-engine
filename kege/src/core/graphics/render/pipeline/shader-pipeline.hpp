//
//  shader-pipeline.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/21/25.
//

#ifndef shader_pipeline_hpp
#define shader_pipeline_hpp

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

namespace kege{

    class ShaderPipeline : public kege::RefCounter
    {
    public:

        //virtual bool bindSets( const std::vector< kege::ShaderResrcBindingSet >& sets ) = 0;
        //virtual bool bindSet( const kege::ShaderResrcBindingSet& set ) = 0;

        virtual const vk::ShaderPipeline* vk() const { return nullptr; }
        virtual vk::ShaderPipeline* vk() { return nullptr; }

        const kege::PipelineRendering& getPipelineRendering()const;
        const kege::VertexLayout& getVertexLayout()const;

        const kege::DepthStencil& getDepthStencil()const;
        const kege::RasterizerState& getRasterizerState()const;
        const kege::ColorBlendState& getColorBlendState()const;

        // Additional render states
        const kege::PrimitiveTopology getTopology()const; // TriangleList, TriangleStrip, LineList, etc.

        //const kege::ShadingModel getShadingModel()const;
        //const kege::RendererType getRendererType()const;
        const kege::FeaturesBitmask getFeatures()const;

        const kege::ref::ShaderLayout& getShaderLayout()const;
        kege::ref::ShaderLayout getShaderLayout();

        bool checkFeature( kege::FeatureFlag feature )const;
        kege::PipelineType getType()const;
        const std::string& getName()const;

        virtual ~ShaderPipeline(){}
        
    protected:

        ShaderPipeline( const kege::ShaderPipelineDesc& desc, const ref::ShaderLayout& shader_layout );

    protected:

        kege::ref::ShaderLayout     _shader_layout;
        kege::VertexLayout          _vertex_layout;
        kege::PipelineType          _type;
        std::string                 _name;

        kege::PipelineRendering     _pipeline_rendering;

        // Fixed-function states (CRITICAL for pipeline caching!)
        kege::DepthStencil          _depth_state;
        kege::RasterizerState       _raster_state;
        kege::ColorBlendState       _color_blend_state;
        kege::DynamicState          _dynamic_states;

        // Additional render states
        kege::PrimitiveTopology     _topology; // TriangleList, TriangleStrip, LineList, etc.

        kege::FeaturesBitmask       _features;

        // Shader selection
        //kege::ShadingModel          _shading_model;
        //kege::RendererType          _renderer_type;

        //bool _alpha_to_coverage_enable;  // For foliage transparency
        friend ref::ShaderPipeline;
    };
    
}


namespace kege::ref{
    typedef kege::Ref< kege::ShaderPipeline > ShaderPipeline;
}
#endif /* shader_pipeline_hpp */
