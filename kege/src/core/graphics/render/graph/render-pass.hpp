//
//  render-pass.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/20/25.
//

#ifndef render_pass_hpp
#define render_pass_hpp

#include "render-pass-context.hpp"

namespace kege{
    
    class RenderManager;

    /**
     * @struct RenderPass
     * @brief Manages the execution of a single rendering stage within a rendering pipeline.
     *
     * The RenderPass class is responsible for orchestrating the commands required to perform
     * a specific rendering operation. This includes beginning the command buffer, configuring
     * the render pass with its render-target, and initiating the draw calls. It acts as
     * an intermediary between the render graph and the actual rendering commands.
     */
    class RenderPass
    {
    public:

        const kege::ShaderResrc* getShaderResource( const RgResrcHandle& handle )const;
        const kege::SamplerHandle* getSampler( const RgResrcHandle& handle )const;
        const kege::BufferHandle* getBuffer( const RgResrcHandle& handle )const;
        const kege::ImageHandle* getImage( const RgResrcHandle& handle )const;

        const kege::ShaderResrc* fetchShaderResource( const std::string& name )const;
        const kege::SamplerHandle* fetchSampler( const std::string& name )const;
        const kege::BufferHandle* fetchBuffer( const std::string& name )const;
        const kege::ImageHandle* fetchImage( const std::string& name )const;


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
        
        void beginRendering( const int IMAGE_INDEX );
        void endRendering();

        RenderPassType getType()const;

        bool execute( RenderManager& render_manager );
        ~RenderPass();

    public:

        std::vector< ShaderPipeline > _fixed_shader_pipelines;
        std::vector< RgResrcHandle > _shader_resources;

        /**
         * The main command buffer for recording command
         */
        std::vector< CommandBuffer* > _command_buffers;

        /**
         * barriers hold the transitions for the ouput resources for this render pass.
         * barriers is computed in the render-graph compile function
         */
        BarrierDescription _barriers;

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
#endif /* render_pass_hpp */
