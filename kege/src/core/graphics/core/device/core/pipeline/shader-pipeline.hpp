//
//  shader-pipeline.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/21/25.
//

#ifndef shader_pipeline_hpp
#define shader_pipeline_hpp

#include "../common/pipeline-key.hpp"
#include "shader.hpp"
#include "shader-pipeline-layout.hpp"

namespace kege
{
    class ShaderPipeline;
}

namespace kege::vk
{
    class ShaderPipeline;
}

namespace kege::ref
{
    typedef kege::Ref< kege::ShaderPipeline > ShaderPipeline;
}

namespace kege::cref
{
    typedef kege::Ref< const kege::ShaderPipeline > ShaderPipeline;
}

namespace kege{

    enum struct PipelineType
    {
        Graphics = 1,
        Compute,
        RayTracing,
    };

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

    typedef std::vector< kege::DynamicState > DynamicStates;
    /**
     * @brief Describes a complete graphics or compute pipeline.
     */
    struct ShaderPipelineDesc
    {
        std::string                 name;
        kege::PipelineType          pipeline_type;
        kege::ShaderPipelineLayoutDesc pipeline_layout;

        kege::VertexLayout          vertex_layout;

        kege::PipelineRendering     pipeline_rendering;
        
        kege::InputAssembly         input_assembly;
        kege::DepthStencil          depth_stencil;
        kege::RasterizerState       rasterizer;
        kege::ColorBlendState       color_blend;
        kege::Multisample           multisample;
        kege::DynamicStates         dynamic_states;

        kege::FeaturesBitmask       features;

        std::vector< ref::Shader >  shaders;
    };

}

namespace kege{

    struct GlobalBinding
    {
        uint32_t set;
        uint32_t binding;
        GlobalSemantic semantic;
    };

    class ShaderPipeline : public kege::RefCounter
    {
    public:

        const kege::ShaderSetBindingDesc* getBindingDesc( const kege::ShaderResrcIndexPair& pair )const;
        const std::vector< kege::GlobalBinding >& getGlobalBindings() const;

        const kege::ShaderSetBindingDesc* getBindingDesc( uint32_t set, uint32_t binding ) const;
        const kege::ShaderResourceSetLayout* getShaderResourceSetLayout( uint32_t set )const;
        const kege::ShaderSetDesc* getSetDesc( uint32_t set ) const;

        
        void insertGlobalBinding( uint32_t set, uint32_t binding, GlobalSemantic semantic );
        const std::vector< kege::GlobalBinding >& getGlobalBinding()const;

        kege::SetIndex getBindLocationIndex( kege::UniformKey key )const;

        const kege::ShaderPipelineLayout* getShaderPipelineLayout()const;
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
        //const kege::MeshType getMeshType()const;
        const kege::FeaturesBitmask getFeatures()const;

        //const kege::ref::ShaderLayout& getShaderLayout()const;
        //kege::ref::ShaderLayout getShaderLayout();

        bool checkFeature( kege::FeatureFlag feature )const;
        kege::PipelineType getType()const;
        const std::string& getName()const;

        virtual ~ShaderPipeline(){}
        
    protected:

        ShaderPipeline( const kege::ShaderPipelineDesc& desc, const ref::ShaderPipelineLayout& shader_layout );

    protected:

        kege::ref::ShaderPipelineLayout _shader_pipeline_layout;
        std::vector< kege::GlobalBinding > _global_bindings;

        //kege::ref::ShaderLayout     _shader_layout;
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
        //kege::MeshType          _renderer_type;

        //bool _alpha_to_coverage_enable;  // For foliage transparency
        friend ref::ShaderPipeline;
    };

    kege::PipelineType stringToPipelineType( const std::string& s );
}


namespace kege::ref{
    typedef kege::Ref< kege::ShaderPipeline > ShaderPipeline;
}
#endif /* shader_pipeline_hpp */
