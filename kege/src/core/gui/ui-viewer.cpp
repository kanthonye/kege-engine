//
//  ui-viewer.cpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#include "render-manager.hpp"
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

    void Viewer::collectVisibleWidgets( RenderManager* manager, ui::Layout& layout )
    {
        begin();
        draw( layout, 1, layout[1]->rect );
        end();

        std::vector< kege::Ref< MeshSource > >& meshs = _meshes[ _graphics->getCurrFrameIndex() ];
        for (int i=0; i<_curr_mesh_index; ++i)
        {
            manager->submit
            (
                RenderObject
                {
                    .constant = _push_constant,
                    .material = _material,
                    .mesh = meshs[i],
                }
            );
        }
    }

    void Viewer::begin()
    {
        _curr_mesh_index = 0;
        _draw_count = 0;
    }

    void Viewer::end()
    {
        if ( 0 < _draw_count )
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

    kege::ImageHandle Viewer::getDefaultTexture()
    {
        return _default_texture;
    }

    kege::Ref< MaterialSource > Viewer::createMaterial()
    {
        kege::Ref< kege::ShaderResrc > resource = new kege::ShaderResrc
        ({
            .descriptors = kege::UniformDescriptorSets
            {
                kege::UniformDescriptorSet
                {
                    .set = 1,
                    .descriptors =
                    {
                        kege::UniformDescriptor
                        {
                            .descriptor_type = kege::DescriptorType::CombinedImageSampler,
                            .binding = 0,
                            .count = 1,
                            .name = "ui_theme"
                        }
                    }
                },
                kege::UniformDescriptorSet
                {
                    .set = 2,
                    .descriptors =
                    {
                        kege::UniformDescriptor
                        {
                            .descriptor_type = kege::DescriptorType::CombinedImageSampler,
                            .binding = 0,
                            .count = 1,
                            .name = "ui_font"
                        }
                    }
                },
                kege::UniformDescriptorSet
                {
                    .set = 3,
                    .descriptors =
                    {
                        kege::UniformDescriptor
                        {
                            .descriptor_type = kege::DescriptorType::CombinedImageSampler,
                            .binding = 0,
                            .count = 1,
                            .name = "ui_viewport"
                        }
                    }
                },
            },
            .resources = kege::UniformResourceSets
            {
                kege::UniformResourceSet
                {
                    kege::UniformResource
                    {
                        .binding = 0,
                        .uniform = kege::ImageBindings
                        {
                            kege::ImageInfo{ .image = _default_texture, .sampler = _font->getSampler(), .layout  = kege::ImageLayout::ShaderReadOnly }
                        }
                    }
                },
                kege::UniformResourceSet
                {
                    kege::UniformResource
                    {
                        .binding = 0,
                        .uniform = kege::ImageBindings
                        {
                            kege::ImageInfo{ .image = _font->getImage(), .sampler = _font->getSampler(), .layout  = kege::ImageLayout::ShaderReadOnly }
                        }
                    }
                },
                kege::UniformResourceSet
                {
                    kege::UniformResource
                    {
                        .binding = 0,
                        .uniform = kege::ImageBindings{ _scene_image_info }
                    }
                },
            },
            .graphics = _graphics
        });

        return new kege::MaterialSource( RenderPassType::UI, _pipeline, false, false, resource );
    }

    kege::Ref< MeshSource > Viewer::createMesh()
    {
        size_t size = _max_render_instances * sizeof( kege::ui::DrawElem );

        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        // create and setup the ui instance buffer shader resources
        // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

        kege::Ref< kege::MeshSource > source = new kege::MeshSource();
        source->instance_buffer_list = new InstanceBufferList
        ({
            kege::InstanceBuffer
            {
                .resource = new kege::ShaderResrc
                ({
                    .descriptors = kege::UniformDescriptorSets
                    {
                        kege::UniformDescriptorSet
                        {
                            .set = 0,
                            .descriptors =
                            {
                                kege::UniformDescriptor
                                {
                                    .descriptor_type = kege::DescriptorType::UniformBuffer,
                                    .binding = 0,
                                    .count = 1,
                                    .name = "draw_buffer"
                                }
                            }
                        },
                    },
                    .resources = kege::UniformResourceSets
                    {
                        kege::UniformResourceSet
                        {
                            kege::UniformResource
                            {
                                .binding = 0,
                                .uniform = kege::BufferBindings
                                {
                                    kege::BufferInfo
                                    {
                                        .buffer = _graphics->createBuffer
                                        ({
                                            .size = size,
                                            .usage = kege::BufferUsage::StorageBuffer,
                                            .memory_usage = kege::MemoryUsage::CpuToGpu,
                                            .data = _drawbuffer.data()
                                        }),
                                        .range = size,
                                        .offset = 0
                                    }
                                }
                            }
                        }
                    },
                    .graphics = _graphics
                }),
                .instance_count = _draw_count,
                .first_instance = 0,
            }
        });
        source->instance_count = _draw_count;
        source->first_instance = 0;
        source->first_index = 0;
        source->index_count = 4;
        
        source->material_index = 0;

        return source;
    }

    void Viewer::flush()
    {
        std::vector< kege::Ref< MeshSource > >& meshs = _meshes[ _graphics->getCurrFrameIndex() ];
        if ( _curr_mesh_index >= meshs.size() )
        {
            meshs.push_back( createMesh() );
        }
        else
        {
            _graphics->updateBuffer
            (
                meshs[ _curr_mesh_index ]->instance_buffer_list->getBufferHandle( 0 ),
                0, _draw_count * sizeof(ui::DrawElem), _drawbuffer.data()
            );
        }

        meshs[ _curr_mesh_index ]->instance_count = _draw_count;
        meshs[ _curr_mesh_index ]->instance_buffer_list->buffers[0].first_instance = 0;
        meshs[ _curr_mesh_index ]->instance_buffer_list->buffers[0].instance_count = _draw_count;
        _curr_mesh_index += 1;
    }

    bool Viewer::initialize( Graphics* graphics, kege::ShaderPipeline pipeline, kege::Font font, ImageInfo& scene_image_info )
    {
        _scene_image_info = scene_image_info;
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
        _drawbuffer.resize( _max_render_instances );
        memset( _drawbuffer.data(), 0x0, _drawbuffer.size()*sizeof(ui::DrawElem));
        /*
         * -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
         * initialize push constant
         * -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
         */

        kege::mat44* matrices = reinterpret_cast< kege::mat44* >( _push_constant.data );
        _push_constant.size = sizeof(kege::mat44) + sizeof(kege::vec4);
        _push_constant.stages = ShaderStage::Vertex | ShaderStage::Fragment;
        _push_constant.offset = 0;
        matrices[0] = kege::orthoproj< float >
        (
            0, float( _graphics->windowWidth() ),
            0,-float( _graphics->windowHeight() ),
            -200.0, 200.0
        );
        matrices[1][0] = vec4
        (
            _graphics->windowWidth(),
            _graphics->windowHeight(),
            float( _graphics->windowWidth() ) / float( _fbo_size.width ),
            0.f
        );

        /*
         * -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
         * create default texture
         * -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
         */

        uint32_t color[] = {0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF};
        _default_texture = _graphics->createImage
        ({
            .width  = 2,
            .height = 2,
            .depth  = 1,
            .mip_levels = 1,
            .memory_usage = MemoryUsage::GpuOnly,
            .sample_count = SampleCount::Count1,
            .format = Format::rgba_u8_norm,
            .data = &color
        });

        /*
         * -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
         * create material
         * -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
         */

        _material = createMaterial();

        return true;
    }

    void Viewer::shutdown()
    {
        if ( _graphics )
        {
            _material.clear();
            _meshes[0].clear();
            _meshes[1].clear();

            if( _default_texture )
            {
                _graphics->destroyImage( _default_texture );
                _default_texture = {};
            }
            for (int i = 0; i<kege::MAX_FRAMES_IN_FLIGHT; ++i)
            {
                if ( !_meshes[i].empty() ) {
                    _meshes[i][0]->unload( _graphics );
                    _meshes[i][0].clear();
                }
                _meshes[i].clear();
            }

            if( _material )
            {
                _material->resource.clear();
                _material.clear();
            }
//            if( _ui_texture_shader_resource )
//            {
//                _graphics->freeUniformSet( _ui_texture_shader_resource );
//            }
            if( _pipeline )
            {
                _pipeline = {};
                //_graphics->destroyGraphicsPipeline( _pipeline );
            }
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
    ,   _curr_mesh_index( 0 )
    ,   _draw_count( 0 )
    {}

}
