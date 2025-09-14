//
//  ui-viewer.cpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

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
        for (const char* c = text; 0 < *c && _count < _drawbuffer.size(); ++c )
        {
            const Glyph& g = _font->glyphs()[ *c ];
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
                _drawbuffer[ _count ].color         = color;
                _drawbuffer[ _count ].rect.width    = w;
                _drawbuffer[ _count ].rect.height   = h;
                _drawbuffer[ _count ].rect.x        = cursor.x - font_size * g.bearing_x;
                _drawbuffer[ _count ].rect.y        = cursor.y + font_size * g.bearing_y;
                _drawbuffer[ _count ].texel.x       = g.x;
                _drawbuffer[ _count ].texel.y       = g.y;
                _drawbuffer[ _count ].texel.width   = g.width;
                _drawbuffer[ _count ].texel.height  = g.height;
                _drawbuffer[ _count ].isfont        = 1.0f;
                _drawbuffer[ _count ].texture_id    = 0.0f;
                _drawbuffer[ _count ].border_radius = 0.0f;
                _drawbuffer[ _count ].clip_rect     = clip_rect;
                _count++;

                if ( _drawbuffer.size() <= _count )
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
            _drawbuffer[ _count ].border_radius = content.style->border_radius.top_left;
            _drawbuffer[ _count ].color         = content.style->background.color;
            _drawbuffer[ _count ].rect.height   = content.rect.height;
            _drawbuffer[ _count ].rect.width    = content.rect.width;
            _drawbuffer[ _count ].rect.x        = content.rect.x;
            _drawbuffer[ _count ].rect.y        = content.rect.y;
            _drawbuffer[ _count ].isfont        = 0.0f;
            _drawbuffer[ _count ].texture_id    = content.texr.id;
            _drawbuffer[ _count ].texel.x       = content.texr.x;
            _drawbuffer[ _count ].texel.y       = content.texr.y;
            _drawbuffer[ _count ].texel.width   = content.texr.width;
            _drawbuffer[ _count ].texel.height  = content.texr.height;
            _drawbuffer[ _count ].clip_rect     = clip_rect;
            _count++;

            if ( _drawbuffer.size() <= _count )
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

    void Viewer::begin( kege::CommandEncoder* encoder )
    {
        _encoder = encoder;
        _encoder->setViewport({ 0.f, 0.f, (float)_fbo_size.width, (float)_fbo_size.height });
        _encoder->setScissor({ 0, 0, _fbo_size.width, _fbo_size.height });
        _count = 0;
    }

    void Viewer::end()
    {
        if ( 0 < _count )
        {
            flush();
        }
    }

    void Viewer::setFont( const kege::Font& font )
    {
        _font = font;
    }

    const kege::Font& Viewer::getFont()const
    {
        return _font;
    }

    void Viewer::setUiImages( std::vector< kege::ImageInfo > image_info )
    {
        _ui_texture_shader_resource[0].images = image_info;
        if ( !_graphics->updateShaderResource( _ui_texture_shader_resource ) )
        {
            kege::Log::error << "unable to update shader resource 'ui_texture'." <<Log::nl;
        }
    }

    kege::ImageHandle Viewer::getDefaultTexture()
    {
        return _default_texture;
    }

    void Viewer::flush()
    {
        _graphics->updateBuffer( _indirect_draw_buffer[ _graphics->getCurrFrameIndex() ], 0, _count * sizeof(ui::DrawElem), _drawbuffer.data());

        _encoder->bindGraphicsPipeline( _pipeline );
        _encoder->bindShaderResource( _storage_buffer_resource[ _graphics->getCurrFrameIndex() ], false );
        _encoder->bindShaderResource( _font_shader_resource, false );
        _encoder->bindShaderResource( _ui_texture_shader_resource, false );
        _encoder->setPushConstants( ShaderStage::Vertex | ShaderStage::Fragment, 0, sizeof( _push_constant ), &_push_constant );
        _encoder->draw( 4, _count, 0, 0 );
        _count = 0;
    }

    bool Viewer::initialize( Graphics* graphics, kege::PipelineHandle pipeline, kege::Font font )
    {
        _graphics = graphics;
        if ( !_graphics )
        {
            return 0;
        }

        _pipeline = pipeline;
        if ( !_pipeline )
        {
            return 0;
        }

        _font = font;
        if ( !_font )
        {
            return 0;
        }
        
        _fbo_size = _graphics->getWindow()->getFramebufferSize();

        _push_constant.projection = kege::orthoproj< float >
        (
            0, float( _graphics->windowWidth() ),
            0,-float( _graphics->windowHeight() ),
            -200.0, 200.0
        );
        _push_constant.resolution = vec4
        (
            _graphics->windowWidth(),
            _graphics->windowHeight(),
            float( _graphics->windowWidth() ) / float( _fbo_size.width ),
            0.f
        );

        _drawbuffer.resize( _max_render_instances );

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // create and setup the ui instance buffer shader resources
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        size_t size = _max_render_instances * sizeof( ui::DrawElem );
        BufferDesc buffer_desc =
        {
            .size = size,
            .usage = kege::BufferUsage::StorageBuffer,
            .memory_usage = MemoryUsage::CpuToGpu,
            .data = nullptr
        };
        UniformLayoutDescription descriptors =
        {{
            .descriptor_type = DescriptorType::StorageBuffer,
            .stage_flags = ShaderStage::Vertex,
            .name = "UIViewBuffer",
            .binding = 0,
            .count = 1,
        }};
        _graphics->allocateShaderResources( descriptors, MAX_FRAMES_IN_FLIGHT, _storage_buffer_resource );
        for (int i = 0; i<kege::MAX_FRAMES_IN_FLIGHT; ++i)
        {
            _storage_buffer_resource[i][0] = BufferBinding
            {
                .binding = 0,
                .buffers
                {{
                    .buffer = _graphics->createBuffer( buffer_desc ),
                    .offset = 0,
                    .range = size
                }}
            };
            _graphics->updateShaderResource( _storage_buffer_resource[i] );
        }

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // create and setup the ui font shader resources
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        UniformDesc font_descriptors =
        {
            .descriptor_type = DescriptorType::CombinedImageSampler,
            .stage_flags = ShaderStage::Fragment,
            .name = "sdf_font_texture",
            .binding = 0,
            .count = 1,
        };
        _graphics->allocateShaderResource( font_descriptors, _font_shader_resource );
        _font_shader_resource[0] = ImageBindings{
            .binding = 0,
            .images = {{
                .image = _font->getImage(),
                .sampler = _font->getSampler(),
                .layout = ImageLayout::ShaderReadOnly
            }}
        };
        _graphics->updateShaderResource( _font_shader_resource );

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // create and setup the ui textures shader resources
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        uint32_t color[] = {0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF};
        _default_texture = _graphics->createImage( ImageDesc{
            .width  = 2,
            .height = 2,
            .depth  = 1,
            .mip_levels = 1,
            .memory_usage = MemoryUsage::GpuOnly,
            .sample_count = SampleCount::Count1,
            .format = Format::rgba_u8_norm,
            .data = &color
        });
        UniformLayoutDescription textures_descriptors =
        {{
            .descriptor_type = DescriptorType::CombinedImageSampler,
            .stage_flags = ShaderStage::Fragment,
            .name = "sdf_font_texture",
            .binding = 0,
            .count = 16,
        }};
        _graphics->allocateShaderResource( font_descriptors, _ui_texture_shader_resource );

        _ui_texture_shader_resource[0] = ImageBindings{
            .binding = 0,
            .images = std::vector< ImageInfo >( 16, ImageInfo{
                .image = _default_texture,
                .sampler = _font->getSampler()
            } )
        };
        _graphics->updateShaderResource( _ui_texture_shader_resource );
        
        return true;
    }

    void Viewer::shutdow()
    {
        if ( _graphics )
        {
            for (int i = 0; i<kege::MAX_FRAMES_IN_FLIGHT; ++i)
            {
                if( _indirect_draw_buffer[i] )
                {
                    _graphics->destroyBuffer( _indirect_draw_buffer[i] );
                }

                if( _storage_buffer_resource[i] )
                {
                    _graphics->destroyBuffer( _storage_buffer_resource[i][0].buffers[0].buffer );
                    _graphics->freeShaderResource( _storage_buffer_resource[i] );
                }
            }

            if( _default_texture )
            {
                _graphics->destroyImage( _default_texture );
                _default_texture = {};
            }
            if( _font_shader_resource )
            {
                _graphics->freeShaderResource( _font_shader_resource );
            }
            if( _ui_texture_shader_resource )
            {
                _graphics->freeShaderResource( _ui_texture_shader_resource );
            }
            if( _pipeline )
            {
                _graphics->destroyGraphicsPipeline( _pipeline );
            }
            _font.clear();
            _graphics = nullptr;
        }
    }
    BufferHandle   _indirect_draw_buffer[ kege::MAX_FRAMES_IN_FLIGHT ];
    ShaderResource _storage_buffer_resource[ kege::MAX_FRAMES_IN_FLIGHT ];
    ShaderResource _ui_texture_shader_resource;
    ShaderResource _font_shader_resource;


    Viewer::Viewer()
    :   _max_render_instances( 500 )
    ,   _graphics( nullptr )
    {}

}
