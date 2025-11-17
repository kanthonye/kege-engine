//
//  ui-viewer.cpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#include "render-executor.hpp"
#include "ui-viewer.hpp"
namespace kege::ui{

    kege::vec2 Viewer::drawText
    (
        const kege::vec2& start,
        float width,
        float font_size,
        const ui::Color& color,
        bool wrap_around,
        const char* text,
        const ui::Rect& clip_rect
    )
    {
        //const int text_index = node.text_id;
        kege::vec2 cursor = {start.x, start.y};
        float max_h = 0;
        float sum_w = 0;
        float length;
        for (const char* c = text; 0 < *c && _draw_count < _drawbuffer.size(); ++c )
        {
            const kege::Glyph& g = _font->glyphs()[ *c ];
            float w = font_size * g.scaled_width;
            float h = font_size * g.scaled_height;

            length = cursor.x + w;
            if ( wrap_around && length > width )
            {
                cursor.x = start.x; // Reset X to start of the row
                cursor.y += max_h;  // Move Y to the next row
                max_h = 0;          // Reset max row height
            }

            max_h = kege::max<float>( max_h, h );

            /**
             * if the render width for the text is greate than 0 then render the text.
             * if not the compute the max height and width that the text spand. this is
             * to save us some computing power, other wise we will be computing it twice.
             * once for setting the max width and height of the text and another when rendering
             */
            if ( *c > 32 && sum_w < width )
            {
                _drawbuffer[ _draw_count ].color         = color;
                _drawbuffer[ _draw_count ].rect.width    = w;
                _drawbuffer[ _draw_count ].rect.height   = h;
                _drawbuffer[ _draw_count ].rect.x        = cursor.x - font_size * g.bearing_x;
                _drawbuffer[ _draw_count ].rect.y        = cursor.y + font_size * g.bearing_y;
                _drawbuffer[ _draw_count ].texel.x       = g.x;
                _drawbuffer[ _draw_count ].texel.y       = g.y;
                _drawbuffer[ _draw_count ].texel.width   = g.width;
                _drawbuffer[ _draw_count ].texel.height  = g.height;
                _drawbuffer[ _draw_count ].texture_id    = 1.0f;
                _drawbuffer[ _draw_count ].border_radius = 0.0f;
                _drawbuffer[ _draw_count ].clip_rect     = clip_rect;
                _draw_count++;

                if ( _drawbuffer.size() <= _draw_count )
                {
                    flush();
                }
            }

            sum_w += g.advance * font_size;
            cursor.x = start.x + sum_w; // Move cursor for next glyph

            if ( sum_w > width )
            {
                break;
            }
        }
        return {sum_w, max_h};
    }

    void Viewer::draw( const ui::Widget& content, const ui::Rect& clip_rect )
    {
        if ( !content.visible )
            return;
        
        if ( content.style->background.color.a > 0.001f)
        {
            _drawbuffer[ _draw_count ].border_radius = content.style->border_radius.top_left;
            _drawbuffer[ _draw_count ].texture_id    = content.texr.id;
            _drawbuffer[ _draw_count ].color         = content.style->background.color;
            _drawbuffer[ _draw_count ].rect.height   = content.rect.height;
            _drawbuffer[ _draw_count ].rect.width    = content.rect.width;
            _drawbuffer[ _draw_count ].rect.x        = content.rect.x;
            _drawbuffer[ _draw_count ].rect.y        = content.rect.y;
            _drawbuffer[ _draw_count ].texel.x       = content.texr.x;
            _drawbuffer[ _draw_count ].texel.y       = content.texr.y;
            _drawbuffer[ _draw_count ].texel.width   = content.texr.width;
            _drawbuffer[ _draw_count ].texel.height  = content.texr.height;
            _drawbuffer[ _draw_count ].clip_rect     = clip_rect;
            _draw_count++;

            if ( _drawbuffer.size() <= _draw_count )
            {
                flush();
            }
        }

        if ( content.text.text && _font )
        {
            kege::vec2 start = { content.rect.x, content.rect.y };
            switch ( content.style->align_text )
            {
                case AlignText::Center:
                {
                    start.x += (content.rect.width  - content.text.width) * 0.5;
                    start.y += (content.rect.height - content.text.height) * 0.5;
                    break;
                }
                case AlignText::Right:
                {
                    if ( content.text.width != 0 )
                    {
                        start.x += ( content.rect.width - content.text.width - content.style->padding.right);
                    }
                    if ( content.text.height != 0 )
                    {
                        start.y +=  content.text.y + content.style->padding.above;
                    }
                    break;
                }

                case AlignText::Left:
                default:
                {
                    if ( content.text.width != 0 )
                    {
                        start.x += content.text.x + content.style->padding.left;
                    }
                    if ( content.text.height != 0 )
                    {
                        start.y += content.text.y + content.style->padding.above;
                    }
                    break;
                };
            }

            drawText
            (
                start,
                content.rect.width,
                content.style->font_size,
                content.style->color,
                content.style->wrap_around,
                content.text.text.str(),
                clip_rect
            );
//            content.text.width = dim.x;
//            content.text.height = dim.y;
        }
    }

    void Viewer::draw( ui::Layout& layout, int pid, const ui::Rect& clip_rect )
    {
        drawsort( layout, pid );
        
        const Cursor& cursor = layout._cursor;
        if ( cursor._visible && cursor._editing )
        {
            float width = cursor._width;
            Color color = ui::Color(1,1,1,1);
            if( cursor._selection )
            {
                width = cursor._selection_end - cursor._offset;
                color = ui::Color(1,1,1,0.3);
            }
            _drawbuffer[ _draw_count ].border_radius = 4;
            _drawbuffer[ _draw_count ].texture_id    = 0;
            _drawbuffer[ _draw_count ].color         = color;
            _drawbuffer[ _draw_count ].rect.height   = cursor._height;
            _drawbuffer[ _draw_count ].rect.width    = width;
            _drawbuffer[ _draw_count ].rect.x        = cursor._x + cursor._offset;
            _drawbuffer[ _draw_count ].rect.y        = cursor._y;
            _drawbuffer[ _draw_count ].texel.x       = 0;
            _drawbuffer[ _draw_count ].texel.y       = 0;
            _drawbuffer[ _draw_count ].texel.width   = 0;
            _drawbuffer[ _draw_count ].texel.height  = 0;
            _drawbuffer[ _draw_count ].clip_rect     = clip_rect;
            _draw_count++;
        }
        if ( _drawbuffer.size() <= _draw_count )
        {
            flush();
        }

//        draw( *layout[ pid ], clip_rect );
//        for ( int eid = layout.head( pid ); eid != 0; eid = layout.next( eid )  )
//        {
//            draw( layout, eid, clip_rect );
//        }

//        std::vector< std::pair< int, ui::Widget* > > contents( layout.count( pid ) );
//        int count = 0;
//
//        linearize( layout, 1, contents, count );
//        insertionSort( contents );
//
//        for ( int i = 0; i < contents.size(); ++i  )
//        {
//            draw( *contents[i], clip_rect );
//        }
//
//        for ( int eid = layout.head( pid ); eid != 0; eid = layout.next( eid )  )
//        {
//            for ( int i = layout.head( eid ); i != 0; i = layout.next( i )  )
//            {
//                draw( layout, eid, clip_rect );
//            }
//        }

//        draw( *layout[ pid ], clip_rect );
//
//        if ( layout[ pid ]->style->clip_overflow )
//        {
//            for ( int eid = layout.head( pid ); eid != 0; eid = layout.next( eid )  )
//            {
//                ui::Rect clip_rect = layout[ pid ]->rect;
//                clip_rect.x += layout[ pid ]->style->padding.left;
//                clip_rect.y += layout[ pid ]->style->padding.above;
//                clip_rect.width -= layout[ pid ]->style->padding.left + layout[ pid ]->style->padding.right;
//                clip_rect.height -= layout[ pid ]->style->padding.above + layout[ pid ]->style->padding.below;
//                if ( clip_rect.width <= 0 || clip_rect.height <= 0 )
//                {
//                    continue;
//                }
//                draw( layout, eid, clip_rect );
//            }
//        }
//        else
//        {
//            for ( int eid = layout.head( pid ); eid != 0; eid = layout.next( eid )  )
//            {
//                draw( layout, eid, clip_rect );
//            }
//        }
    }

    void Viewer::linearize( ui::Layout& layout, int pid, int zindex, std::vector< std::pair< int, ui::Widget* > >& contents, int& count )
    {
        contents[count++] = { zindex, layout[pid] };
        for ( int eid = layout.head( pid ); eid != 0; eid = layout.next( eid )  )
        {
            linearize( layout, eid, layout[eid]->style->zindex + layout[pid]->style->zindex, contents, count );
        }
    }

    void Viewer::insertionSort(std::vector< std::pair< int, ui::Widget* > >& arr)
    {
        int n = (int)arr.size();
        for (int i = 1; i < n; i++)
        {
            auto key = arr[i]; // store the pair
            int j = i - 1;

            // Compare using the first element of the pair
            while (j >= 0 && arr[j].first > key.first)
            {
                arr[j + 1] = arr[j];
                j--;
            }

            arr[j + 1] = key;
        }
    }

    void Viewer::drawsort( ui::Layout& layout, int pid )
    {
        std::vector< std::pair< int, ui::Widget* > > contents( layout.count() );
        int count = 0;

        linearize( layout, 1, layout[1]->style->zindex, contents, count );
        insertionSort( contents );

        std::vector< ui::Rect > clip_rect_stack;
        ui::Rect clip_rect = contents[0].second->rect;
        for ( int i = 0; i < contents.size(); ++i  )
        {
            ui::Widget* content = contents[i].second;
            if( content == nullptr ) continue;
            
            draw( *content, clip_rect );
            if ( content->style->clip_overflow )
            {
                clip_rect = content->rect;
                clip_rect.x += content->style->padding.left;
                clip_rect.y += content->style->padding.above;
                clip_rect.width -= content->style->padding.left + content->style->padding.right;
                clip_rect.height -= content->style->padding.above + content->style->padding.below;
                if ( clip_rect.width <= 0 || clip_rect.height <= 0 )
                {
                    continue;
                }
            }
        }
    }

    void Viewer::collectVisibleWidgets( RenderExecutor* manager, ui::Layout& layout )
    {
        if ( layout.count() == 0 ) return;
        
        begin();
        draw( layout, 1, layout[1]->rect );
        end();

        manager->submit( RenderPassType::UI, _meshs[ _graphics->getFrameIndex() ], _shader_data, _push_constant );
    }

    void Viewer::begin()
    {
        _curr_buffer_index = 0;
        _draw_count = 0;
    }

    void Viewer::end()
    {
        if ( 0 < _draw_count )
        {
            flush();
        }
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
        size_t size = _max_render_instances * sizeof( kege::ui::DrawElem );
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
            bufr->copyFrom( _drawbuffer.data(), _draw_count * sizeof(ui::DrawElem), 0 );
        }

        //_meshs[ _graphics->getFrameIndex() ]->getInstanceShaderData()->setBuffers( 0, binding );
        _meshs[ _graphics->getFrameIndex() ]->index_count = 4;
        _meshs[ _graphics->getFrameIndex() ]->first_index = 0;
        _meshs[ _graphics->getFrameIndex() ]->instance_count = _draw_count;
        _meshs[ _graphics->getFrameIndex() ]->first_instance = 0;
        _curr_buffer_index += 1;
    }

    bool Viewer::initialize( kege::Graphics* graphics, kege::AssetManager* asset_manager, ref::Font font )
    {
        _asset_manager = asset_manager;
        _graphics = graphics;
        _font = font;

        kege::string shader_file = kege::vfs( "graphics-shaders/gui/gui-rounded-corner-sdf-text.json" );
        _pipeline = _asset_manager->load< ref::ShaderPipeline >( shader_file.c_str() );
        if( _pipeline == 0 )
        {
            kege::Log::error << "CREATE_FAILED -> ShaderPipeline -> " << shader_file.c_str() << Log::nl;
            return false;
        }
        _shader_pipeline = *_asset_manager->get< ref::ShaderPipeline >( _pipeline );

        _buffer_bindings[0] = { createBuffer() };
        _buffer_bindings[1] = { createBuffer() };
        _meshs[0] = new kege::Mesh({}, new kege::ShaderData( _shader_pipeline,{ "UIViewBuffer" }));
        _meshs[0]->getInstanceShaderData()->setBuffers("UIViewBuffer", _buffer_bindings[0]);
        _meshs[1] = new kege::Mesh({}, new kege::ShaderData( _shader_pipeline,{ "UIViewBuffer" }));
        _meshs[1]->getInstanceShaderData()->setBuffers("UIViewBuffer", _buffer_bindings[1]);
        _meshs[0]->getInstanceShaderData()->update();
        _meshs[1]->getInstanceShaderData()->update();

        ref::Image* image = asset_manager->fetch< ref::Image >( "default" );
        ref::Sampler* sampler = asset_manager->fetch< ref::Sampler >( "default" );

        _shader_data = new kege::ShaderData( _shader_pipeline,{ "_scene","_theme","_font" });
        setViewportImage({ .image = *image, .sampler = *sampler, .layout = ImageLayout::ShaderRead }, 0);
        setThemeImage({ .image = *image, .sampler = *sampler, .layout = ImageLayout::ShaderRead }, 0);
        setFontImage(font->getImageBindInfo(), 0);

        _shader_data->update();

        _drawbuffer.resize(500);

        kege::mat44* matrices = reinterpret_cast< kege::mat44* >( _push_constant.data );
        _fbo_size = _graphics->getWindow()->getFramebufferSize();
        _push_constant.size = sizeof(kege::mat44) + sizeof(kege::vec4);
        _push_constant.stages = ShaderStageFlag::All;
        _push_constant.offset = 0;
        matrices[0] = kege::orthoproj< float >
        (
            0, float( _graphics->getWindow()->getWidth() ),
            0,-float( _graphics->getWindow()->getHeight() ),
            -200.0, 200.0
        );
        matrices[1][0] = vec4
        (
            _graphics->getWindow()->getWidth(),
            _graphics->getWindow()->getHeight(),
            float( _graphics->getWindow()->getWidth() ) / float( _fbo_size.width ),
            0.f
        );

        return true;
    }

    void Viewer::setViewportImage( const kege::ImageBindInfo& info, int frame )
    {
        _shader_data->setImages( "_scene", kege::ImageBindings{info}, frame );
    }

    void Viewer::setThemeImage( const kege::ImageBindInfo& info, int frame )
    {
        _shader_data->setImages( "_theme", kege::ImageBindings{info}, frame  );
    }

    void Viewer::setFontImage( const kege::ImageBindInfo& info, int frame )
    {
        _shader_data->setImages( "_font", kege::ImageBindings{info}, frame  );
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
    :   _max_render_instances( 500 )
    ,   _graphics( nullptr )
    ,   _curr_buffer_index( 0 )
    ,   _draw_count( 0 )
    {}

}
