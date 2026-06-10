//
//  ui-gui.cpp
//  editor
//
//  Created by Kenneth Esdaile on 5/12/26.
//

#include "ui-gui.hpp"
#include "../../render/renderer.hpp"

namespace kege{
    
    uint32_t GUI::computeExtent( int font_size, const char* text, float& width, float& height )
    {
        uint32_t count = 0;
        while (text[count] != 0)
        {
            const kege::Glyph& g = _font->glyphs()[ text[count] ];
            float h = font_size * (g.scaled_height);

            height = kege::max<float>( height, h );
            width += g.advance * font_size;
            count += 1;
        }
        return count;
    }

    kege::ui::Text GUI::text( const char* str, int font_size )
    {
        kege::ui::Text text;
        computeExtent(font_size, str, text.width, text.height);
        text.font_size = font_size;
        text.ptr = str;
        return text;
    }

    void GUI::onWindowResize(int width, int height)
    {
        _rect.height = height;
        _rect.width = width;
        for (auto& layout : _layouts)
        {
            layout.onWindowResize( width, height );
        }
    }

    kege::ui::WidgetId GUI::pushRoot( const kege::ui::WidgetDesc& desc )
    {
        return _layouts[ _layout_index ].pushRoot( desc );
    }

    kege::ui::WidgetId GUI::putRoot( const kege::ui::WidgetDesc& desc )
    {
        return _layouts[ _layout_index ].putRoot( desc );
    }

    void GUI::popRoot()
    {
        return _layouts[ _layout_index ].popRoot();
    }

    kege::ui::WidgetId GUI::push( const kege::ui::WidgetDesc& desc )
    {
        return _layouts[ _layout_index ].push( desc );
    }

    uint32_t GUI::pop()
    {
        return _layouts[ _layout_index ].pop();
    }

    kege::ui::WidgetId GUI::put( const kege::ui::WidgetDesc& desc )
    {
        return _layouts[ _layout_index ].put( desc );
    }

    kege::ui::WidgetId GUI::text( const kege::ui::Text& text )
    {
        return _layouts[ _layout_index ].text( text );
    }

    void GUI::pushLayer( uint32_t index )
    {
        return _layouts[ _layout_index ].pushLayer( index );
    }
    
    bool GUI::popLayer()
    {
        return _layouts[ _layout_index ].popLayer();
    }

    kege::dvec2 GUI::getClickPosition( kege::MouseButtonCode code ) const
    {
        return _mouse->getClickPosition( code );
    }

    kege::dvec2 GUI::getPointerPosition() const
    {
        return _mouse->getPosition();
    }

    kege::dvec2 GUI::getPointerDelta() const
    {
        return _mouse->getDelta();
    }

    kege::dvec2 GUI::getScrollOffset() const
    {
        return _mouse->getScrollDelta();
    }

    bool GUI::isPointerDragging() const
    {
        return _mouse->isDragging( kege::MouseButtonCode::Left );
    }

    bool GUI::leftClickDown()const
    {
        return _left_click_down;
    }

    bool GUI::mouseover( const kege::ui::ID& uid )const
    {
        return _curr.hot.user_id == uid && _curr.hot.user_id != 0;
    }

    bool GUI::doubleClick( const kege::ui::ID& uid )const
    {
        return _curr.hit.user_id == uid && _curr.clicks == 2 && _curr.hit.user_id != 0;
    }

    bool GUI::click( const kege::ui::ID& uid )const
    {
        return _curr.hit.user_id == uid && _curr.clicks == 1 && _curr.hit.user_id != 0;
    }

    bool GUI::hasFocus( const kege::ui::ID& uid )const
    {
        return _curr.focus.user_id == uid && _curr.focus.user_id != 0;
    }

    const kege::ui::Widget* GUI::elem( const kege::ui::WidgetId& widget_id ) const
    {
        if ( _widgets.size() <= widget_id.index)
        {
            kege::Log::error << "out of bound WidgetId index" <<kege::Log::nl;
            return nullptr;
        }
        if ( _widgets[ widget_id.index ].user_id != widget_id.id) return nullptr;
        return &_widgets[ widget_id.index ];
    }

    kege::ui::Widget* GUI::elem( const kege::ui::WidgetId& widget_id )
    {
        if ( _widgets.size() <= widget_id.index)
        {
            kege::Log::error << "out of bound WidgetId index" <<kege::Log::nl;
            return nullptr;
        }
        if ( _widgets[ widget_id.index ].version != widget_id.version ) return nullptr;
        return &_widgets[ widget_id.index ];
    }

    kege::ui::Widget* GUI::elemParent( const kege::ui::WidgetId& widget_id )
    {
        if ( _widgets.size() <= widget_id.index)
        {
            kege::Log::error << "out of bound WidgetId index" <<kege::Log::nl;
            return nullptr;
        }
        if ( _widgets[ widget_id.index ].version != widget_id.version ) return nullptr;
        return &_widgets[ _widgets[ widget_id.index ].parent ];
    }

    const kege::ui::Widget* GUI::at( uint32_t node_id )const
    {
        return &_widgets[ node_id ];
    }

    kege::ui::Widget* GUI::at( uint32_t index )
    {
        return &_widgets[ index ];
    }

    uint32_t GUI::parent( uint32_t index )const
    {
        return _widgets[ index ].parent;
    }

    uint32_t GUI::head( uint32_t index )const
    {
        return _widgets[ index ].head;
    }

    uint32_t GUI::tail( uint32_t index )const
    {
        return _widgets[ index ].tail;
    }

    uint32_t GUI::next( uint32_t index )const
    {
        const ui::Widget& w = _widgets[ index ];
        return w.next;
    }

    uint32_t GUI::prev( uint32_t index )const
    {
        return _widgets[ index ].next;
    }

    uint32_t GUI::count( uint32_t index )const
    {
        return _widgets[ index ].count;
    }

    void GUI::setFont( const kege::ref::Font& font )
    {
        _font = font;
    }

    const kege::ref::Font& GUI::getFont()const
    {
        return _font;
    }

    const kege::InputManager* GUI::getInputManager()const
    {
        return _input_manager;
    }

    kege::GraphicsDevice* GUI::getGraphicsDevice()
    {
        return _graphic_device;
    }

    kege::AssetManager* GUI::getAssetManager()
    {
        return _asset_manager;
    }

    const kege::ui::Cursor& GUI::getCursor()const
    {
        return _cursor;
    }

    kege::ui::Cursor* GUI::cursor()
    {
        return &_cursor;
    }

    double GUI::getTimeDelta()const
    {
        return _dms;
    }

    const kege::ui::Layout& GUI::getLayout( uint32_t i )const
    {
        return _layouts[i];
    }

    kege::ui::Layout& GUI::getLayout( uint32_t i )
    {
        return _layouts[i];
    }

    uint32_t GUI::totalLayouts()const
    {
        return (uint32_t)_layouts.size();
    }

    const kege::ui::Rect& GUI::getRect()const
    {
        return _rect;
    }

    bool GUI::hasHit()const
    {
        return _curr.hit.user_id != 0;
    }

    kege::ui::Record GUI::getHotElem(bool button)
    {
        kege::ui::Record hot = {};
        for (int i = 0; i < _layouts.size(); ++i)
        {
            kege::ui::Record result = _layouts[ i ].getHotElem( button );
            if ( result.user_id != 0 )
            {
                hot = result;
                break;
            }
        }
        return hot;
    }

    uint32_t GUI::find(uint64_t user_id)
    {
        for (int i = 0; i <= _layouts.size(); ++i)
        {
            uint32_t index = _layouts[ i ].find( user_id);
            if ( index != 0 )
            {
                return index;
            }
        }
        return 0;
    }

    void GUI::handleInputEvents()
    {
        _next.hit.user_id = 0;

        // when mouse button not down scan for hot element

        _mouse = _input_manager->getMouse();

        _left_click_state = _mouse->isDown( kege::MouseButtonCode::Left );
        if ( !_left_click_state )
        {
            if ( _click_registered )
            {
                _click_registered = false;
                //_active_index = find( _active );

                if (_next.hot.index != 0 && _next.hot.index < _widgets.size())
                {
                    kege::dvec2 position = _mouse->getClickPosition( kege::MouseButtonCode::Left );
                    if( testPointVsRect( position, _widgets[ _next.pressing.index ].rect ) )
                    {
                        if (_widgets[ _next.pressing.index ].single_click == kege::ui::ClickTrigger::OnRelease)
                        {
                            _next.focus = _next.hit = _next.pressing;
                            return;
                        }
                    }
                }
            }

            _next.hot = getHotElem(false);
            _next.pressing = {};
            //std::cout <<"INDEX: "<< _next.hot.index << " : " << _next.hot.user_id <<"\n";
        }
        else if ( _left_click_state && !_click_registered )
        {
            //_left_click_state = true;
            _click_registered = true;
            _next.hot = getHotElem(true);
            _next.clicks = _mouse->isDoubleClick( kege::MouseButtonCode::Left ) ? 2 : 1;
        }

        if ( _left_click_state )
        {
            if ( _next.pressing.user_id == 0 && _next.hot.user_id != 0 )
            {
                kege::dvec2 position = _mouse->getClickPosition( kege::MouseButtonCode::Left );
                if( testPointVsRect( position, _widgets[ _next.hot.index ].rect ) )
                {
                    _next.clicks = (_mouse->isDoubleClick( kege::MouseButtonCode::Left ) ? 2 : 1);
                    _next.pressing = _next.hot;
                    _next.focus = _next.hot;

                    if (_widgets[ _next.hot.index ].single_click == kege::ui::ClickTrigger::Immediate ||
                        _widgets[ _next.hot.index ].single_click == kege::ui::ClickTrigger::Continuous)
                    {
                        _next.hit = _next.hot;
                    }
                }
            }
            else if ( _next.pressing.user_id != 0 )
            {
                //_active_index = find( _active );
                if (_widgets[ _next.pressing.index ].single_click == kege::ui::ClickTrigger::Continuous ||
                    _widgets[ _next.pressing.index ].double_click == kege::ui::ClickTrigger::Continuous)
                {
                    _next.hit = _next.pressing;
                }
            }
            else
            {
                _next.focus.user_id = 0;
            }
        }
    }

    void GUI::begin( double dms )
    {
        _dms = dms;
        _widget_count = 1;
        _deferred_operations.reset();

        // update current hit
        if ( _next.hit.user_id != 0 )
        {
            _curr.hit = _next.hit;
            _curr.clicks = _next.clicks;
        }
        else
        {
            _curr.hit.user_id = 0;
        }

        // update current focus
        _curr.focus = _next.focus;

        // update current mouse over
        _curr.hot = _next.hot;

        _left_click_down = _left_click_state;

        for (int i=0; i<_layouts.size(); ++i)
        {
            _layouts[i].begin( dms );
        }
    }

    void GUI::end()
    {
        for (int i=0; i<_layouts.size(); ++i)
        {
            _layouts[i].end();
        }
        handleInputEvents();
        _deferred_operations.execute( this );
        //popLayer();
    }

    bool GUI::initialize( const GuiConfig& config )
    {
        const kege::Extent2D& extent = _window->getSize();
        _window_scale_factor = 1.0 / _window->getContentScale();
        _proj_matrix = kege::orthoproj< float >( 0, float(extent.width), 0, -float(extent.height), -200.0, 200.0 );

        _rect.height = extent.height;
        _rect.width = extent.width;
        _rect.x = 0;
        _rect.y = 0;

        _widgets.resize( config.max_widgets );
        _layouts.resize( config.max_layouts );

        kege::string font_file = kege::vfs( config.font_path.c_str() );
        _font = FontCreator::create(_graphic_device, 8, 16, font_file.str());
        if( _font == nullptr )
        {
            kege::Log::error << "Can not load Font from file -> \"" << font_file.c_str() <<"\"" << Log::nl;
            return false;
        }
        //uint64_t font_id = _asset_manager->load< kege::ref::Font >( font_file.c_str() );
        //_font = *_asset_manager->get< kege::ref::Font >( font_id );

        uint32_t index = 0;
        for ( auto& layout : _layouts)
        {
            auto irect = config.layout_rects.find( index );
            kege::ui::Rect rect = (irect == config.layout_rects.end())
            ? _rect : irect->second;

            auto itranform = config.layout_transforms.find( index );
            kege::mat44 tranform = (itranform == config.layout_transforms.end())
            ? kege::mat44( 1.f ) : itranform->second;

            layout.initialize( this, tranform, rect, config.max_layers_per_layout );
        }

        kege::string shader_file = kege::vfs( config.pipeline_filename.c_str() );
        uint64_t pipeline_id = _asset_manager->load< kege::ref::ShaderPipeline >( shader_file.c_str() );
        if( pipeline_id == 0 )
        {
            kege::Log::error << "Can not create GUI ShaderPipeline from file -> \"" << shader_file.c_str() <<"\"" << Log::nl;
            return false;
        }

        //std::vector< kege::ref::Image >* scene_color_images = _asset_manager->fetch< std::vector< kege::ref::Image > >( "postprocess_ldr" );

        uint64_t default_image_handle = _asset_manager->getId< kege::ref::Image >( "default" );
        if ( default_image_handle == 0)
        {
            uint32_t color[] = {0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF};
            ref::Image default_image = _graphic_device->createImage
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
            default_image_handle = _asset_manager->add< kege::ref::Image >( "default", default_image );
        }

        uint64_t sampler_handle = _asset_manager->getId< kege::ref::Sampler >( "sampler-linear-clamp" );
        if( sampler_handle == 0 )
        {
            ref::Sampler linear_sampler = _graphic_device->createSampler
            ({
                .min_filter = Filter::Linear,
                .mag_filter = Filter::Linear,
                .address_mode_u = AddressMode::ClampToEdge,
                .address_mode_v = AddressMode::ClampToEdge,
                .address_mode_w = AddressMode::ClampToEdge,
                .name = "default-sampler",
            });
            sampler_handle = _asset_manager->add< ref::Sampler >( "linear", linear_sampler );
        }


        kege::ref::Sampler sampler = *_asset_manager->get< kege::ref::Sampler >( sampler_handle );
        _mesh_storage_buffers.resize( MAX_FRAMES_IN_FLIGHT );
        _draw_records.resize( MAX_FRAMES_IN_FLIGHT );
        for (int i=0; i<_mesh_storage_buffers.size(); ++i)
        {
            ShaderSetDesc descs[3] =
            {
                ShaderSetDesc
                {
                    ShaderSetBindingDesc
                    {
                        .name = "Instances",
                        .type = kege::BindType::Buffer,
                        .usage = kege::BindingUsage::StorageBuffer,
                        .binding_index = 0,
                        .binding_count = 1,
                        .stages = kege::ShaderStageFlag::Vertex,
                    }
                },
                ShaderSetDesc
                {
                    ShaderSetBindingDesc
                    {
                        .name = "Textures",
                        .type = kege::BindType::Image,
                        .usage = kege::BindingUsage::CombinedImageSampler,
                        .binding_index = 0,
                        .binding_count = 2,
                        .stages = kege::ShaderStageFlag::Fragment,
                        .array_type = kege::ArrayType::Static,
                    }
                },
                ShaderSetDesc
                {
                    ShaderSetBindingDesc
                    {
                        .name = "SceneColor",
                        .type = kege::BindType::Image,
                        .usage = kege::BindingUsage::CombinedImageSampler,
                        .binding_index = 0,
                        .binding_count = 1,
                        .stages = kege::ShaderStageFlag::Fragment,
                        .array_type = kege::ArrayType::None,
                    },
                }
            };
            ref::ShaderResourceBinder resource_binder = _graphic_device->createShaderResourceBinder(3, descs);
            resource_binder->insertSemanticBinding( "SceneColor", kege::GlobalSemantic::SceneColor );


//            Uniform* uniform1 = pipeline->getUniform(0);
//            Uniform* uniform2 = pipeline->getUniform(1);
//            Uniform* uniform3 = pipeline->getUniform(2);
//            Uniform* uniform1 = pipeline->getUniform("Instances");
//            Uniform* uniform2 = pipeline->getUniform("Textures");
//            Uniform* uniform3 = pipeline->getUniform("SceneColor");
//            Binder binder = device->CreateBinder(pipeline, {"Instances", "Textures", "SceneColor"})
//            Binder binder = device->CreateBinder({uniform1, uniform2, uniform3})
//
//             set
//             binding
//             descriptor_set_layout_key

            size_t size = config.max_widgets * sizeof( kege::ui::DrawInstance );
            _mesh_storage_buffers[i] = _graphic_device->createBuffer
            ({
                .size = size,
                .usage = kege::BufferUsages::StorageBuffer,
                .memory_usage = kege::MemoryUsage::CpuToGpu,
                .name = "ui-instance-buffer"
            });

            kege::BufferBindInfo buffer_bind_info;
            buffer_bind_info.buffer = _mesh_storage_buffers[i];
            buffer_bind_info.offset = 0;
            buffer_bind_info.range = size;

            resource_binder->setBinding("UiBuffer", {buffer_bind_info});
            
            kege::ImageBindInfo image_bind_info[3];
            image_bind_info[0].image = *_asset_manager->get< kege::ref::Image >( default_image_handle );
            image_bind_info[0].sampler = sampler;
            image_bind_info[0].layout = ImageLayout::ShaderRead;

            image_bind_info[1] = _font->getImageBindInfo();

            image_bind_info[2].image = *_asset_manager->get< kege::ref::Image >( default_image_handle );
            image_bind_info[2].sampler = sampler;
            image_bind_info[2].layout = ImageLayout::ShaderRead;

            resource_binder->setBinding("Textures", {image_bind_info[0],image_bind_info[1]});
            resource_binder->setBinding("SceneColor", {image_bind_info[2]});
            //resource_binder->setBinding("SceneDepth", {image_bind_info[2]});

            kege::ref::Mesh mesh = new kege::BufferMesh( resource_binder, kege::AABB{});
            _draw_records[i].mesh_id = _asset_manager->add< kege::ref::Mesh >( "gui-mesh", mesh );
            _draw_records[i].pipeline_id = pipeline_id;
            _draw_records[i].material_id = 0;
            _draw_records[i].projection = _proj_matrix;
        }
        return true;
    }

    void GUI::shutdown()
    {
        for( int i=0; i<_mesh_storage_buffers.size(); ++i)
        {
            _asset_manager->erase< kege::ref::Mesh >( _draw_records[i].mesh_id );
            _mesh_storage_buffers[i].clear();
        }
        _mesh_storage_buffers.clear();
        _draw_records.clear();

        _layouts.clear();
    }

    const kege::ui::DrawRecord* GUI::getDrawRecord( uint32_t frame_index )
    {
        frame_index = frame_index % (int)_mesh_storage_buffers.size();

        kege::ui::DrawRecord* record = &_draw_records[ frame_index ];
        record->screen_info = {_rect.width, _rect.height, _window_scale_factor.x, _window_scale_factor.y};
        record->projection = _proj_matrix;
        record->batches.clear();

        uint32_t first_instance = 0;

        kege::ref::Buffer& storage_buffer = _mesh_storage_buffers[ frame_index ];

        _draw_buffer = (kege::ui::DrawInstance*) storage_buffer->map();
        for (int i=0; i<_layouts.size(); ++i)
        {
            const kege::ui::Layout& layout = _layouts[i];
            kege::ui::DrawBatch batch = {};

            renderLayout( &batch, layout, first_instance );
            first_instance = batch.instance_count;

            if ( batch.instance_count != 0 )
            {
                record->batches.push_back( batch );
            }
        }
        
        if ( _cursor.isVisible() )
        {
            kege::ui::DrawBatch& batch = record->batches[ record->batches.size() - 1 ];
            kege::ui::DrawInstance& instance = _draw_buffer[ batch.instance_count++ ];
            instance = {};
            if( _cursor.isSelectionActive() )
            {
                instance.color = _cursor.getCursorColor();
                instance.rect = _cursor.getSelectionRect();
            }
            else
            {
                instance.color = _cursor.getCursorColor();
                instance.rect = _cursor.getSelectionRect();
            }
            instance.clip_rect = _rect;
        }

        storage_buffer->unmap();
        return record;
    }

    void GUI::renderLayout
    (
        kege::ui::DrawBatch* batch,
        const kege::ui::Layout& layout,
        uint32_t first_instance
    )
    {
        batch->push_block_size = (uint32_t) sizeof( layout.getTransform() );
        batch->push_block_stages = kege::ShaderStageFlag::Vertex | kege::ShaderStageFlag::Fragment;
        batch->push_block_offset = 0;
        batch->transform = _proj_matrix;
        batch->first_instance = first_instance;
        batch->instance_count = 0;
        batch->first_element = 0;
        batch->element_count = 4;

        for (int32_t i = 0; i < layout.getLayerCount(); ++i)
        {
            const kege::ui::Layer& layer = layout.getLayer(i);
            for (uint32_t root = layer.getHeadRoot(); root != 0; root = layer.getNextRoot( root ))
            {
                const ui::Widget* widget = at( root );
                if ( kege::ui::checkOverlap( layout.getRect(), widget->rect ))
                {
                    renderWidget( batch, layout, widget, layout.getRect() );
                }
                else
                {
                    //printf("checkOverlap\n");
                }
            }
        }
    }

    void GUI::renderWidget
    (
        kege::ui::DrawBatch* batch,
        const kege::ui::Layout& layout,
        const kege::ui::Widget* widget,
        kege::ui::Rect clip_rect
    )
    {
        kege::ui::DrawInstance& instance = _draw_buffer[ batch->instance_count++ ];
        instance.rect = widget->rect;
        instance.texel = widget->texel;
        instance.border = widget->border;
        instance.clip_rect = clip_rect;
        instance.color = widget->color;
        instance.texr_info = widget->texr_info;

        if ( widget->clip_overflow )
        {
            clip_rect = widget->rect;
            clip_rect.x += widget->padding.left;
            clip_rect.y += widget->padding.above;
            clip_rect.width -= widget->padding.left + widget->padding.right;
            clip_rect.height -= widget->padding.above + widget->padding.below;
        }

        if ( widget->text.ptr && _font )
        {
            kege::vec2 start = { widget->rect.x, widget->rect.y };
            const kege::ui::Padding& padding = ((widget->style) ? widget->style->padding : widget->padding);

            switch ( ((widget->style) ? widget->style->align_text : widget->text.align) )
            {
                case kege::ui::AlignText::Center:
                {
                    start.x += (widget->rect.width  - widget->text.width) * 0.5;
                    start.y += (widget->rect.height - widget->text.height) * 0.5;
                    break;
                }

                case kege::ui::AlignText::Right:
                {
                    if ( widget->text.width != 0 )
                    {
                        start.x += ( widget->rect.width - widget->text.width - padding.right);
                    }
                    if ( widget->text.height != 0 )
                    {
                        start.y +=  widget->text.y + padding.above;
                    }
                    break;
                }

                case kege::ui::AlignText::Left:
                default:
                {
                    if ( widget->text.width != 0 )
                    {
                        start.x += widget->text.x + padding.left;
                    }
                    if ( widget->text.height != 0 )
                    {
                        start.y += widget->text.y + padding.above;
                    }
                    break;
                };
            }

            renderText
            (
                batch,
                start,
                widget->rect.width - padding.left - padding.right,
                widget->text.font_size,
                widget->text.color,
                widget->alignment.wrap.enable,
                widget->text.ptr,
                clip_rect
            );
        }

        if ( clip_rect.width > 0 && clip_rect.height > 0 )
        {
            for ( uint32_t eid = widget->head; eid != 0; eid = _widgets[ eid ].next )
            {
                const kege::ui::Widget* child = &_widgets[ eid ];
                if ( kege::ui::checkOverlap( child->rect, clip_rect ) )
                {
                    renderWidget( batch, layout,child, clip_rect );
                }
                else
                {
                    //printf("checkOverlap\n");
                }
            }
        }
    }

    ui::Extent GUI::renderText
    (
        kege::ui::DrawBatch* batch,
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
        float baseline = font_size - 1;

        for (const char* c = text; 0 < *c && batch->instance_count < _widgets.size(); ++c )
        {
            const kege::Glyph& g = _font->glyphs()[ *c ];
            float fw  = font_size * g.scaled_width;
            float fh = font_size * g.scaled_height;
            max_h = kege::max<float>( max_h, fh );

            length = cursor.x + fw;
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
                kege::ui::DrawInstance& instance = _draw_buffer[ batch->instance_count++ ];
                instance.rect.width = fw;
                instance.rect.height = fh;
                instance.texr_info.id = 1;
                instance.texr_info.index = 1;
                instance.color = color;
                instance.clip_rect = clip_rect;

                instance.rect.x = cursor.x - font_size * g.bearing_x;
                instance.rect.y = cursor.y + baseline - font_size * g.bearing_y;

                instance.texel.x       = g.x;
                instance.texel.y       = g.y;
                instance.texel.width   = g.width;
                instance.texel.height  = g.height;
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

    bool GUI::indexIsValid( uint32_t index ) const
    {
        return (index < _widgets.size());
    }

    kege::ui::Widget* GUI::newWidget()
    {
        if ( !indexIsValid( _widget_count ) )
        {
            kege::Log::error << "exceeding maximum ui widget capacity.";
            return nullptr;
        }

        kege::ui::Widget* widget = &_widgets.at( _widget_count );
        widget->version = widget->version + 1;
        widget->index   = _widget_count;
        widget->parent  = 0;
        widget->count   = 0;
        widget->head    = 0;
        widget->tail    = 0;
        widget->next    = 0;
        widget->prev    = 0;

        widget->layer.next    = 0;
        widget->layer.prev    = 0;
        widget->layer.layer   = 0;

        _widget_count += 1;
        return widget;
    }

    const kege::mat44& GUI::getProjection()const
    {
        return _proj_matrix;
    }

    GUI::~GUI()
    {
        shutdown();
    }

    GUI::GUI( kege::GraphicsDevice* device, kege::AppWindow* window, kege::InputManager* input_manager, kege::AssetManager* asset_manager )
    :   _input_manager( input_manager )
    ,   _asset_manager( asset_manager )
    ,   _graphic_device( device )
    ,   _window( window )
    ,   _cursor( this )
    ,   _widget_count( 0 )
    {
        _mouse = input_manager->getMouse();
    }

}
