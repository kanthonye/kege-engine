//
//  shader-pipeline.hpp
//  physics
//
//  Created by Kenneth Esdaile on 9/10/25.
//

#ifndef shader_pipeline_hpp
#define shader_pipeline_hpp

#include <map>
#include "uniform-resource.hpp"

namespace kege{

    /**
     * @brief Describes a field within a push constant block.
     */
    struct MemberField
    {
        std::string name;
        uint32_t offset;
        uint32_t size;
        MemberType type;
    };

    /**
     * @brief Describes a push constant range used in a pipeline.
     */
    struct PushConstantInfo
    {
        std::vector< MemberField > fields;
        std::string name;
        uint32_t offset;
        uint32_t size;
        ShaderStage stages;
    };

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
        // Render target information
        std::vector< Format > color_attachment_formats;// color_attachment_formats; ///< Must match render pass
        Format depth_attachment_format = Format::depth_32;
        Format stencil_attachment_format = Format::undefined;
        SampleCount render_sample_count = SampleCount::Count1;
    };

    enum BindingType
    {
        BUFFER, TEXTURE, PUSH_CONSTANTS, SHADER_RESOURCE
    };
    struct PipelineResourceBinding
    {
        std::string name;
        BindingType type;
    };

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

    /**
     * @brief Describes a complete graphics or compute pipeline.
     */
    struct PipelineInfo
    {
        /**
         * @brief Debug name for graphics debugging tools.
         */
        std::string name = "";

        /**
         * @brief Descriptor set layouts used by this pipeline.
         */
        UniformDescriptorSets layouts;

        /**
         * @brief Push constant ranges used by this pipeline.
         */
        std::vector< PushConstantInfo > push_constants;

        VertexInputStateDesc vertex_input;

        PipelineStates states;

        PipelineOutputs outputs;

        std::vector< int > stages;


        std::vector< std::pair<int,int> > specialization_constants;
        std::vector< PipelineResourceBinding > global_resources;
        PipelineSupport support;
    };

    /**
     * @brief Information required to create a shader pipeline.
     */
    struct CreateShaderPipelineInfo
    {
        std::vector< kege::ShaderHandle > stages;
        std::vector< kege::PipelineInfo > pipelines;
    };


}

namespace kege{

    class Graphics;
    class ShaderPipelineManager;

    struct PipelineContext
    {
        /**
         * - name:
         *   A human‑readable label used by debugging and profiling tools to identify
         *   this pipeline configuration. This does not affect rendering behavior but
         *   can greatly aid diagnostics.
         */
        std::string name = "";

        /**
         * - layouts:
         *   The descriptor set layout descriptions required by the shaders bound to
         *   this pipeline. These define the sets, bindings, resource types and
         *   visibility (shader stages) for all uniform/storage resources the pipeline
         *   expects.
         */
        UniformSetsDesc layouts;

        /**
         * - push_constants:
         *   A collection of push constant ranges and their field metadata used by the
         *   pipeline. Push constants provide a low‑latency path for passing small
         *   amounts of data directly to shaders without creating buffers.
         */
        std::vector< PushConstantInfo > push_constants;

        /**
         * - vertex_input:
         *   The vertex input state description detailing the format and rate of vertex
         *   attributes and their buffer bindings. This must match the input interface
         *   declared in the vertex shader stage.
         */
        VertexInputStateDesc vertex_input;

        /**
         * - states:
         *   The fixed‑function pipeline states (input assembly, rasterization,
         *   depth/stencil, color blending, and multisampling). These govern how
         *   primitives are assembled, rasterized, tested, and blended into the
         *   framebuffer.
         */
        PipelineStates states;

        /**
         * - outputs:
         *   The render target configuration, including color attachment formats,
         *   depth/stencil formats, and the render sample count. These must be
         *   compatible with the render pass or framebuffer to which the pipeline will
         *   be bound.*
         */
        PipelineOutputs outputs;

        /**
         * - specialization_constants:
         *   A list of key/value pairs used to specialize shader constants at pipeline
         *   creation time. These allow toggling or configuring shader behavior without
         *   recompiling shader source.
         */
        std::vector< std::pair<int,int> > specialization_constants;


        std::vector< PipelineResourceBinding > global_resources;
        PipelineSupport support;

        kege::PipelineHandle pipeline;
    };

    class ShaderPipeline : public RefCounter
    {
    public:
        /**
         * @brief Enables access to this shader resource handles
         * @return A collection of resource handle associated with this shader resource.
         */
        const PipelineContext* operator ->()const;

        /**
         * @brief Enables access to each uniform set associated with this shader resource
         * @return A reference to the uniform set at the given index
         */
        const PipelineContext& operator[](int i) const;

        /**
         * @brief Enables access to each uniform set associated with this shader resource
         * @return A reference to the uniform set at the given index
         */
        PipelineContext& operator[](int i);

        /**
         * @brief Boolean conversion operator.
         * @return True if the resource is valid, false otherwise.
         */
        operator bool() const;

        PipelineHandle handle()const;
        int32_t id()const;

        /**
         * @brief Copy assignment operator
         */
        ShaderPipeline& operator =( const ShaderPipeline& other );

        /**
         * @brief Move assignment operator
         */
        ShaderPipeline& operator =( ShaderPipeline&& other) noexcept;

        /**
         * @brief Copy constructor
         */
        ShaderPipeline( const ShaderPipeline& other );

        /**
         * @brief Move constructor
         */
        ShaderPipeline( ShaderPipeline&& other )noexcept;

        /**
         * @brief Default constructor creates an invalid/null resource.
         */
        ShaderPipeline();

        /**
         * @brief deconstructor
         */
        ~ShaderPipeline();

    private:

        /**
         * @brief Default constructor creates an invalid/null resource.
         */
        ShaderPipeline(ShaderPipelineManager* manager, int32_t index);

    private:

        friend ShaderPipelineManager;
        ShaderPipelineManager* _manager;
        int32_t _index;
    };



    // Define equality comparison for VkDescriptorSetLayoutBinding
    bool operator==(const kege::ShaderPipeline& a, const kege::ShaderPipeline& b);

    // Define none equality comparison for VkDescriptorSetLayoutBinding
    bool operator!=(const kege::ShaderPipeline& a, const kege::ShaderPipeline& b);

    // Define less-than comparison (needed to resolve the compiler error)
    bool operator<(const kege::ShaderPipeline& a, const kege::ShaderPipeline& b);

}

namespace std{

    template <> struct hash< kege::ShaderPipeline >
    {
        std::size_t operator()( const kege::ShaderPipeline& pipeline ) const
        {
            return static_cast< std::size_t >( pipeline.id() + 1 );
        }
    };

}
#endif /* shader_pipeline_hpp */
