//
//  Renderer.hpp
//  editor
//
//  Created by Kenneth Esdaile on 4/17/26.
//

#ifndef Renderer_hpp
#define Renderer_hpp

#include "../../scene/ecs.hpp"
#include "../../scene/scene.hpp"
#include "../../graphics/components/gui/ui-gui.hpp"
#include "../../graphics/components/mesh/mesh.hpp"
#include "../../graphics/components/light/light.hpp"
#include "../../graphics/components/camera/camera.hpp"
#include "../../graphics/components/camera/frustum.hpp"
#include "../../graphics/core/device/core/instance.hpp"
#include "../../graphics/render/graph/render-graph.hpp"
#include "../../graphics/core/device/vulkan/vk-instance.hpp"
#include "render-graph-resource-manager.hpp"

namespace kege
{
    class Renderer;
}

namespace kege::vk
{
    class Renderer;
}

namespace kege::ref
{
    typedef kege::Ref< kege::Renderer > Renderer;
}

namespace kege::cref
{
    typedef kege::Ref< const kege::Renderer > Renderer;
}

namespace kege{

    struct ResrcLink
    {
        union
        {
            kege::RgBufferDesc* bufr_desc;
            kege::RgImageDesc* imag_desc;
            kege::RgSamplerDesc* samp_desc;
        };
        kege::GlobalSemantic semantic = kege::GlobalSemantic::None;
        kege::RgResrcType type;
        uint64_t asset_handle = 0;
        uint32_t frame_id = 0;
    };

    class Renderer : public kege::RefCounter
    {
        using BuffersInFlight = kege::array< kege::ref::Buffer >;
        using ImageInFlight = kege::array< kege::ref::Image >;

    public:

        kege::ShaderResourceBinder* getShaderResourceBinder
        (
            kege::ShaderPipeline* pipeline,
            const kege::RgRenderPassDesc* pass_desc
        );

        kege::ref::Buffer getBuffer( const RgHandle& handle );
        kege::ref::Image getImage( const RgHandle& handle );
        kege::ref::Sampler getSampler( const RgHandle& handle );


        /**
         * @fn initialize
         * 
         * @brief Initialize the renderer with the given application window and asset manager. 
         * This will set up the graphics instance, device, swapchain, and any necessary resources 
         * for rendering. The initialization process may involve selecting a physical device (GPU), 
         * creating a logical device, setting up command pools and buffers, and preparing the 
         * render graph.
         * 
         * @param window Reference to the application window to render to.
         * @param am Reference to the asset manager for loading resources.
         * 
         * @return True if initialization was successful, false otherwise.
         */
        bool initialize( kege::AppWindow* window, kege::AssetManager* am );

        /** 
         * @fn shutdown
         * 
         * @brief Shut down the renderer and release all resources. This will clean up the graphics 
         * instance, device, swapchain, render graph, and any other resources that were created during 
         * initialization. It is important to call this function before exiting the application to ensure
         */
        void shutdown();

        kege::CommandBuffer* acquireCommandBuffer( QueueType type );
        const kege::Material* getDefaultMaterial()const;
        const kege::AssetManager* getAssetManager()const;
        kege::AssetManager* getAssetManager();
        const kege::Swapchain* getSwapchain() const;
        const kege::AppWindow* getWindow() const;

        /**
         * @fn getDevice
         *
         * @brief Get the graphics device.
         *
         * @return Pointer to the graphics device.
         */
        kege::GraphicsDevice* getDevice();

        /**
         * @fn getWindow
         * 
         * @brief Get the application window.
         * 
         * @return Pointer to the application window.
         */
        kege::AppWindow* getWindow();

        int getFrameIndex() const;

        /** 
         * @fn render
         * 
         * @brief Render the given scene.
         * 
         * @param scene Pointer to the scene to render.
         */
        void render( kege::Scene* scene, kege::GUI* gui );

        /**
         * @fn beginFrame
         *
         * @brief Begin a new frame.
         *
         * @return True if the frame was begun successfully, false otherwise.
         */
        bool beginFrame();

        /**
         * @fn endFrame
         *
         * @brief End the current frame.
         *
         * @return True if the frame was ended successfully, false otherwise.
         */
        bool endFrame();

        ~Renderer();
        Renderer();

    private:

        void cleanupOutdatedResources();

        void updateShaderResourceBinder
        (
            kege::ShaderResourceBinder* binder,
            const kege::RgRenderPassDesc* pass_desc
        );

        kege::ShaderResourceBinder* createShaderResourceBinder( kege::ShaderPipeline* pipeline );

        /**
         * @fn getCulledDrawItems
         *
         * @brief Extract draw items from the scene, perform frustum culling, and return a list of 
         * draw items that are visible in the current view. This function will iterate through the 
         * renderable entities in the scene, check if they are within the view frustum, and if so, 
         * create a DrawItem for each instance of the renderable. The resulting list of draw items 
         * will be used for sorting and batching before rendering.
         *
         * @param scene Reference to the scene to extract draw items from.
         * @param frustum Reference to the view frustum for culling.
         *
         * @return A vector of DrawItem structures representing the visible draw calls for this frame.
         */
        std::vector< kege::DrawItem > getCulledDrawItems( kege::Scene& scene, const kege::Frustum& frustum );

        /**
         * @fn buildRenderView
         *
         * @brief Build a RenderView for a given camera in the scene. This function will calculate the 
         * view frustum based on the camera's projection and transform, extract the visible draw items 
         * for that view, and organize them into render batches based on pipeline, material, and mesh. 
         * The resulting RenderView will contain all the information needed to render the scene from that 
         * camera's perspective.
         *
         * @param scene Reference to the scene to build the view from.
         * @param camera Reference to the camera data for this view.
         *
         * @return A Ref-counted pointer to the constructed RenderView.
         */
        kege::Ref< kege::RenderView > buildRenderView( kege::Scene& scene, const kege::CameraData& camera );

        /**
         * @fn extractSceneViews
         *
         * @brief Extract RenderViews from the scene for all cameras. This function will iterate through
         * all entities in the scene that have camera components, build a RenderView for each camera, and
         * return a list of RenderViews. Each RenderView will contain the necessary information for rendering
         * from that camera's perspective, including the view frustum, camera data, and batches of draw items.
         *
         * @param scene Reference to the scene to extract views from.
         * @param render_view_frame Reference to the RenderViewFrame to populate with the extracted views.
         */
        void extractSceneViews( kege::Scene& scene, kege::RenderViewFrame& render_view_frame );

        /**
         * @fn extractGuiViews
         * 
         * @brief Extract GUI views from the scene. This function will identify any GUI components in the scene,
         * build RenderViews for them, and add those views to the render view frame. GUI views may have different 
         * rendering requirements than regular 3D views, such as orthographic projection and different rendering layers.
         * The resulting GUI RenderViews will be used to render the user interface elements of the scene. 
         * 
         * @param gui Reference to the GUI system to extract views from.
         * @param render_view_frame Reference to the RenderViewFrame to populate with the extracted GUI views.
         */
        void extractGuiViews( kege::GUI& gui, kege::RenderViewFrame& render_view_frame );

        /**
         * @fn buildRenderGraphBatchCache
         *
         * @brief Build a cache of render batches for the render graph. This function will iterate through the 
         * render batches and cache them for efficient access during rendering.
         *
         * @param scene Reference to the scene to build the batch cache for.
         * @param batches Reference to the vector of render batches to cache.
         * @param batch_cache Reference to the render graph batch cache to populate.
         */
        void buildRenderGraphBatchCache
        (
            kege::Scene& scene,
            std::vector< kege::RenderBatch >& batches,
            kege::RenderPassBatchTable& batch_cache
        );

    private:

        struct PipelineBindContext
        {
            kege::array< kege::ref::ShaderResourceBinder > binders;
        };

        std::unordered_map< kege::ShaderPipeline*, PipelineBindContext > _binders;


//        struct GlobalShaderResourceSet
//        {
//            kege::ref::ShaderResourceSet set;
//            uint32_t frame_id;
//        };
//        std::unordered_map< uint32_t, GlobalShaderResourceSet > _sets;

        


        // Default material for rendering objects without a specified material
        kege::AssetManager* _asset_manager;

        // Application window reference for rendering output
        kege::AppWindow* _window;

        // Render graph for managing render passes and resources
        kege::ref::RenderGraph _render_graph;
        
        // Swapchain creation parameters
        kege::SwapchainDesc _swapchain_create_info;

        // Graphics instance, device, and application window references
        kege::ref::GraphicsInstance _instance;

        // Graphics device for creating resources and submitting commands
        kege::ref::GraphicsDevice _device;

        // Swapchain reference for presenting rendered images
        kege::ref::Swapchain _swapchain;

        kege::Material _default_material;
        ref::Sampler _linear_sampler;
        kege::array< std::vector< kege::ref::CommandBuffer > > _command_buffers;
        int _frame_command_buffer_counter;
        int _frame_count;
    };

}
namespace kege::ref{
    typedef kege::Ref< kege::Renderer > Renderer;
}
#endif /* Renderer_hpp */
