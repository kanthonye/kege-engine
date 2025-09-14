//
//  render-manager.hpp
//  physics
//
//  Created by Kenneth Esdaile on 9/8/25.
//

#ifndef render_manager_hpp
#define render_manager_hpp

#include "render-queue.hpp"
#include "entity-renderer.hpp"
#include "../graphics/render-graph/render-graph.hpp"
#include "../graphics/pipeline/shader-pipeline.hpp"
#include "../graphics/render-graph/render-graph-loader.hpp"

namespace kege{

    class RenderManager : public RefCounter
    {
    public:

        struct CreateInfo
        {
            kege::Engine* engine;
            kege::Graphics* graphics;
            kege::RenderGraph* graph;
            uint32_t frames_in_flight = 2;
        };

        /**
         * @brief Submits a renderable object for rendering.
         * @param object The renderable object to submit.
         */
        void submit( const RenderObject& object );

        /**
         * @brief Retrieves the internal render queue.
         * @return Reference to the internal RenderQueue.
         */
        RenderQueue& getRenderQueue();

        /**
         * @brief Execute the rendering of the render objects belonging .
         * @param context The render pass context to use in rendering the objects for the current pass.
         * This function processes the render queue and issues draw calls.
         */
        void operator()( kege::RenderPassContext* context );

        /**
         * @brief Executes the rendering process for the current frame.
         * @param dms Delta time in milliseconds since the last frame.
         * This function processes the render queue and issues draw calls.
         */
        void execute( double dms );

        /**
         * @brief Initializes the RenderManager and its resources.
         * @return True if initialization was successful, false otherwise.
         * This function sets up necessary resources and prepares the manager for rendering.
         */
        bool initialize();

        /**
         * @brief Shuts down the RenderManager and releases resources.
         * This function cleans up allocated resources and prepares the manager for destruction.
         */
        void shutdown();

        /**
         * @brief Clears the current render queue.
         * This function removes all submitted render objects from the queue.
         */
        void clear();

        explicit RenderManager( CreateInfo info );
        ~RenderManager();

    private:

        // internals
        kege::Engine* _engine;
        kege::Graphics* _graphics;
        kege::RenderGraph* _graph;
        uint32_t _frames_in_flight;

        RenderQueue _render_queue;
    };

}
#endif /* render_manager_hpp */
