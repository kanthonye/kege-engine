//
//  Renderer.cpp
//  editor
//
//  Created by Kenneth Esdaile on 4/17/26.
//

#include "renderer.hpp"
//#include "../core/device/vulkan/vk-device.hpp"

#include "../../resource/image-loader.hpp"
#include "../core/pipeline/shader-system/shader-pipeline-loader.hpp"
#include "../core/pipeline/shader-system/json-shader-loader.hpp"

namespace kege{

    kege::ShaderResourceBinder* Renderer::getShaderResourceBinder
    (
        kege::ShaderPipeline* pipeline,
        const kege::RgRenderPassDesc* pass_desc
    )
    {
        kege::ShaderResourceBinder* binder = nullptr;
        auto itr = _binders.find( pipeline );
        if ( itr == _binders.end() )
        {
            binder = createShaderResourceBinder( pipeline );
        }
        else
        {
            uint32_t frames_in_flight_index = _device->getFrameIndex() % int(itr->second.binders.size());
            binder = itr->second.binders[ frames_in_flight_index ].ref();
        }
        updateShaderResourceBinder( binder, pass_desc );
        return binder;
    }

    void Renderer::updateShaderResourceBinder
    (
        kege::ShaderResourceBinder* binder,
        const kege::RgRenderPassDesc* pass_desc
    )
    {
        for (auto i = pass_desc->reads.begin(); i != pass_desc->reads.end(); i++)
        {
            if ( i->semantic == kege::GlobalSemantic::None) {
                continue;
            }

            switch ( i->resource.type )
            {
                case kege::RgResrcType::Buffer:
                {
                    kege::ref::Buffer buffer = getBuffer( i->resource );
                    binder->setBuffers( i->semantic, 0, 1, &buffer );
                    break;
                }
                case kege::RgResrcType::Image:
                {
                    kege::ref::Image image = getImage( i->resource );
                    binder->setImages( i->semantic, 0, 1, &image );
                    binder->setSamplers( i->semantic, 0, 1, &_linear_sampler );
                    break;
                }
                case kege::RgResrcType::Sampler:
                {
                    kege::ref::Sampler sampler = getSampler( i->resource );
                    binder->setSamplers( i->semantic, 0, 1, &sampler );
                    break;
                }
                default:break;    
            }
        }
    }

    kege::ShaderResourceBinder* Renderer::createShaderResourceBinder( kege::ShaderPipeline* pipeline )
    {
        const std::vector< kege::GlobalBinding >& global_bindings = pipeline->getGlobalBindings();
        std::vector< kege::ShaderSetDesc > global_set_descriptions;
        for (int i=0; i<global_bindings.size(); ++i)
        {
            const kege::ShaderSetDesc* desc = pipeline->getSetDesc( global_bindings[i].set );
            if( desc != nullptr )
            {
                global_set_descriptions.push_back( *desc );
            }
        }

        PipelineBindContext* context = &_binders[ pipeline ];
        context->binders.resize(MAX_FRAMES_IN_FLIGHT);
        for (int i= 0; i<context->binders.size(); ++i)
        {
            context->binders[i] = _device->createShaderResourceBinder
            (
                (int)global_set_descriptions.size(),
                global_set_descriptions.data()
            );

            for (int j=0; j<global_bindings.size(); ++j)
            {
                const kege::ShaderSetDesc* desc = pipeline->getSetDesc( global_bindings[j].set );
                if( desc != nullptr )
                {
                    context->binders[i]->insertSemanticBinding( desc->at( global_bindings[j].binding ).name, global_bindings[j].semantic );
                }
            }

        }
        uint32_t frames_in_flight_index = _device->getFrameIndex() % int(global_bindings.size());
        return context->binders[ frames_in_flight_index ].ref();
    }

    kege::ref::Buffer Renderer::getBuffer( const RgHandle& handle )
    {
        kege::ResrcLink* link = _asset_manager->get< kege::ResrcLink >( handle.id );
        if ( link )
        {
            link->frame_id = _frame_count;
            uint32_t frames_in_flight_index = _device->getFrameIndex() % link->bufr_desc->frames_in_flight;
            if( link->asset_handle == 0 )
            {
                kege::BufferDesc desc;
                desc.size = link->bufr_desc->size;
                desc.name = link->bufr_desc->name.c_str();
                desc.memory_usage = link->bufr_desc->memory_usage;
                desc.usage = link->bufr_desc->usage;

                BuffersInFlight buffers_in_flight( link->bufr_desc->frames_in_flight );
                for (int i=0; i<link->bufr_desc->frames_in_flight; ++i)
                {
                    buffers_in_flight[i] = _device->createBuffer( desc );
                }
                link->asset_handle = _asset_manager->add< BuffersInFlight >( link->bufr_desc->name, buffers_in_flight );
                return buffers_in_flight[ frames_in_flight_index ];
            }
            BuffersInFlight* buffers_in_flight = _asset_manager->get< BuffersInFlight >( link->asset_handle );
            return buffers_in_flight->at( frames_in_flight_index );
        }
        return {};
    }

    kege::ref::Image Renderer::getImage( const RgHandle& handle )
    {
        kege::ResrcLink* link = _asset_manager->get< kege::ResrcLink >( handle.id );
        if ( link )
        {
            link->frame_id = _frame_count;
            if( link->asset_handle == 0 )
            {
                if ( link->imag_desc->is_swapcain_color )
                {
                    /**
                     * Note: Swapchain operates on its own frames_in_flight_index aka _image_index
                     */
                    link->imag_desc->frames_in_flight = _swapchain->getImageCount();
                    ImageInFlight images_in_flight( link->imag_desc->frames_in_flight );

                    std::vector< ref::Image > images = _swapchain->getColorImages();
                    for (int i=0; i<link->imag_desc->frames_in_flight; ++i)
                    {
                        images_in_flight[i] = images[i];
                    }
                    link->asset_handle = _asset_manager->add< ImageInFlight >( link->imag_desc->name, images_in_flight );
                }
                else if ( link->imag_desc->is_swapcain_depth )
                {
                    /**
                     * Note: Swapchain operates on its own frames_in_flight_index aka _image_index
                     */
                    link->imag_desc->frames_in_flight = _swapchain->getImageCount();
                    ImageInFlight images_in_flight( link->imag_desc->frames_in_flight );

                    std::vector< ref::Image > images = _swapchain->getDepthImages();
                    for (int i=0; i<link->imag_desc->frames_in_flight; ++i)
                    {
                        images_in_flight[i] = images[i];
                    }
                    link->asset_handle = _asset_manager->add< ImageInFlight >( link->imag_desc->name, images_in_flight );
                }
                else
                {
                    kege::ImageDesc desc;
                    desc.format = link->imag_desc->format;
                    desc.extent.width = link->imag_desc->width;
                    desc.extent.height = link->imag_desc->height;
                    desc.extent.depth = link->imag_desc->depth;
                    desc.name = link->imag_desc->name.c_str();
                    desc.usage = link->imag_desc->usages;
                    desc.initial_layout = link->imag_desc->layout;
                    desc.array_layers = link->imag_desc->array_layers;

                    ImageInFlight images_in_flight( link->imag_desc->frames_in_flight );
                    for (int i=0; i<link->imag_desc->frames_in_flight; ++i)
                    {
                        images_in_flight[i] = _device->createImage( desc );
                    }
                    link->asset_handle = _asset_manager->add< ImageInFlight >( link->imag_desc->name, images_in_flight );
                }
            }
            uint32_t frames_in_flight_index;
            if ( link->imag_desc->is_swapcain_color || link->imag_desc->is_swapcain_depth )
            {
                frames_in_flight_index = _swapchain->getImageIndex();
            }
            else
            {
                frames_in_flight_index = _device->getFrameIndex() % link->imag_desc->frames_in_flight;
            }
            ImageInFlight* images_in_flight = _asset_manager->get< ImageInFlight >( link->asset_handle );
            return images_in_flight->at( frames_in_flight_index );
        }
        return {};
    }

    kege::ref::Sampler Renderer::getSampler( const RgHandle& handle )
    {
        kege::ResrcLink* link = _asset_manager->get< kege::ResrcLink >( handle.id );
        if ( link )
        {
            link->frame_id = _frame_count;
            if( link->asset_handle == 0 )
            {
                kege::SamplerDesc desc;
                desc.min_filter = link->samp_desc->min_filter;
                desc.mag_filter = link->samp_desc->mag_filter;
                desc.mipmap_mode = link->samp_desc->mipmap_mode;
                desc.address_mode_u = link->samp_desc->address_mode_u;
                desc.address_mode_v = link->samp_desc->address_mode_v;
                desc.address_mode_w = link->samp_desc->address_mode_w;

                kege::ref::Sampler sampler = _device->createSampler( desc );
                link->asset_handle = _asset_manager->add< kege::ref::Sampler >( link->bufr_desc->name, sampler );
                return sampler;
            }
            return *_asset_manager->get< kege::ref::Sampler >( link->asset_handle );
        }
        return {};
    }

    std::vector< kege::DrawItem > Renderer::getCulledDrawItems( kege::Scene& scene, const kege::Frustum& frustum )
    {
        kege::AssetCacheTable< kege::Material >* materials = 0;
        std::vector<kege::DrawItem> items;
        for (auto [ entity, transform, renderable ] : scene.view<kege::Transform, kege::Renderable>())
        {
            if ( renderable->culled )
            {
                continue;
            }

            /**
             * Perform frustum culling on the renderable's bounding sphere. If the sphere is outside the frustum, 
             * skip this renderable.
             */
            if( testFrustumSphere( frustum, renderable->sphere ) )
            {
                continue;
            }
            
            //if( testFrustumAABB( frustum, drawable->box ) )
            //{
            //    continue;
            //}

            /**
             * Build the model matrices for this renderable based on its transform.
             */
            ModelMatrices matrices = kege::buildModelMatrice
            (
                transform->position,
                transform->orientation,
                transform->scale
            );

            /**
             * Lookup the material for this renderable. If the material is not found, use a default material. 
             * The material will provide the pipeline and other rendering information needed to create the draw item.
             */
            kege::Material* material = materials->get( renderable->material_id );
            if (material == nullptr)
            {
                material = &_default_material;
            }

            items.push_back
            ({
                .matrices    = matrices,
                .pipeline_id = material->pipeline_handle,
                .material_id = material->id,
                .mesh_id     = renderable->mesh_id,
                .pass        = renderable->pass,
                .first_element = renderable->start_index,
                .element_count = renderable->draw_count,
                .first_instance = renderable->first_instance,
                .instance_count = renderable->instance_count,
                .vertex_offset = renderable->vertex_offset,
                .render_layer = renderable->render_layer
            });
        }

        /**
         * Sort the draw items by pass, pipeline, and material for efficient rendering.
         */
        std::sort(items.data(), items.data() + items.size(), [](const DrawItem& a, const DrawItem& b)
        {
            if (a.pass != b.pass) return a.pass < b.pass;
            if (a.pipeline_id != b.pipeline_id) return a.pipeline_id < b.pipeline_id;
            if (a.material_id != b.material_id) return a.material_id < b.material_id;
            return a.mesh_id < b.mesh_id;
        });
        return items;
    }

    kege::Ref< kege::RenderView > Renderer::buildRenderView( kege::Scene& scene, const kege::CameraData& camera )
    {
        kege::Ref< kege::RenderView > view = new kege::RenderView;

        /**
         * Calculate the view frustum from the camera's projection and transform, and store it in 
         * the RenderView along with the camera data and viewport extent. This will be used for 
         * culling and rendering.
         */
        view->frustum = getFrustum( camera.projection, camera.transform );
        view->extent = _window->getSize();
        view->camera = camera;

        /**
         * Extract draw items from the scene, perform frustum culling, and sort them into batches 
         * based on pipeline, material, and mesh. Then fill the render graph's batch cache for 
         * each pass.
         */
        std::vector< kege::DrawItem > items = getCulledDrawItems( scene, view->frustum );

        /**
         * Create render batches for each unique combination of pipeline, material, and mesh.
         * Each batch will contain a list of instance matrices for draw calls that can be rendered together.
         * The batches are also categorized by render pass type for efficient rendering in the render graph.
         */
        view->batches.reserve(items.size()); // upper bound
        RenderBatch* current = nullptr;
        for (auto& item : items)
        {
            if (!current ||
                current->pipeline_id != item.pipeline_id ||
                current->mesh_id != item.mesh_id ||
                current->material_id != item.material_id)
            {
                // start new batch
                view->batches.emplace_back();
                current = &view->batches.back();
                current->pipeline_id = item.pipeline_id;
                current->material_id = item.material_id;
                current->mesh_id = item.mesh_id;
                current->pass = item.pass;

                view->pass_types.insert( current->pass );
            }

            current->instances.push_back( item.matrices );
        }

        /**
         * For each batch, add a pointer to it in the render graph's batch cache for the corresponding render pass type.
         */
        for (auto& b : view->batches)
        {
            view->render_pass_batchs[ (int)b.pass ].push_back( &b );
        }

        // Optional: pipeline sort per pass
        for (int i = 0; i < view->render_pass_batchs.size(); ++i)
        {
            RenderBatchPtrs& render_batch = view->render_pass_batchs[i];
            std::sort( render_batch.data(), render_batch.data() + render_batch.size(),
            []( kege::RenderBatch* a, kege::RenderBatch* b)
            {
                return a->pipeline_id < b->pipeline_id;
            });
        }
        return view;
    }

    kege::CameraData calcCameraData( const Extent2D& extent, const kege::Transform& transform, const kege::mat44& projection )
    {
        return kege::CameraData
        {
            .projection = projection,
            .transform = kege::viewMatrix< float >
            (
                transform.orientation,
                transform.position
            ),
            .position = kege::vec4(transform.position, 0.f),
        };
    }

    void Renderer::extractSceneViews( kege::Scene& scene, kege::RenderViewFrame& render_view_frame )
    {
        Extent2D extent = _window->getSize();
        kege::CameraData camera;

        /**
         * Iterate over all entities in the scene that have a Transform and either a Perspective or Orthographic 
         * component. For each camera entity, calculate the camera data including the projection matrix and view 
         * transform, and then build a RenderView for that camera. The RenderView will contain the frustum, extent, 
         * camera data, and batches of draw items for rendering. Finally, return the list of RenderViews to be 
         * used in the render graph.
         */

        for (auto [ entity, transform, perspective ] : scene.view<kege::Transform, kege::Perspective>())
        {
            kege::mat44 projection = kege::perspproj< float >
            (
                float( extent.width ) / float( extent.height ),
                perspective->fov,
                perspective->znear,
                perspective->zfar
            );
            camera = calcCameraData( extent, *transform, projection );
            render_view_frame.push_back( buildRenderView( scene, camera ) );
        }

        for (auto [ entity, transform, orthographic ] : scene.view<kege::Transform, kege::Orthographic>())
        {
            kege::mat44 projection = kege::orthoproj< float >
            (
                orthographic->left,
                orthographic->right,
                orthographic->above,
                orthographic->below,
                orthographic->znear,
                orthographic->zfar
            );
            camera = calcCameraData( extent, *transform, projection );
            render_view_frame.push_back( buildRenderView( scene, camera ) );
        }
    }

    void Renderer::extractGuiViews( kege::GUI& gui, kege::RenderViewFrame& render_view_frame )
    {
        const kege::ui::DrawRecord* record = gui.getDrawRecord( _device->getFrameIndex() );
        if ( record->batches.empty() ) return;
        
        kege::Ref< kege::RenderView > gui_view = new kege::RenderView;
        kege::RenderPassType render_pass_type = kege::RenderPassType::UI;
        
        gui_view->camera.screen_info = record->screen_info;;
        gui_view->camera.transform = kege::translate< float >(0, 0, 0);
        gui_view->camera.projection = gui.getProjection();
        gui_view->pass_types.insert( render_pass_type );
        
        for (auto& b : record->batches)
        {
            RenderBatch batch;
            batch.pass = render_pass_type;
            batch.pipeline_id = record->pipeline_id;
            batch.material_id = record->material_id;
            batch.mesh_id = record->mesh_id;

            batch.render_layer = 0;
            batch.vertex_offset = 0;
            batch.first_element = b.first_element;
            batch.element_count = b.element_count;
            batch.first_instance = b.first_instance;
            batch.instance_count = b.instance_count;

            batch.culled = false;

            gui_view->batches.push_back( batch );
        }

        for (auto& batch : gui_view->batches)
        {
            gui_view->render_pass_batchs[ (int)render_pass_type ].push_back( &batch );
        }

        render_view_frame.push_back( gui_view );
    }

    void Renderer::render( kege::Scene* scene, kege::GUI* gui )
    {
        std::vector< kege::SubmitInfo > submits;

        beginFrame();
        
        /**
         * Build, compile, and execute the render graph for the current frame. The render graph will
         * use the extracted render views to determine which batches of draw items to render for each
         * pass, and will handle resource transitions, synchronization, and command buffer submission
         * to the GPU. After execution, end the frame to present the rendered image.
         */
        {
            _render_graph->begin();

            /**
             * RenderViewFrame: Collection of all views (camera eye points) for the current frame.
             * Each view represents a collection of objects visible to a camera. So for each view
             * in side the frame, we collect all visivle objects rendering data and store it into
             * a RenderViewFrame object.
             */
            kege::RenderViewFrame render_view_frame;

            /**
             * Extract render views from the scene. Each render view corresponds to a camera in the
             * scene and contains the view frustum, camera data, and batches of draw items for rendering.
             * These views will be passed to the render graph to determine what to render and how to cull
             * objects.
             */
            if( scene )
            {
                extractSceneViews( *scene, render_view_frame );
            }

            /**
             * Extract render views from the GUI. Each render view corresponds to a Layout in the
             * GUI and contains the transform, camera data, and batches of draw items for rendering.
             */
            if( gui )
            {
                extractGuiViews( *gui, render_view_frame );
            }

            /**
             * Using the data from the RenderViewFrame object, for each views extracted collect
             * get all the necessary render-pass descriptions using the render pass type collected
             * from the visible objects. All the views and associated render passes will be stored
             * into the RenderGraphFrame object.
             */
            kege::RenderGraphFrame render_graph_frame = _render_graph->build( render_view_frame );

            /**
             * From the RenderGraphFrame, all the render view render passes needs to be compile
             * and sorted in correct execution order, which result in the RenderGraphExecutionPlan
             * object.
             */
            kege::RenderGraphExecutionPlan render_graph_execution_plan = _render_graph->compile( render_graph_frame, {} );

            /**
             * Finally, the execution plan is executed for the current frame, rendering the scene to the
             * swapchain images.
             *
             * Note: In a more complete implementation, you would likely want to pass the render views to
             * the render graph in a way that it can use them during execution, such as through a context
             * or by having the render graph access them directly. The current code assumes that the render
             * graph has access to the necessary data to perform culling and rendering based on the views.
             */
            submits = _render_graph->execute( this, render_graph_execution_plan );

            _render_graph->end();
        }

        _device->submitAndPresent( _swapchain, submits );
        endFrame();
    }

    bool Renderer::beginFrame()
    {
        _frame_command_buffer_counter = 0;
        cleanupOutdatedResources();

        if ( !_device->beginFrame() )
        {
            KEGE_LOG_ERROR << "Failed to begin Frame\n";
            return false;
        }

        kege::Result result = _swapchain->acquireNextImage();
        return result == kege::Result::SUCCESS;
    }

    bool Renderer::endFrame()
    {
        _device->endFrame();
        return true;
    }

    kege::CommandBuffer* Renderer::acquireCommandBuffer( QueueType type )
    {
        if (_frame_command_buffer_counter >= 128)
        {
            kege::Log::error << _frame_command_buffer_counter;
            kege::Log::error << " exceeds maximum number of CommandBuffers allowed per frame.";
            kege::Log::error << kege::Log::nl;
            return nullptr;
        }
        
        std::vector< kege::ref::CommandBuffer >& frame_command_buffers =
        _command_buffers[ _device->getFrameIndex() % _command_buffers.size() ];

        if ( _frame_command_buffer_counter >= frame_command_buffers.size())
        {
            frame_command_buffers.resize( frame_command_buffers.size() * _frame_command_buffer_counter + 1 );
            frame_command_buffers[ _frame_command_buffer_counter ] = _device->createCommandBuffer( type );
        }

        kege::CommandBuffer* cmd = frame_command_buffers[ _frame_command_buffer_counter ].ref();
        _frame_command_buffer_counter += 1;
        return cmd;
    }

    void Renderer::cleanupOutdatedResources()
    {
        std::vector< kege::ResrcLink* > deletables;
        auto* amgr = _asset_manager->getAssetCacheTable< kege::ResrcLink >();
        for (uint64_t h = amgr->begin(); h != 0; h = amgr->next( h ))
        {
            kege::ResrcLink* link = amgr->get( h );
            if (abs(int(_frame_count - link->frame_id)) >= 60 )
            {
                deletables.push_back( link );
            }
        }
        for (kege::ResrcLink* link : deletables )
        {
            switch ( link->type )
            {
                case kege::RgResrcType::Buffer:
                {
                    _asset_manager->erase< kege::ref::Buffer >( link->asset_handle );
                    link->asset_handle = 0;
                    break;
                }
                case kege::RgResrcType::Image:
                {
                    _asset_manager->erase< kege::ref::Image >( link->asset_handle );
                    link->asset_handle = 0;
                    break;
                }
                case kege::RgResrcType::Sampler:
                {
                    _asset_manager->erase< kege::ref::Sampler >( link->asset_handle );
                    link->asset_handle = 0;
                    break;
                }
                default:break;
            }

//            if ( link->binding_handle )
//            {
//                _asset_manager->erase< kege::Binding >( link->binding_handle );
//                link->binding_handle = 0;
//            }
        }
    }

    bool Renderer::initialize( kege::AppWindow* window, kege::AssetManager* am )
    {
        _window = window;
        _asset_manager = am;

        /**
         * Initialize the GraphicsInstance. The instance will enumerate physical devices (GPUs)
         * and select the best one based on the requirements specified in the DeviceInitializationInfo.
         * The instance will also handle any necessary setup for the chosen graphics API (e.g.
         * Vulkan instance creation, validation layers, etc.). The initialization info includes
         * preferences for discrete GPU, ray tracing support, and debug validation.
         */
        kege::DeviceInitializationInfo device_init_info = {};
        device_init_info.window = window;
        device_init_info.preferred_API = kege::GraphicsAPI::Vulkan;
        device_init_info.enable_raytracing = false;
        device_init_info.prefer_discrete_gpu = true;
        device_init_info.prefer_higher_api_version = true;
        device_init_info.require_shader_float64 = false;
        device_init_info.engine = "KEGE";
        device_init_info.name = "dev";
        device_init_info.enable_debug_validation = true;
        //device_init_info.enable_debug_general = true;
        //device_init_info.enable_debug_performance = true;

        /**
         * Create the GraphicsInstance based on the preferred graphics API. The instance will be
         * responsible for managing the connection to the graphics API and will be used to create
         * the GraphicsDevice. The instance will also handle any necessary cleanup when the renderer
         * is shut down.
         */
        switch ( device_init_info.preferred_API )
        {
            case GraphicsAPI::Vulkan:
            {
                _instance = new kege::vk::Instance;
                if ( !_instance->initalize( device_init_info ) )
                {
                    kege::Log::error << "Failed to initialize GraphicsInstance."<<Log::nl;
                    return false;
                }
                break;
            }
            case kege::GraphicsAPI::Metal:
            {
                break;
            }
            case kege::GraphicsAPI::D3D12:
            {
                break;
            }
            default:
            {
                break;
            }
        }

        /**
         * Create the GraphicsDevice using the instance. The device will represent the logical
         * connection to the GPU and will be used for all rendering operations. The instance
         * will select the best suitable physical device (GPU) based on the initialization info
         * and create a logical device from it.
         */
        _device = _instance->createDevice( _instance->getBestSuitablePhysicalDevice( device_init_info ) );
        if( !_device )
        {
            kege::Log::error << "Failed to create GraphicsDevice."<<Log::nl;
            return false;
        }
        KEGE_LOG_INFO << "GraphicsDevice initialized..."<<Log::nl;

        /**
         * Create the swapchain for the window. The swapchain will manage the images that are
         * presented to the screen. The swapchain creation info is filled out based on the window
         * size and desired formats. The swapchain will be created with the specified number of
         * images, formats, and presentation mode. The swapchain will also create the necessary
         * synchronization primitives for acquiring images and presenting them.
         */
        kege::Extent2D window_size = window->getSize();
        kege::SwapchainDesc swapchain_create_info = {};
        swapchain_create_info.image_count = kege::MAX_FRAMES_IN_FLIGHT;// + 1;
        swapchain_create_info.name = "swapchain";
        swapchain_create_info.width = window_size.width;
        swapchain_create_info.height = window_size.height;
        swapchain_create_info.color_format = kege::Format::bgra_u8_norm;
        swapchain_create_info.depth_format = kege::Format::depth_32;
        swapchain_create_info.present_mode = kege::PresentMode::Fifo;
        swapchain_create_info.present_queue_type = kege::QueueType::Graphics;
        swapchain_create_info.image_usage = kege::ImageUsage::Color | kege::ImageUsage::TransferDst;
        _swapchain_create_info = swapchain_create_info;
        _swapchain = _device->createSwapchain( _swapchain_create_info );
        if( !_swapchain )
        {
            return false;
        }

        //-----------------------------------------------------------------------//
        // Add Asset Loaders
        //-----------------------------------------------------------------------//

        _asset_manager->addLoader< ref::ShaderPipeline, kege::KMSLShaderLoader >( ".kmsl" );
        _asset_manager->addLoader< ref::ShaderPipeline, kege::JsonShaderLoader >( ".json" );
        _asset_manager->addLoader< ref::Image, kege::ImageLoader >( ".jpg" );
        _asset_manager->addLoader< ref::Image, kege::ImageLoader >( ".png" );

        //-----------------------------------------------------------------------//
        // Create Default Resources add them to AssetManager so they can be accessed
        //-----------------------------------------------------------------------//

        // default white texture (1x1 white pixel) for untextured materials or as a fallback when a texture is missing
        uint32_t color[] = {0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF};
        ref::Image default_image = _device->createImage
        ({
            .format = Format::rgba_u8_norm,
            .extent  = {2,2,1},
            .mip_levels = 1,
            .array_layers = 1,
            .samples = SampleCount::Count1,
            .usage = ImageUsage::Color | ImageUsage::Sampled,
            .memory_usage = MemoryUsage::GpuOnly,
            .data = &color,
            .name = "default-image",
        });
        _asset_manager->add< ref::Image >( "default", default_image );

        // default sampler
        _linear_sampler = _device->createSampler
        ({
            .min_filter = Filter::Linear,
            .mag_filter = Filter::Linear,
            .address_mode_u = AddressMode::ClampToEdge,
            .address_mode_v = AddressMode::ClampToEdge,
            .address_mode_w = AddressMode::ClampToEdge,
            .name = "default-sampler",
        });
        _asset_manager->add< ref::Sampler >( "linear", _linear_sampler );

        // default nearest sampler for point sampling (e.g. for pixel art or UI)
        ref::Sampler nearest_sampler = _device->createSampler
        ({
            .min_filter = Filter::Nearest,
            .mag_filter = Filter::Nearest,
            .address_mode_u = AddressMode::ClampToEdge,
            .address_mode_v = AddressMode::ClampToEdge,
            .address_mode_w = AddressMode::ClampToEdge,
            .name = "default-sampler",
        });
        _asset_manager->add< ref::Sampler >( "nearest", nearest_sampler );

        // fallback shader
        uint64_t error_shader_handle = _asset_manager->load< ref::ShaderPipeline >( "error-shader", "graphics-shaders/error/error.kmsl" );
        if( error_shader_handle == 0 )
        {
            return false;
        }

        ref::Image scene_image = _device->createImage
        ({
            .format = Format::rgba_u8_norm,
            .extent  = {2,2,1},
            .mip_levels = 1,
            .array_layers = 1,
            .samples = SampleCount::Count1,
            .usage = ImageUsage::Color | ImageUsage::Sampled,
            .memory_usage = MemoryUsage::GpuOnly,
            .data = nullptr,
            .name = "scene-image",
        });
        _asset_manager->add< ref::Image >( "scene-image", scene_image );

        /**
         * Create the render graph. The render graph will manage the rendering passes, resources, and
         * execution of the rendering pipeline. The render graph will be built based on a configuration
         * file (e.g. JSON) that defines the render passes, their dependencies, and the resources they
         * use. The render graph will be responsible for compiling this configuration into an execution
         * plan that can be efficiently executed each frame.
         * The render graph will also handle resource transitions, synchronization, and command buffer
         * submission to the GPU. By using a render graph, we can achieve better performance and
         * flexibility in our rendering pipeline, as well as easier management of complex rendering
         * techniques.
         */
        _render_graph = new kege::RenderGraph( this );
        if( !_render_graph->load( vfs("config/render-graph.json").str() ) ) return false;

        KEGE_LOG_INFO << "GraphicsInstance initialized..."<<Log::nl;

        _command_buffers.resize( MAX_FRAMES_IN_FLIGHT );
        return true;
    }

    kege::GraphicsDevice* Renderer::getDevice()
    {
        return _device.ref();
    }

    const kege::Material* Renderer::getDefaultMaterial()const
    {
        return &_default_material;
    }

    const kege::AssetManager* Renderer::getAssetManager()const
    {
        return _asset_manager;
    }

    kege::AssetManager* Renderer::getAssetManager()
    {
        return _asset_manager;
    }

    const kege::Swapchain* Renderer::getSwapchain() const
    {
        return _swapchain.ref();
    }

    const kege::AppWindow* Renderer::getWindow() const
    {
        return _window;
    }

    int Renderer::getFrameIndex() const
    {
        return _device->getFrameIndex();
    }

    kege::AppWindow* Renderer::getWindow()
    {
        return _window;
    }

    void Renderer::shutdown()
    {
        _binders.clear();
        _render_graph.clear();

        _asset_manager = nullptr;

        _swapchain.clear();
        _device.clear();
        _instance.clear();
        
        _window = nullptr;
    }

    Renderer::Renderer()
    :   _frame_command_buffer_counter(0)
    {
    }

    Renderer::~Renderer()
    {
        shutdown();
    }

}
