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
            sum_w += g.advance * font_size;

            /**
             * if the render width for the text is greate than 0 then render the text.
             * if not the compute the max height and width that the text spand. this is
             * to save us some computing power, other wise we will be computing it twice.
             * once for setting the max width and height of the text and another when rendering
             */
            if ( *c > 32 && sum_w < width )
            {
                _drawbuffer[ _count ].rect.width    = w;
                _drawbuffer[ _count ].rect.height   = h;

                _drawbuffer[ _count ].color         = color;
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

            cursor.x = start.x + sum_w; // Move cursor for next glyph

            if ( sum_w > width )
            {
                break;
            }
        }
        return {sum_w, max_h};
    }

    void Viewer::draw( const ui::Content& content, const ui::Rect& clip_rect )
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
            _drawbuffer[ _count ].texture_id    = 0.0f;
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

            vec2 dim = drawText
            (
                start,
                content.rect.width,
                content.style->font_size,
                content.style->color,
                content.style->wrap_around,
                content.text.text,
                clip_rect
            );
            content.text.width = dim.x;
            content.text.height = dim.y;
        }
    }

    void Viewer::draw( ui::Layout& layout, int pid, const ui::Rect& clip_rect )
    {
        drawsort( layout, pid );
//        draw( *layout[ pid ], clip_rect );


//        std::vector< std::pair< int, ui::Content* > > contents( layout.count( pid ) );
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

    void Viewer::linearize( ui::Layout& layout, int pid, int zindex, std::vector< std::pair< int, ui::Content* > >& contents, int& count )
    {
        contents[count++] = { zindex, layout[pid] };
        for ( int eid = layout.head( pid ); eid != 0; eid = layout.next( eid )  )
        {
            linearize( layout, eid, layout[eid]->style->zindex + layout[pid]->style->zindex, contents, count );
        }
    }

    void Viewer::insertionSort(std::vector< std::pair< int, ui::Content* > >& arr)
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
        std::vector< std::pair< int, ui::Content* > > contents( layout.count() );
        int count = 0;

        linearize( layout, 1, layout[1]->style->zindex, contents, count );
        insertionSort( contents );

        std::vector< ui::Rect > clip_rect_stack;
        ui::Rect clip_rect = contents[0].second->rect;
        for ( int i = 0; i < contents.size(); ++i  )
        {
            ui::Content* content = contents[i].second;
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

    void Viewer::setUiImages( SamplerHandle sampler, std::vector< kege::ImageHandle > images )
    {
        std::vector< ImageInfo > image_info;
        for (int i=0; i < images.size(); ++i)
        {
            image_info.push_back
            ({
                .image = images[i],
                .sampler = sampler,
                .layout = ImageLayout::ShaderReadOnly
            });
        }
        bool result = _graphics->updateDescriptorSets
        ({
            kege::WriteDescriptorSet
            {
                .array_element = 0,
                .binding = 0,
                .descriptor_type = DescriptorType::CombinedImageSampler,
                .image_info = image_info,
                .set = _descriptor_ui_texture
            }
        });
        if ( result == false )
        {
            kege::Log::error << "unable to update shader resource 'ui_texture'." <<Log::nl;
        }
    }

    void Viewer::flush()
    {
        _graphics->updateBuffer( _gpu_draw_buffer[ _graphics->getCurrFrameIndex() ], 0, _count * sizeof(ui::DrawElem), _drawbuffer.data());

        _encoder->bindGraphicsPipeline( _pipeline );
        _encoder->bindDescriptorSets( _shader_resource_draw_buffer[ _graphics->getCurrFrameIndex() ], false );
        _encoder->bindDescriptorSets( _shader_resource_font, false );
        _encoder->bindDescriptorSets( _descriptor_ui_texture, false );
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

        for (int i = 0; i<kege::MAX_FRAMES_IN_FLIGHT; ++i)
        {
            _gpu_draw_buffer[i] = _graphics->createBuffer
            ({
                .size = _max_render_instances * sizeof(ui::DrawElem),
                .usage = kege::BufferUsage::StorageBuffer,
                .memory_usage = MemoryUsage::CpuToGpu,
                .data = nullptr
            });

            _shader_resource_draw_buffer[i] = _graphics->allocateDescriptorSet({DescriptorSetLayoutBinding{
                .name = "UIViewBuffer",
                .descriptor_type = DescriptorType::StorageBuffer,
                .binding = 0,
                .count = 1,
                .stage_flags = ShaderStage::Vertex,
            }});

            _graphics->updateDescriptorSets({ kege::WriteDescriptorSet{
                .array_element = 0,
                .binding = 0,
                .descriptor_type = DescriptorType::StorageBuffer,
                .buffer_info =
                {{
                    .buffer = _gpu_draw_buffer[i],
                    .offset = 0,
                    .range = _max_render_instances * sizeof(ui::DrawElem)
                }},
                .set = _shader_resource_draw_buffer[i]
            } });
        }


        _shader_resource_font = _graphics->allocateDescriptorSet({ DescriptorSetLayoutBinding{
            .name = "sdf_font_texture",
            .descriptor_type = DescriptorType::CombinedImageSampler,
            .stage_flags = ShaderStage::Fragment,
            .binding = 0,
            .count = 1,
        }});
        _graphics->updateDescriptorSets({ kege::WriteDescriptorSet{
            .array_element = 0,
            .binding = 0,
            .descriptor_type = DescriptorType::CombinedImageSampler,
            .image_info =
            {{
                .image = _font->getImage(),
                .sampler = _font->getSampler(),
                .layout = ImageLayout::ShaderReadOnly
            }},
            .set = _shader_resource_font
        } });


        uint32_t color[] = {0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF};
        _default_ui_texture = _graphics->createImage( ImageDesc{
            .width  = 2,
            .height = 2,
            .depth  = 1,
            .mip_levels = 1,
            .memory_usage = MemoryUsage::GpuOnly,
            .sample_count = SampleCount::Count1,
            .format = Format::rgba_u8_norm,
            .data = &color
        });
        
        _descriptor_ui_texture = _graphics->allocateDescriptorSet({ DescriptorSetLayoutBinding{
            .name = "ui_texture",
            .descriptor_type = DescriptorType::CombinedImageSampler,
            .stage_flags = ShaderStage::Fragment,
            .binding = 0,
            .count = 16,
        }});
        _graphics->updateDescriptorSets({ kege::WriteDescriptorSet{
            .array_element = 0,
            .binding = 0,
            .descriptor_type = DescriptorType::CombinedImageSampler,
            .image_info =
            {{
                .image = _default_ui_texture,
                .sampler = _font->getSampler(),
                .layout = ImageLayout::ShaderReadOnly
            }},
            .set = _descriptor_ui_texture
        } });


//        _shader_resource_font = _graphics->allocateDescriptorSet
//        ({
//            .stage_flags = ShaderStage::Fragment,
//            .bindings =
//            {
//                DescriptorSetBindingInfo
//                {
//                    .name = "ui_texture",
//                    .descriptor_type = DescriptorType::CombinedImageSampler,
//                    .binding = 1,
//                    .array_element = 0,
//                    .image_info =
//                    {{
//                        .image = _default_ui_texture,
//                        .sampler = _font->getSampler(),
//                        .layout = ImageLayout::ShaderReadOnly
//                    }}
//                }
//            }
//         });

        return true;
    }

    void Viewer::shutdow()
    {
        if ( _graphics )
        {
            _graphics->destroyImage( _default_ui_texture );

            if( _shader_resource_font )
                _graphics->freeDescriptorSet( _shader_resource_font );
            for (int i = 0; i<kege::MAX_FRAMES_IN_FLIGHT; ++i)
            {
                if( _shader_resource_draw_buffer[i] )
                    _graphics->freeDescriptorSet( _shader_resource_draw_buffer[i] );
            }
            if( _pipeline )
                _graphics->destroyGraphicsPipeline( _pipeline );
            _font.clear();
            _graphics = nullptr;
        }
    }

    Viewer::Viewer()
    :   _max_render_instances( 500 )
    {}

}
