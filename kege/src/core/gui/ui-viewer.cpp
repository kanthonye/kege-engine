//
//  ui-viewer.cpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#include "ui-viewer.hpp"
namespace kege::ui{

    void Viewer::view( kege::CommandEncoder* encoder, ui::Layout& layout, Node& node, const Rect2D& clip_rect )
    {
        if ( node.elem.style.background.color.a > 0.001f)
        {
            _drawbuffer[ _count ].color         = node.elem.style.background.color;
            _drawbuffer[ _count ].rect.height   = node.elem.rect.height;
            _drawbuffer[ _count ].rect.width    = node.elem.rect.width;
            _drawbuffer[ _count ].rect.x        = node.elem.rect.x;
            _drawbuffer[ _count ].rect.y        = node.elem.rect.y;
            _drawbuffer[ _count ].border_radius = node.elem.style.border_radius;
            _drawbuffer[ _count ].isfont        = 0.0f;
            _drawbuffer[ _count ].texture_id    = 0.0f;
            _drawbuffer[ _count ].clip_rect     = clip_rect;
            _count++;

            if ( _drawbuffer.size() <= _count )
            {
                flush( encoder );
            }
        }

        if ( node.elem.text.data && _font )
        {
            const kege::vec2 start = // Tracks position for each character
            {
                node.elem.rect.x + node.elem.text.x + node.elem.style.padding.left,
                node.elem.rect.y + node.elem.text.y + node.elem.style.padding.right
            };

            const float font_size = node.elem.style.font_size;
            //const int text_index = node.text_id;
            kege::vec2 cursor = start;
            float max_h = 0;
            float sum_w = 0;

            for (const char* c = node.elem.text.data; 0 < *c && _count < _drawbuffer.size(); ++c )
            {
                const Glyph& g = _font->glyphs()[ *c ];
                float w = font_size * g.scaled_width;
                float h = font_size * g.scaled_height;

                if ( node.elem.style.align.wrap_around )
                {
                    // Check if the child overflows the parent's width
                    if ( cursor.x + w > node.elem.rect.width )
                    {
                        cursor.x = start.x; // Reset X to start of the row
                        cursor.y += max_h;  // Move Y to the next row
                        max_h = 0;          // Reset max row height
                    }
                }
                else if ( sum_w + g.advance * font_size > node.elem.rect.width )
                {
                    break;
                }

                if ( *c > 32 && 0 < node.elem.text.width )
                {
                    _drawbuffer[ _count ].rect.width    = w;
                    _drawbuffer[ _count ].rect.height   = h;

                    _drawbuffer[ _count ].color         = node.elem.style.color;
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
                        flush( encoder );
                    }
                }

                max_h = kege::max<float>( max_h, h );
                sum_w += g.advance * font_size;
                cursor.x = start.x + sum_w; // Move cursor for next glyph
            }
            node.elem.text.width = sum_w;
            switch ( node.elem.style.align.text )
            {
                case AlignText::Center:
                {
                    node.elem.text.x = (node.elem.rect.width - node.elem.text.width) * 0.5;
                    break;
                }
                case AlignText::Right:
                {
                    node.elem.text.x = (node.elem.rect.width - node.elem.text.width);
                    break;
                }
                default: break;
            }
        }
        if ( node.elem.style.clip_overflow )
        {
            for ( int i = node.head; i != 0; i = layout.nodes( i ).next  )
            {
                Rect2D clip_rect = node.elem.rect;
                clip_rect.x += node.elem.style.padding.left;
                clip_rect.y += node.elem.style.padding.above;
                clip_rect.width -= node.elem.style.padding.left + node.elem.style.padding.right;
                clip_rect.height -= node.elem.style.padding.above + node.elem.style.padding.below;
                if ( clip_rect.width <= 0 || clip_rect.height <= 0 )
                {
                    continue;
                }
                view( encoder, layout, layout.nodes( i ), clip_rect );
            }
        }
        else
        {
            for ( int i = node.head; i != 0; i = layout.nodes( i ).next  )
            {
                view( encoder, layout, layout.nodes( i ), clip_rect );
            }
        }
    }

    void Viewer::view( kege::CommandEncoder* encoder, ui::Layout& layout )
    {
        encoder->setViewport({ 0.f, 0.f, (float)_fbo_size.width, (float)_fbo_size.height });
        encoder->setScissor({ 0, 0, _fbo_size.width, _fbo_size.height });
        _count = 0;

        view( encoder, layout, layout.nodes( 1 ), layout.nodes( 1 ).elem.rect );
        
        if ( 0 < _count )
        {
            flush( encoder );
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

    void Viewer::flush( kege::CommandEncoder* encoder )
    {
        _graphics->updateBuffer( _gpu_draw_buffer[ _graphics->getCurrFrameIndex() ], 0, _count * sizeof(ui::DrawElem), _drawbuffer.data());

        encoder->bindGraphicsPipeline( _pipeline );
        encoder->bindDescriptorSets( _shader_resource_draw_buffer[ _graphics->getCurrFrameIndex() ], false );
        encoder->bindDescriptorSets( _shader_resource_font, false );
        encoder->bindDescriptorSets( _descriptor_ui_texture, false );
        encoder->setPushConstants( ShaderStage::Vertex | ShaderStage::Fragment, 0, sizeof( _push_constant ), &_push_constant );
        encoder->draw( 4, _count, 0, 0 );
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
