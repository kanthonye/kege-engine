//
//  render-stage.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/20/25.
//

#ifndef render_stage_hpp
#define render_stage_hpp

#include "render-pass-context.hpp"

namespace kege{
    
    class RenderExecutor;

    /**
     * @struct RenderStage
     * @brief Manages the execution of a single rendering stage within a rendering pipeline.
     *
     * The RenderStage class is responsible for orchestrating the commands required to perform
     * a specific rendering operation. This includes beginning the command buffer, configuring
     * the render pass with its render-target, and initiating the draw calls. It acts as
     * an intermediary between the render graph and the actual rendering commands.
     */
    class RenderStage
    {
    public:

        const kege::ShaderResrc* getShaderResource( const RgResrcHandle& handle )const;
        const ref::Sampler getSampler( const RgResrcHandle& handle )const;
        const ref::Buffer getBuffer( const RgResrcHandle& handle )const;
        const ref::Image getImage( const RgResrcHandle& handle )const;

        const kege::ShaderResrc* fetchShaderResource( const std::string& name )const;
        const ref::Sampler fetchSampler( const std::string& name )const;
        const ref::Buffer fetchBuffer( const std::string& name )const;
        const ref::Image fetchImage( const std::string& name )const;


        const std::vector< RgResrcHandle >& getShaderResources()const;
        const std::vector< ShaderPipeline >& getShaderPipelines()const;

        const std::vector< RgWriteResrcDesc >& getWrites()const;
        std::vector< RgWriteResrcDesc >& getWrites();

        const std::vector< RgReadResrcDesc >& getReads()const;
        std::vector< RgReadResrcDesc >& getReads();


        const RenderPassDefn& getRenderPassDefn()const;

        CommandEncoder* getCommandEncoder();
        CommandBuffer* getCommandBuffer();
        //RenderPassContext& getContext();
        bool hasFixedPipelines()const;

        void beginRendering();
        void endRendering();

        const SubmitInfo& getSubmitInfo()const;

        RenderPassType getType()const;
        int getPassId()const;

        bool execute();
        void destroy();
        ~RenderStage();
        RenderStage();

    public:

        void applyBarriers( const BarrierDescriptions& barriers );
        void destroySemaphores();

        std::vector< ShaderPipeline > _fixed_shader_pipelines;
        std::vector< RgResrcHandle > _shader_resources;

        std::vector< SubmitInfo > _submit_info;

        /**
         * barriers hold the transitions for the ouput resources for this render pass.
         * barriers is computed in the render-graph compile function
         */
        BarrierDescriptions _barriers;


        //std::vector< kege::BufferMemoryBarrier > _buffer_barriers;
        //std::vector< kege::ImageMemoryBarrier > _image_barriers;

        /**
         * render pass definition hold the input resources and output target for this render-pass
         */
        RenderPassDefn _defn;

        /**
         * a pointer to the parent render graph
         */
        RenderGraph* _graph;

        /**
         * This render-pass index id. The index of this render-pass in the parent render-graph render-pass array
         */
        int _id;

        Rect2D _render_area;
        
        friend RenderGraph;
    };

}
#endif /* render_stage_hpp */
