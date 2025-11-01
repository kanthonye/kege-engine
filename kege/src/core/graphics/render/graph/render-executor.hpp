//
//  render-executor.hpp
//  physics
//
//  Created by Kenneth Esdaile on 9/8/25.
//

#ifndef render_manager_hpp
#define render_manager_hpp

#include "camera.hpp"
#include "render-queue.hpp"
#include "../pipeline/shader-pipeline.hpp"

namespace kege{

    class RenderGraph;
    class RenderStage;

    class RenderExecutor : public RefCounter
    {
    public:

        void setSceneCamera( const kege::CameraData& data );
        void setGuiCamera( const kege::CameraData& data );
        void updateLights( int size, void* data );

        /**
         * @brief Submits a renderable object for rendering.
         * @param object The renderable object to submit.
         */
        void submit( const RenderObject& object );

        kege::Graphics* getGraphics();

        /**
         * @brief Retrieves the internal render queue.
         * @return Reference to the internal RenderQueue.
         */
        RenderQueue& getRenderQueue();

        /**
         * @brief Initializes the RenderExecutor and its resources.
         * @return True if initialization was successful, false otherwise.
         * This function sets up necessary resources and prepares the manager for rendering.
         */
        bool initialize();

        /**
         * @brief Shuts down the RenderExecutor and releases resources.
         * This function cleans up allocated resources and prepares the manager for destruction.
         */
        void shutdown();

        /**
         * @brief Clears the current render queue.
         * This function removes all submitted render objects from the queue.
         */
        void clearRenderQueue();

        explicit RenderExecutor
        (
            kege::Graphics* graphics,
            uint32_t frames_in_flight
        );

        ~RenderExecutor();

    private:

        void drawObjects
        (
            CommandEncoder* encoder,
            RenderStage* pass,
            const ref::ShaderPipeline& pipeline,
            const std::vector< RenderObject >& objects
        );

        void drawMesh( CommandEncoder* encoder, const Ref< MeshSource >& mesh );

        void drawIndices( CommandEncoder* encoder, const Ref< MeshSource >& mesh );
        void drawArrays( CommandEncoder* encoder, const Ref< MeshSource >& mesh );

        /**
         * @brief Executes the rendering process for the current frame.
         * This function processes the render queue and issues draw calls.
         */
        void execute( RenderStage* pass );
        void execute( RenderGraph* graph, const ref::Semaphore& image_available, const ref::Semaphore& render_complete );

    private:

        uint32_t _frames_in_flight;

        RenderQueue _render_queue;

        kege::CameraData _scene_camera_data;
        kege::CameraData _gui_camera_data;

        Ref< MeshSource > _fullscreen_quad;
        Ref< MeshSource > _sphere;
        Ref< MeshSource > _cube;

        kege::Graphics* _graphics;
        friend kege::RenderGraph;
        friend kege::RenderStage;
    };

}
namespace kege::ref{
    typedef kege::Ref< kege::RenderExecutor > RenderExecutor;
}
#endif /* render_manager_hpp */
