//
//  shader-pipeline.hpp
//  physics
//
//  Created by Kenneth Esdaile on 9/10/25.
//

#ifndef shader_pipeline_hpp
#define shader_pipeline_hpp

#include "graphics-core.hpp"

namespace kege{

    class Graphics;

    enum ShaderRequirement
    {
        NO_REQUIREMENTS = 0,
        PUSH_CONSTANTS  = 1 << 0,
        PBR_MATERIAL    = 1 << 1,
        LIGHTS          = 1 << 2,
        SHADOW_MAP      = 1 << 3,
        ENVIRONMENT_MAP = 1 << 4,
        CAMERA          = 1 << 5,
        MESH_TRANSFORM  = 1 << 6,
        SKELETON        = 1 << 7,
        VERTEX_BUFFERS  = 1 << 8,
        INDEX_BUFFER    = 1 << 9,
        FULL_SCREEN_QUAD = 1 << 10
    };

    class ShaderPipeline : public RefCounter
    {
    public:

        struct States
        {
            // Fixed-function states
            VertexInputStateDesc    vertex_input_state;
            InputAssemblyStateDesc  input_assembly_state;
            RasterizationStateDesc  rasterization_state;
            DepthStencilStateDesc   depth_stencil_state;
            ColorBlendStateDesc     color_blend_state;
            MultisampleStateDesc    multisample_state;

            // Render target information
            std::vector<Format> color_attachment_formats; ///< Must match render pass
            Format depth_attachment_format;
            Format stencil_attachment_format;
            SampleCount render_sample_count;
        };

        struct Layout
        {
            //std::vector< UniformResourceLayoutInfos > descriptors;
            std::vector< PushConstantRange > constants;
        };

    public:

        bool require( ShaderRequirement requirement ) const
        {
            return (_requirements & requirement) != 0;
        }

        void unsetRequirement( ShaderRequirement requirement )
        {
            _requirements &= ~requirement;
        }
        void setRequirement( ShaderRequirement requirement )
        {
            _requirements |= requirement;
        }

        const ShaderPipeline::Layout& getLayout()const;
        const ShaderPipeline::States states()const;
        const kege::PipelineHandle& handle()const;

        ~ShaderPipeline();
        ShaderPipeline();

    private:

        ShaderPipeline::Layout _layout;
        ShaderPipeline::States _states;
        kege::PipelineHandle _pipeline;
        kege::Graphics* _graphics;
        uint32_t _requirements;
    };

}

#endif /* shader_pipeline_hpp */
