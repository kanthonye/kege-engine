//
//  shader-pipeline.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/21/25.
//

#ifndef shader_pipeline_hpp
#define shader_pipeline_hpp

#include "shader-layout.hpp"
#include "graphics-core.hpp"
#include "shader-reflect.hpp"

namespace kege{

    /**
     * @brief Describes the fixed-function states of a graphics pipeline.
     */
    struct PipelineStates
    {
        // Fixed-function states
        InputAssemblyStateDesc  input_assembly;
        RasterizationStateDesc  rasterization;
        DepthStencilStateDesc   depth_stencil;
        ColorBlendStateDesc     color_blend;
        MultisampleStateDesc    multisample;
    };

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

    class ShaderPipeline
    {
    public:

        virtual bool bindSets( const std::vector< kege::ShaderResrcBindingSet >& sets ) = 0;
        virtual bool bindSet( const kege::ShaderResrcBindingSet& set ) = 0;

        virtual const vk::ShaderPipeline* vk() const { return nullptr; }
        virtual vk::ShaderPipeline* vk() { return nullptr; }

        const kege::VertexBufferLayout& getVertexBufferLayout()const;
        const kege::PipelineOutputs& getPipelineOutputs()const;
        const kege::PipelineStates& getPipelineStates()const;
        const kege::ShaderLayout* getShaderLayout()const;

        kege::PipelineType getPipelineType()const;

    protected:

        ShaderPipeline( kege::PipelineType pipeline_type, kege::Ref< const kege::ShaderLayout >& shader_layout );
        virtual ~ShaderPipeline(){}

    protected:

        kege::Ref< const kege::ShaderLayout > _shader_layout;

        /**
         * - vertex_input:
         *   The vertex input state description detailing the format and rate of vertex
         *   attributes and their buffer bindings. This must match the input interface
         *   declared in the vertex shader stage.
         */
        kege::VertexBufferLayout _vertex_bufer_layout_info;

        /**
         * - states:
         *   The fixed‑function pipeline states (input assembly, rasterization,
         *   depth/stencil, color blending, and multisampling). These govern how
         *   primitives are assembled, rasterized, tested, and blended into the
         *   framebuffer.
         */
        kege::PipelineStates _states;

        /**
         * - outputs:
         *   The render target configuration, including color attachment formats,
         *   depth/stencil formats, and the render sample count. These must be
         *   compatible with the render pass or framebuffer to which the pipeline will
         *   be bound.*
         */
        kege::PipelineOutputs _outputs;

        /**
         * - name:
         *   A human‑readable label used by debugging and profiling tools to identify
         *   this pipeline configuration. This does not affect rendering behavior but
         *   can greatly aid diagnostics.
         */
        std::string _name;

        kege::PipelineType _pipeline_type;
    };
    
}
#endif /* shader_pipeline_hpp */
