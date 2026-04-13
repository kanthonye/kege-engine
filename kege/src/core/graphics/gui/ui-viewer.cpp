//
//  ui-viewer.cpp
//  ui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#include "../render/graph/render-executor.hpp"
#include "ui-viewer.hpp"
namespace kege::ui{

    void Viewer::onWindowFrameBufferResize(int width, int height)
    {
        _fbo_size.width = width;
        _fbo_size.height = height;
    }
    
    void Viewer::onWindowResize(int width, int height)
    {
        _window_size.width = width;
        _window_size.height = height;
    }

    void Viewer::push( const ui::UIDrawInstance& instance )
    {
        memcpy(&_drawbuffer[ _draw_count ], &instance, sizeof(instance));
        _draw_count += 1;

        if ( _drawbuffer.size() <= _draw_count )
        {
            flush();
        }
    }

    UIDrawInstance& Viewer::nextInstance()
    {
        if ( _drawbuffer.size() <= _draw_count )
        {
            flush();
        }
        return _drawbuffer[ _draw_count++ ];
    }

    void Viewer::drawRect
    (
        const ui::Rect& rect,
        const ui::Color& color,
        const ui::Border& border,
        const ui::TexrInfo& texr_info,
        const ui::Rect& texel,
        const ui::Rect& clip_rect
    )
    {
        if ( color.a < 0.001f || rect.width == 0.f || rect.height == 0.f) return;

        UIDrawInstance& instance = nextInstance();

        instance.color     = ui::packRGBA8(color);
        instance.border    = border;
        instance.texel     = texel;
        instance.rect      = rect;
        instance.texr_info = texr_info;
        instance.clip_rect = clip_rect;
    }

    ui::Extent Viewer::drawText
    (
        const kege::vec2& start,
        float width,
        float font_size,
        uint32_t color,
        bool wrap_around,
        const char* text,
        const ui::Rect& clip_rect
    )
    {
        float a = (color & 0xFF) / 255.0;
        if ( a < 0.001f ) return {};

        kege::vec2 cursor = {start.x, start.y};
        float max_h = 0;
        //float sum_w = 0;
        float length;

        float end_x = start.x + width;

        //ui::Rect rect;
        //ui::Border border = {};
        //ui::TexrInfo texr = {1};
        ui::Extent extent = {};

        UIDrawInstance instance = {};
        instance.texr_info.id = 1;
        instance.texr_info.index = 1;
        instance.clip_rect = clip_rect;
        instance.color = color;

        float baseline = font_size - 1;

        for (const char* c = text; 0 < *c && _draw_count < _drawbuffer.size(); ++c )
        {
            const kege::Glyph& g = _font->glyphs()[ *c ];
            instance.rect.width  = font_size * g.scaled_width;
            instance.rect.height = font_size * g.scaled_height;
            max_h = kege::max<float>( max_h, instance.rect.height );

            length = cursor.x + instance.rect.width;
            if ((wrap_around && length > end_x ) || *c == '\n' )
            {
                cursor.x = start.x; // Reset X to start of the row
                cursor.y += max_h;  // Move Y to the next row
                extent.width = 0;
                if ( *c == '\n' )
                {
                    continue;
                }
            }

            /**
             * if the render width for the text is greate than 0 then render the text.
             * if not the compute the max height and width that the text spand. this is
             * to save us some computing power, other wise we will be computing it twice.
             * once for setting the max width and height of the text and another when rendering
             */
            if ( *c > 32 /*&& extent.width < width*/ )
            {
                instance.rect.x = cursor.x - font_size * g.bearing_x;
                instance.rect.y = cursor.y + baseline - font_size * g.bearing_y;

                instance.texel.x       = g.x;
                instance.texel.y       = g.y;
                instance.texel.width   = g.width;
                instance.texel.height  = g.height;

                push(instance);
            }

            extent.width += g.advance * font_size;
            cursor.x = start.x + extent.width; // Move cursor for next glyph

            if ( extent.width > width )
            {
                break;
            }
        }
        extent.height = cursor.y + max_h;
        return {extent.width, extent.height};
    }

    void Viewer::draw( const ui::Widget& widget, const ui::Rect& clip_rect )
    {
        if ( !widget.visible )
            return;

        float a = (widget.color & 0xFF) / 255.0;
        if ( a > 0.001f )
        {
            UIDrawInstance& instance = nextInstance();
            instance.rect       = widget.rect;
            instance.color      = widget.color;
            instance.border     = widget.border;
            instance.texel      = widget.texel;
            instance.texr_info  = widget.texr_info;
            instance.clip_rect  = clip_rect;
        };

        if ( widget.text.ptr && _font )
        {
            kege::vec2 start = { widget.rect.x, widget.rect.y };
            const Padding& padding = ((widget.style) ? widget.style->padding : widget.padding);
            //if ( widget.style )
            {
                switch ( ((widget.style) ? widget.style->align_text : widget.text.align) )
                {
                    case AlignText::Center:
                    {
                        start.x += (widget.rect.width  - widget.text.width) * 0.5;
                        start.y += (widget.rect.height - widget.text.height) * 0.5;
                        break;
                    }
                        
                    case AlignText::Right:
                    {
                        if ( widget.text.width != 0 )
                        {
                            start.x += ( widget.rect.width - widget.text.width - padding.right);
                        }
                        if ( widget.text.height != 0 )
                        {
                            start.y +=  widget.text.y + padding.above;
                        }
                        break;
                    }

                    case AlignText::Left:
                    default:
                    {
                        if ( widget.text.width != 0 )
                        {
                            start.x += widget.text.x + padding.left;
                        }
                        if ( widget.text.height != 0 )
                        {
                            start.y += widget.text.y + padding.above;
                        }
                        break;
                    };
                }
            }

            /*
            UIDrawInstance& instance = nextInstance();
            instance.rect.x       = start.x;
            instance.rect.y       = start.y;
            instance.rect.width  = widget.text.width;
            instance.rect.height = widget.text.height;
            instance.color      = 0xFF00FF40;
            instance.border     = {};
            instance.texel      = {};
            instance.texr_info  = {};
            instance.clip_rect  = clip_rect;
             */

            drawText
            (
                start,
                widget.rect.width - padding.left - padding.right,
                widget.text.font_size,
                widget.text.color,
                widget.alignment.wrap.enable,
                widget.text.ptr,
                clip_rect
            );
        }
    }

    bool checkOverlap(const ui::Rect& a, const ui::Rect& b) {
        // Check if one rectangle is to the left of the other
        if (a.x + a.width < b.x || b.x + b.width < a.x) {
            return false;
        }

        // Check if one rectangle is above the other
        // Note: This logic works regardless of whether Y increases up or down,
        // as long as it is consistent for both rectangles.
        if (a.y + a.height < b.y || b.y + b.height < a.y) {
            return false;
        }

        return true;
    }
    void Viewer::draw( ui::Layout& layout, int pid, ui::Rect clip_rect )
    {
        Widget* widget = layout[ pid ];
        draw( *widget, clip_rect );

        if ( widget->clip_overflow )
        {
            clip_rect = widget->rect;
            clip_rect.x += widget->padding.left;
            clip_rect.y += widget->padding.above;
            clip_rect.width -= widget->padding.left + widget->padding.right;
            clip_rect.height -= widget->padding.above + widget->padding.below;
        }

        if ( clip_rect.width > 0 && clip_rect.height > 0 )
        {
            for ( int eid = layout.head( pid ); eid != 0; eid = layout.next( eid )  )
            {
                if ( checkOverlap(widget->rect, layout[ eid ]->rect))
                {
                    draw( layout, eid, clip_rect );
                }
            }
        }
    }

    void Viewer::render( ui::Layout& layout )
    {
        for (size_t layer_index = 0; layer_index < layout._layers.size(); ++layer_index)
        {
            for (uint32_t root = layout._layers[ layer_index ].head; root != 0; root = layout._widgets[ layer_index ].layer.next)
            {
                if ( checkOverlap(layout._rect, layout[ root ]->rect))
                {
                    draw( layout, root, layout[ root ]->rect );
                }
            }
        }

        for (int layer_index = 0; layer_index < layout._layers.size(); ++layer_index)
        {
            ui::Layer& layer = layout._layers[ layer_index ];
            for( uint32_t root = layer.head; root != 0; root = layout.next( root ) )
            {
                draw( layout, root, _clip_rect );
            }
        }
        const Cursor& cursor = layout._cursor;
        if ( cursor._visible )
        {
            UIDrawInstance& instance = nextInstance();

            if( cursor._selection_active )
            {
                instance.color = cursor._color_selection;
                instance.rect = cursor._rect_selection;
            }
            else
            {
                instance.color = cursor._color_cursor;
                instance.rect = cursor._rect_cursor;
            }

            instance.clip_rect = layout.getRect();
            instance.border = {};
            instance.texel = {};
            instance.texr_info = {};
        }
    }

    void Viewer::begin()
    {
        _curr_buffer_index = 0;
        _draw_count = 0;
    }

    void Viewer::end(RenderExecutor* manager)
    {
        if ( 0 < _draw_count )
        {
            flush();
        }
        _shader_data->update();

        kege::mat44* matrices = reinterpret_cast< kege::mat44* >( _push_constant.data );
        matrices[0] = kege::orthoproj< float >
        (
            0, _window_size.width,
            0,-_window_size.height,
            -200.0, 200.0
        );
        matrices[1][0] = vec4
        (
            _window_size.width,
            _window_size.height,
            float( _window_size.width ) / float( _fbo_size.width ),
            0.f
        );
        manager->submit( RenderPassType::UI, _meshs[ _graphics->getFrameIndex() ], _shader_data, _push_constant );
    }

    void Viewer::setFont( const ref::Font& font )
    {
        _font = font;
    }

    const ref::Font& Viewer::getFont()const
    {
        return _font;
    }

    BufferBindInfo Viewer::createBuffer()
    {
        size_t size = _drawbuffer.size() * sizeof( UIDrawInstance );
        return BufferBindInfo
        {
            .range = size,
            .offset = 0,
            .buffer = _graphics->createBuffer
            ({
                .size = size,
                .usage = kege::BufferUsages::StorageBuffer,
                .memory_usage = kege::MemoryUsage::CpuToGpu,
                .data = _drawbuffer.data(),
                .name = "ui-instance-buffer"
            })
        };
    }

    void Viewer::flush()
    {
        kege::BufferBindings& binding = _buffer_bindings[ _graphics->getFrameIndex() ];
        if ( _curr_buffer_index >= binding.size() )
        {
            binding.push_back( createBuffer() );
        }
        else
        {
            ref::Buffer& bufr = binding[ _curr_buffer_index ].buffer;
            bufr->copyFrom( _drawbuffer.data(), _draw_count * sizeof(UIDrawInstance), 0 );
        }

        //_meshs[ _graphics->getFrameIndex() ]->getInstanceShaderData()->setBuffers( 0, binding );
        _meshs[ _graphics->getFrameIndex() ]->index_count = 4;
        _meshs[ _graphics->getFrameIndex() ]->first_index = 0;
        _meshs[ _graphics->getFrameIndex() ]->instance_count = _draw_count;
        _meshs[ _graphics->getFrameIndex() ]->first_instance = 0;
        _curr_buffer_index += 1;
        _draw_count = 0;
    }

    bool Viewer::initialize( kege::Graphics* graphics, kege::AssetManager* asset_manager, ref::Font font )
    {
        _asset_manager = asset_manager;
        _graphics = graphics;
        _font = font;

        Extent2D extent;

        extent = _graphics->getWindow()->getSize();
        _window_size.width = float( extent.width );
        _window_size.height = float( extent.height );

        extent = _graphics->getWindow()->getFramebufferSize();
        _fbo_size.width = float( extent.width );
        _fbo_size.height = float( extent.height );

        _clip_rect.x = _clip_rect.y = 0;
        _clip_rect.height = _fbo_size.height;
        _clip_rect.width = _fbo_size.width;

        kege::string shader_file = kege::vfs( "graphics-shaders/gui/ui.kmsl" );
        _pipeline = _asset_manager->load< ref::ShaderPipeline >( shader_file.c_str() );
        if( _pipeline == 0 )
        {
            kege::Log::error << "CREATE_FAILED -> ShaderPipeline -> " << shader_file.c_str() << Log::nl;
            return false;
        }
        _shader_pipeline = *_asset_manager->get< ref::ShaderPipeline >( _pipeline );

        _drawbuffer.resize(65536);
        _buffer_bindings[0] = { createBuffer() };
        _buffer_bindings[1] = { createBuffer() };
        _meshs[0] = new kege::Mesh({}, new kege::ShaderData( _shader_pipeline, { 0 }));
        if(_meshs[0]->getInstanceShaderData()->setBuffers(0, 0, _buffer_bindings[0]))
        {
            _meshs[0]->getInstanceShaderData()->update();
        }
        _meshs[1] = new kege::Mesh({}, new kege::ShaderData( _shader_pipeline,{ 0 }));
        _meshs[1]->getInstanceShaderData()->setBuffers(0, 0, _buffer_bindings[1]);
        _meshs[1]->getInstanceShaderData()->update();

        ref::Image* image = asset_manager->fetch< ref::Image >( "default" );
        ref::Sampler* sampler = asset_manager->fetch< ref::Sampler >( "default" );

        // create BindSet
        _shader_data = new kege::ShaderData( _shader_pipeline,{ "theme", "font", "scene" });
        setViewportImage({ .image = *image, .sampler = *sampler, .layout = ImageLayout::ShaderRead }, 0);
        setThemeImage({ .image = *image, .sampler = *sampler, .layout = ImageLayout::ShaderRead }, 0);
        setFontImage(font->getImageBindInfo(), 0);
        _shader_data->update();

        kege::mat44* matrices = reinterpret_cast< kege::mat44* >( _push_constant.data );
        _push_constant.size = sizeof(kege::mat44) + sizeof(kege::vec4);
        _push_constant.stages = ShaderStageFlag::Vertex | ShaderStageFlag::Fragment;
        _push_constant.offset = 0;
        matrices[0] = kege::orthoproj< float >
        (
            0, _window_size.width,
            0,-_window_size.height,
            -200.0, 200.0
        );
        matrices[1][0] = vec4
        (
            _window_size.width,
            _window_size.height,
            float( _window_size.width ) / float( _fbo_size.width ),
            0.f
        );
        return true;
    }

    void Viewer::setViewportImage( const kege::ImageBindInfo& info, int frame )
    {
        _shader_data->setImages( "scene", kege::ImageBindings{info}, frame );
    }

    void Viewer::setThemeImage( const kege::ImageBindInfo& info, int frame )
    {
        _shader_data->setImages( "theme", kege::ImageBindings{info}, frame  );
    }

    void Viewer::setFontImage( const kege::ImageBindInfo& info, int frame )
    {
        _shader_data->setImages( "font", kege::ImageBindings{info}, frame  );
    }

    void Viewer::shutdown()
    {
        if ( _graphics )
        {
            _meshs[0].clear();
            _meshs[1].clear();
            _shader_data.clear();
            _shader_pipeline.clear();

            _font.clear();
            _graphics = nullptr;
        }
    }

    Viewer::~Viewer()
    {
        shutdown();
    }

    Viewer::Viewer()
    :   _graphics( nullptr )
    ,   _curr_buffer_index( 0 )
    ,   _draw_count( 0 )
    {
    }

}
