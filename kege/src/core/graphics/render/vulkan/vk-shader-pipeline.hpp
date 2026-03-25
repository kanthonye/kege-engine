//
//  shader-pipeline.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/28/25.
//

#ifndef vk_shader_pipeline_hpp
#define vk_shader_pipeline_hpp

#include "vk-shader.hpp"

namespace kege::vk{

    /**
     * @brief Wrapper for Vulkan graphics pipeline resources
     *
     * Encapsulates a VkPipeline configured for graphics operations.
     * Could be extended to support pipeline caching.
     */
    class ShaderPipeline final : public kege::ShaderPipeline
    {
    public:

        //virtual bool bindSets( const std::vector< kege::ShaderSet >& sets ){return false;}
        //virtual bool bindSet( const kege::ShaderSet& set ){return false;}

        virtual const vk::ShaderPipeline* vk() const { return this; }
        virtual vk::ShaderPipeline* vk() { return this; }

        const VkPipeline handle() const { return _handle; }

        virtual ~ShaderPipeline(){}

    private:

        ShaderPipeline
        (
            vk::Device* device,
            const kege::ShaderPipelineDesc& info,
            const ref::ShaderLayout& shader_layout
        );
        void createGraphicsPipeline( const kege::ShaderPipelineDesc& desc, const ref::ShaderLayout& shader_layout );
        void createComputePipeline( const kege::ShaderPipelineDesc& desc, const ref::ShaderLayout& shader_layout );

    private:

        VkPipelineBindPoint _bind_point;

        /** @brief Native Vulkan pipeline handle */
        VkPipeline _handle;

        vk::Device* _device;

    private:

        friend List< vk::ShaderPipeline >;
        vk::ShaderPipeline* prev;
        vk::ShaderPipeline* next;
        
        friend vk::Device;
    };

}
#endif // vk_shader_pipeline_hpp
