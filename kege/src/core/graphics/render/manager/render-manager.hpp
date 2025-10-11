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
#include "../graph/render-graph.hpp"
#include "../graph/render-graph-loader.hpp"
#include "../pipeline/shader-pipeline.hpp"

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

        void setSceneCamera( const kege::CameraData& data );
        void setGuiCamera( const kege::CameraData& data );
        void updateLights( int size, void* data );

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

        void drawObjects( CommandEncoder* encoder, RenderStage* pass, const ShaderPipeline& pipeline, const std::vector< RenderObject >& objects );
        void drawMesh( CommandEncoder* encoder, const Ref< MeshSource >& mesh );

        /**
         * @brief Executes the rendering process for the current frame.
         * This function processes the render queue and issues draw calls.
         */
        void execute( RenderStage* pass );

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

        void drawIndices( CommandEncoder* encoder, const Ref< MeshSource >& mesh );
        void drawArrays( CommandEncoder* encoder, const Ref< MeshSource >& mesh );

    private:

        // internals
        kege::Engine* _engine;
        kege::Graphics* _graphics;
        kege::RenderGraph* _graph;
        uint32_t _frames_in_flight;

        RenderQueue _render_queue;

        
//        ShaderResource _lights_shader_resource;
//        ShaderResource _camera_shader_resource;

        kege::CameraData _scene_camera_data;
        kege::CameraData _gui_camera_data;

        Ref< MeshSource > _fullscreen_quad;
        Ref< MeshSource > _sphere;
        Ref< MeshSource > _cube;
    };

}
#endif /* render_manager_hpp */
