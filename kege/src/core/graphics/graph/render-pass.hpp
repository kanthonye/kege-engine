//
//  render-pass.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/20/25.
//

#ifndef render_pass_hpp
#define render_pass_hpp

#include <unordered_set>
#include "../../graphics/core/graphics.hpp"
#include "../../utils/communication.hpp"
#include "render-pass-context.hpp"

namespace kege{

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

        const std::vector< RgWriteResrcDesc >& getWrites()const;
        std::vector< RgWriteResrcDesc >& getWrites();

        const std::vector< RgReadResrcDesc >& getReads()const;
        std::vector< RgReadResrcDesc >& getReads();

        RenderPassContext& getContext();
        
        void beginRendering( const int IMAGE_INDEX );
        void endRendering();

        bool execute();
        ~RenderPass();

    public:

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
         * context hold the resources for this render pass.
         * context is initalize in the render-graph compile function
         */
        RenderPassContext _context;

        /**
         * render pass definition hold the input resources and output target for this render-pass
         */
        RenderPassDefn _defn;

        /**
         * a pointer to the parent render graph
         */
        RenderGraph* _graph;

        /**
         * This render-pass type eg. Graphics, Compute, Transfer
         */
        QueueType _type;

        /**
         * This render-pass index id. The index of this render-pass in the parent render-graph render-pass array
         */
        int _id;

        friend RenderGraph;
    };

}
#endif /* render_pass_hpp */
