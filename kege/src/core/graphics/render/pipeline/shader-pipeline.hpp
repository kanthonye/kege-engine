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

namespace kege{

    /**
     * @brief Describes the output configuration of a graphics pipeline.
     */
    struct PipelineOutputs
    {
        std::vector< Format > color_attachment_formats;
        Format depth_attachment_format = Format::depth_32;
        Format stencil_attachment_format = Format::undefined;
        SampleCount render_sample_count = SampleCount::Count1;
    };

    enum struct PipelineType
    {
        Graphics = 1,
        Compute,
        RayTracing,
    };

}

namespace kege{

    struct PipelineSupport
    {
        enum Flag{ Camera, Lighting, Material, ObjectPushConst };
        constexpr friend PipelineSupport& operator <<( PipelineSupport& flags, uint32_t mask )
        {
            flags.state = flags.state | mask;
            return flags;
        }
        constexpr friend PipelineSupport operator |=( PipelineSupport& flags, uint32_t mask )
        {
            uint32_t s = flags.state | mask;
            return PipelineSupport{s};
        }
        constexpr friend PipelineSupport operator &=( PipelineSupport& flags, uint32_t mask )
        {
            uint32_t s = flags.state & mask;
            return PipelineSupport{s};
        }
        constexpr bool operator[]( uint32_t mask )const{ return (state & mask) == mask; }
        uint32_t state;
    };

    struct PipelineLayoutInfo
    {
        std::string name;
        kege::SetsConfigLayout layouts;
        kege::PushBlockLayout push_constants;
    };

    /**
     * @brief Describes a complete graphics or compute pipeline.
     */
    struct PipelineCreateInfo
    {
        std::string name = "";
        kege::PipelineType pipeline_type;
        kege::ShaderLayoutDesc shader_layout;
        kege::VertexBufferLayout vertex_input;
        kege::InputAssemblyStateDesc input_assembly;
        kege::RasterizationStateDesc rasterization;
        kege::DepthStencilStateDesc depth_stencil;
        kege::ColorBlendStateDesc color_blend;
        kege::MultisampleStateDesc multisample;
        kege::PipelineOutputs outputs;
        kege::PipelineSupport support;

        std::vector< std::string > global_resources;
        std::vector< std::pair<int,int> > specialization_constants;

        std::vector< ref::Shader > shaders;
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

        const kege::InputAssemblyStateDesc& getInputAssemblyState()const;
        const kege::RasterizationStateDesc& getRasterizationState()const;
        const kege::DepthStencilStateDesc& getDepthStencilStateDesc()const;
        const kege::ColorBlendStateDesc& getColorBlendStateDesc()const;;
        const kege::MultisampleStateDesc& getMultisampleStateDesc()const;
        const kege::VertexBufferLayout& getVertexBufferLayout()const;
        const kege::PipelineOutputs& getPipelineOutputs()const;
        const kege::ShaderLayout* getShaderLayout()const;
        const std::vector< std::string > getGlobalBinds()const;
        const std::string& getName()const;

        kege::PipelineType getPipelineType()const;
        bool checkSupport( uint32_t mask )const;

        virtual ~ShaderPipeline(){}
        
    protected:

        ShaderPipeline( const kege::PipelineCreateInfo& info, const ref::ShaderLayout& shader_layout );

    protected:

        kege::ref::ShaderLayout _shader_layout;
        kege::VertexBufferLayout _vertex_input;
        kege::InputAssemblyStateDesc _input_assembly;
        kege::RasterizationStateDesc _rasterization;
        kege::DepthStencilStateDesc _depth_stencil;
        kege::ColorBlendStateDesc _color_blend;
        kege::MultisampleStateDesc _multisample;
        kege::PipelineOutputs _outputs;
        kege::PipelineSupport _support;

        std::vector< std::string > _global_resources;
        std::vector< std::pair<int,int> > _specialization_constants;

        kege::PipelineType _pipeline_type;
        std::string _name;

        friend ref::ShaderPipeline;
    };
    
}


namespace kege::ref{
    typedef kege::Ref< kege::ShaderPipeline > ShaderPipeline;
}
#endif /* shader_pipeline_hpp */
