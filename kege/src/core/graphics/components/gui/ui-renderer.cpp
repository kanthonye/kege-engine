//
//  ui-renderer.cpp
//  editor
//
//  Created by Kenneth Esdaile on 5/6/26.
//

#include "ui-renderer.hpp"
namespace kege::ui{

    ui::Extent Renderer::rendererText
    (
        const kege::vec2& start,
        float width,
        float font_size,
        uint32_t color,
        bool wrap_around,
        const char* text,
        const kege::ui::Rect& clip_rect
    )
    {
        float a = (color & 0xFF) / 255.0;
        if ( a < 0.001f ) return {};

        kege::vec2 cursor = {start.x, start.y};
        float max_h = 0;
        float length;
        float end_x = start.x + width;

        kege::ui::Extent extent = {};

//        kege::ui::DrawCommand command = {};
//        command.texr_info.id = 1;
//        command.texr_info.index = 1;
//        command.clip_rect = clip_rect;
//        command.color = color;

        float baseline = font_size - 1;

//        for (const char* c = text; 0 < *c && _gui_draw_batch.element_count < MAX_DRAW_COUNT; ++c )
//        {
//            const kege::Glyph& g = _font->glyphs()[ *c ];
//            command.rect.width  = font_size * g.scaled_width;
//            command.rect.height = font_size * g.scaled_height;
//            max_h = kege::max<float>( max_h, command.rect.height );
//
//            length = cursor.x + command.rect.width;
//            if ((wrap_around && length > end_x ) || *c == '\n' )
//            {
//                cursor.x = start.x; // Reset X to start of the row
//                cursor.y += max_h;  // Move Y to the next row
//                extent.width = 0;
//                if ( *c == '\n' )
//                {
//                    continue;
//                }
//            }
//
//            /**
//             * if the render width for the text is greate than 0 then render the text.
//             * if not the compute the max height and width that the text spand. this is
//             * to save us some computing power, other wise we will be computing it twice.
//             * once for setting the max width and height of the text and another when rendering
//             */
//            if ( *c > 32 /*&& extent.width < width*/ )
//            {
//                command.rect.x = cursor.x - font_size * g.bearing_x;
//                command.rect.y = cursor.y + baseline - font_size * g.bearing_y;
//
//                command.texel.x       = g.x;
//                command.texel.y       = g.y;
//                command.texel.width   = g.width;
//                command.texel.height  = g.height;
//
//                if ( kege::ui::checkOverlap( command.rect, clip_rect ))
//                {
//                    draw( command );
//                }
//            }
//
//            extent.width += g.advance * font_size;
//            cursor.x = start.x + extent.width; // Move cursor for next glyph
//
//            if ( extent.width > width )
//            {
//                break;
//            }
//        }
        extent.height = cursor.y + max_h;
        return {extent.width, extent.height};
    }

    void Renderer::renderWidget
    (
        const kege::ui::Layout& layout,
        const kege::ui::Widget* widget,
        kege::ui::Rect clip_rect
    )
    {
//        draw
//        ({
//            .rect = widget->rect,
//            .texel = widget->texel,
//            .border = widget->border,
//            .clip_rect = clip_rect,
//            .color = widget->color,
//            .texr_info = widget->texr_info,
//         });
//
//        if ( widget->clip_overflow )
//        {
//            clip_rect = widget->rect;
//            clip_rect.x += widget->padding.left;
//            clip_rect.y += widget->padding.above;
//            clip_rect.width -= widget->padding.left + widget->padding.right;
//            clip_rect.height -= widget->padding.above + widget->padding.below;
//        }
//
//        if ( widget->text.data && _font )
//        {
//            kege::vec2 start = { widget->rect.x, widget->rect.y };
//            const Padding& padding = ((widget->style) ? widget->style->padding : widget->padding);
//            
//            switch ( ((widget->style) ? widget->style->align_text : widget->text.align) )
//            {
//                case AlignText::Center:
//                {
//                    start.x += (widget->rect.width  - widget->text.width) * 0.5;
//                    start.y += (widget->rect.height - widget->text.height) * 0.5;
//                    break;
//                }
//
//                case AlignText::Right:
//                {
//                    if ( widget->text.width != 0 )
//                    {
//                        start.x += ( widget->rect.width - widget->text.width - padding.right);
//                    }
//                    if ( widget->text.height != 0 )
//                    {
//                        start.y +=  widget->text.y + padding.above;
//                    }
//                    break;
//                }
//
//                case AlignText::Left:
//                default:
//                {
//                    if ( widget->text.width != 0 )
//                    {
//                        start.x += widget->text.x + padding.left;
//                    }
//                    if ( widget->text.height != 0 )
//                    {
//                        start.y += widget->text.y + padding.above;
//                    }
//                    break;
//                };
//            }
//
//            rendererText
//            (
//                start,
//                widget->rect.width - padding.left - padding.right,
//                widget->text.font_size,
//                widget->text.color,
//                widget->alignment.wrap.enable,
//                widget->text.data,
//                clip_rect
//            );
//        }
//
//        if ( clip_rect.width > 0 && clip_rect.height > 0 )
//        {
//            for ( int eid = layout.head( widget->head ); eid != 0; eid = layout.next( eid )  )
//            {
//                if ( kege::ui::checkOverlap( widget->rect, layout[ eid ]->rect ) )
//                {
//                    renderWidget( layout, layout[ eid ], clip_rect );
//                }
//            }
//        }
    }

    void Renderer::render( const ui::Layout& layout )
    {
//        const kege::array< ui::Layer >& layers = layout.getLayers();
//        for (uint32_t layer_index = 0; layer_index < layers.size(); ++layer_index)
//        {
//            const ui::Layer& layer = layers[ layer_index ];
//            for( uint32_t root = layer.head; root != 0; root = layout.next( root ) )
//            {
//                const ui::Widget* widget = layout[ root ];
//                if ( kege::ui::.checkOverlap(layout.getRect(), widget->rect))
//                {
//                    renderWidget( layout, widget, widget->rect );
//                }
//            }
//        }
//
//        const Cursor& cursor = layout.getCursor();
//        if ( cursor.isVisible() )
//        {
//            ui::DrawCommand command = {};
//            if( cursor.isSelectionActive() )
//            {
//                command.color = cursor.getCursorColor();
//                command.rect = cursor.getSelectionRect();
//            }
//            else
//            {
//                command.color = cursor.getCursorColor();
//                command.rect = cursor.getSelectionRect();
//            }
//            command.clip_rect = layout.getRect();
//            draw( command );
//        }
    }

    void Renderer::draw( const ui::DrawInstance& command )
    {
    //    _ui_draw_command_buffer[ _gui_draw_batch.instance_count ] = command;
    //    _gui_draw_batch.instance_count += 1;
    }

    void Renderer::onWindowResize( const Extent2D& extent )
    {
        //_gui_draw_batch.extent.height = extent.height;
        //_gui_draw_batch.extent.width = extent.width;
        //_gui_draw_batch.projection = kege::orthoproj< float >
        //(
        //    0, extent.width, 0, extent.height, -200.0, 200.0
        //);
    }

    const kege::ref::Font& Renderer::getFont() const
    {
        return _font;
    }

    bool Renderer::initialize
    (
        kege::Renderer* renderer,
        kege::AssetManager* assets,
        kege::ECS* ecs
    )
    {
        renderer->getAssetManager();
        _graphics = renderer->getDevice();
        _assets = assets;
        _ecs = ecs;

        onWindowResize( renderer->getWindow()->getSize() );

        if( !loadFont() ) return false;
        if( !loadPipeline() ) return false;
        if( !createRenderableResources() ) return false;

        ecs::Entity entity = _ecs->create();
        //_ecs->add< kege::ui::GuiDrawBatch >( entity );
        _ecs->add< kege::Renderable >( entity );

        return true;
    }

    bool Renderer::createRenderableResources()
    {
        ref::Image* image = _assets->fetch< ref::Image >( "default" );
        if( image )
        {
            uint32_t color[] = {0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF};
            ref::Image default_image = _graphics->createImage
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
            uint64_t handle = _assets->add< ref::Image >( "default", default_image );
            image = _assets->get< ref::Image >( handle );
        }

        ref::Sampler* sampler = _assets->fetch< ref::Sampler >( "linear" );
        if( sampler )
        {
            ref::Sampler linear_sampler = _graphics->createSampler
            ({
                .min_filter = Filter::Linear,
                .mag_filter = Filter::Linear,
                .address_mode_u = AddressMode::ClampToEdge,
                .address_mode_v = AddressMode::ClampToEdge,
                .address_mode_w = AddressMode::ClampToEdge,
                .name = "default-sampler",
            });
            uint64_t handle = _assets->add< ref::Sampler >( "linear", linear_sampler );
            sampler = _assets->get< ref::Sampler >( handle );
        }

        for (int i=0; i<MAX_FRAMES_IN_FLIGHT; ++i)
        {
            ShaderSetDesc descs[2] = {
                ShaderSetDesc
                {
                    ShaderSetBindingDesc
                    {
                        .name = "Instances",
                        .usage = kege::BindingUsage::StorageBuffer,
                        .type = kege::BindType::Buffer,
                        .binding_index = 0,
                        .binding_count = 1,
                        //.starting_index = 0,
                        .stages = kege::ShaderStageFlag::Vertex,
                    }
                },
                ShaderSetDesc
                {
                    ShaderSetBindingDesc
                    {
                        .name = "Textures",
                        .usage = kege::BindingUsage::CombinedImageSampler,
                        .type = kege::BindType::Image,
                        .binding_index = 0,
                        .binding_count = 3,
                        //.starting_index = 0,
                        .stages = kege::ShaderStageFlag::Fragment,
                    }
                }
            };
            //ref::ShaderResourceBinder resource_binder = _graphics->createShaderResourceBinder(2, descs);

            size_t size = MAX_DRAW_COUNT * sizeof( kege::ui::DrawInstance );
            _ui_instance_buffers[i] = _graphics->createBuffer
            ({
                .size = size,
                .usage = kege::BufferUsages::StorageBuffer,
                .memory_usage = kege::MemoryUsage::CpuToGpu,
                .name = "ui-instance-buffer"
            });

            //resource_binder->setBuffer(0, 0, 0, {
           //     kege::BufferBindInfo{ .range = size, .offset = 0, .buffer = _ui_instance_buffers[i] }
            //});

            //resource_binder->setImage("Textures", 0, _font->getImageBindInfo());
            //resource_binder->setImage("Textures", 1, { .image = *image, .sampler = *sampler, .layout = ImageLayout::ShaderRead });
            //resource_binder->setImage("Textures", 2, { .image = *image, .sampler = *sampler, .layout = ImageLayout::ShaderRead });

            //kege::ref::Mesh mesh = new kege::BufferMesh( resource_binder, kege::AABB{} );
            //_gui_draw_batch.buffer_mesh_id[i] = _assets->add< ref::Mesh >( "gui-resource-binder", mesh );
        }
        return true;
    }

    bool Renderer::loadPipeline()
    {
        //kege::string shader_file = kege::vfs( "graphics-shaders/gui/ui.kmsl" );
        //_gui_draw_batch.pipeline_id = _assets->load< ref::ShaderPipeline >( shader_file.c_str() );
        //if( _gui_draw_batch.pipeline_id == 0 )
        //{
        //    kege::Log::error << "CREATE_FAILED -> ShaderPipeline -> " << shader_file.c_str() << Log::nl;
        //    return false;
        //}
        return true;
    }

    bool Renderer::loadFont()
    {
        kege::string font_filename = vfs( "assets/fonts/monaco.tga" ).c_str();
        _font = kege::FontCreator::create( _graphics, 8, 16, font_filename.c_str() );
        if( !_font )
        {
            kege::Log::error << "Failed to create font." << Log::nl;
            return false;
        }_assets->add< kege::ref::Font >( "monaco", _font );
        return true;
    }

    void Renderer::shutdown()
    {
        for (int i=0; i<MAX_FRAMES_IN_FLIGHT; ++i) _ui_instance_buffers[i].clear();
        _font.clear();
    }

    void Renderer::begin()
    {
        //_gui_draw_batch.first_element = 0;
        //_gui_draw_batch.element_count = 4;
        //_gui_draw_batch.first_instance = 0;
        //_gui_draw_batch.instance_count = 0;
        //uint32_t frame_index = _graphics->getFrameIndex();
        //_ui_draw_command_buffer = ( kege::ui::DrawCommand* ) _ui_instance_buffers[ frame_index ]->map();
    }

    void Renderer::end()
    {
        //if ( _ui_draw_command_buffer == nullptr ) return;

        uint32_t frame_index = _graphics->getFrameIndex();
        _ui_instance_buffers[ frame_index ]->unmap();
        //_ui_draw_command_buffer = nullptr;
    }


    Renderer::Renderer()
    {
        shutdown();
    }

}
