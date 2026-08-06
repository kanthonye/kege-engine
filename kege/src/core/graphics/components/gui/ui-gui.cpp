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
        float text_height = font_size;
        computeExtent(font_size, str, text.width, text_height);
        text.font_size = font_size;
        text.data = str;
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

//    kege::ui::NodeId GUI::pushRootStyle( const kege::ui::Style& style )
//    {
//        kege::ui::Node* node = newNode({ .wid = newElem({ .style = style }) });
//        return _layouts[ _layout_index ].pushRoot( node );
//    }
//
//    kege::ui::NodeId GUI::putRootStyle( const kege::ui::Style& style )
//    {
//        kege::ui::Node* node = newNode({ .wid = newElem({ .style = style }) });
//        return _layouts[ _layout_index ].putRoot( node );
//    }
//    kege::ui::NodeId GUI::pushStyle( const kege::ui::Style& style )
//    {
//        kege::ui::Node* node = newNode({ .wid = newElem({ .style = style }) });
//        return _layouts[ _layout_index ].push( node );
//    }
//    kege::ui::NodeId GUI::putStyle( const kege::ui::Style& style )
//    {
//        kege::ui::Node* node = newNode({ .wid = newElem({ .style = style }) });
//        return _layouts[ _layout_index ].put( node );
//    }

    void GUI::beginRoot()
    {
        _layouts[ _layout_index ].beginRoot();
    }

    void GUI::endRoot()
    {
        _layouts[ _layout_index ].endRoot();
    }

    kege::ui::NodeId GUI::push( const kege::ui::NodeDesc& desc )
    {
        return _layouts[ _layout_index ].push( newNode( desc ) );
    }

    uint32_t GUI::pop()
    {
        return _layouts[ _layout_index ].pop();
    }

    kege::ui::NodeId GUI::put( const kege::ui::NodeDesc& desc )
    {
        return _layouts[ _layout_index ].put( newNode( desc ) );
    }

    kege::ui::NodeId GUI::text( const kege::ui::Text& text )
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

//    kege::ui::Widget* GUI::elemParent( const kege::ui::NodeId& node_id )
//    {
//        if ( _nodes.size() <= node_id.index)
//        {
//            kege::Log::error << "out of bound NodeId index" <<kege::Log::nl;
//            return nullptr;
//        }
//        if ( _nodes[ node_id.index ].version != node_id.version ) return nullptr;
//        return &_nodes[ _nodes[ node_id.index ].parent ];
//    }

    const kege::ui::Elem* GUI::elem( const kege::ui::Node* node ) const
    {
        return &_elements[ node->wid.index ];
    }

    kege::ui::Elem* GUI::elem( const kege::ui::Node* node )
    {
        return &_elements[ node->wid.index ];
    }

    const kege::ui::Elem* GUI::elem( const kege::ui::NodeId& id ) const
    {
        return &_elements[ _nodes[ id.index ].wid.index ];
    }

    kege::ui::Elem* GUI::elem( const kege::ui::NodeId& id )
    {
        return &_elements[ _nodes[ id.index ].wid.index ];
    }

    const kege::ui::Node* GUI::at( const kege::ui::NodeId& id )const
    {
        return &_nodes[ id.index ];
    }

    kege::ui::Node* GUI::at( const kege::ui::NodeId& id )
    {
        return &_nodes[ id.index ];
    }

    uint32_t GUI::parent( uint32_t index )const
    {
        return _nodes[ index ].parent;
    }

    uint32_t GUI::head( uint32_t index )const
    {
        return _nodes[ index ].head;
    }

    uint32_t GUI::tail( uint32_t index )const
    {
        return _nodes[ index ].tail;
    }

    uint32_t GUI::next( uint32_t index )const
    {
        return _nodes[ index ].next;
    }

    uint32_t GUI::prev( uint32_t index )const
    {
        return _nodes[ index ].prev;
    }

    uint32_t GUI::count( uint32_t index )const
    {
        return _nodes[ index ].count;
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

    ui::Record GUI::getHitRecord()const
    {
        return _curr.hit;
    }

    ui::Record GUI::getHotRecord()const
    {
        return _curr.hot;
    }

    ui::ID GUI::getHitId()const
    {
        return _nodes[ _curr.hit.index ].user_id;
    }

    bool GUI::hasHit()const
    {
        return _curr.hit.user_id != 0;
    }

//    kege::ui::Record GUI::getHotElem( bool button )
//    {
//        for (int layer_index = int(_layers.size() - 1); 0 <= layer_index; --layer_index)
//        {
//            kege::ui::Record result = _layers[ layer_index ].getHotElem( button );
//            if ( result.user_id != 0 )
//            {
//                return result;
//            }
//        }
//        return {};
//    }
    kege::ui::Record GUI::getHotElem(uint32_t node_index, bool button)
    {
        const kege::ui::Node* node = &_nodes[ node_index ];
        if ( !node->visible || node->inactive )
            return {};

        for (uint32_t child_index = _nodes[ node_index ].head; child_index != 0; child_index = _nodes[ child_index ].next)
        {
            kege::ui::Record record = getHotElem( child_index, button );
            if ( record.user_id != 0 )
            {
                return record;
            }
        }

        if ( node->quad.width == 0.f || node->quad.height == 0.f )
            return {};

        if (button)
        {
            if (node->single_click == kege::ui::ClickTrigger::Disable &&
                node->double_click == kege::ui::ClickTrigger::Disable)
            {
                return {};
            }
        }

        if ( !node->mouseover )
            return {};

        if ( testPointVsRect( getPointerPosition(), node->quad ) )
        {
            return kege::ui::Record
            {
                .user_id = node->user_id.value,
                .index = node_index,
            };
        }
        return {};
    }

    kege::ui::Record GUI::getHotElem(bool button)
    {
        kege::ui::Record hot = {};
        for (int i = 0; i < _layouts.size(); ++i)
        {
            kege::ui::Record result = getHotElem( _layouts[i]._head, button );
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
        _mouse = _input_manager->getMouse();
        _left_click_state = _mouse->isDown( kege::MouseButtonCode::Left );

        // when mouse left button is not down, scan for hot element
        if ( !_left_click_state )
        {
            // if a click was registered in the previous frame and left mouse
            // button is released this frame, process click on-release selection.
            if ( _click_registered )
            {
                _click_registered = false;
                if (_next.hot.index != 0 && _next.hot.index < _nodes.size())
                {
                    kege::dvec2 position = _mouse->getClickPosition( kege::MouseButtonCode::Left );
                    if( testPointVsRect( position, _nodes[ _next.pressing.index ].quad ) )
                    {
                        if (_nodes[ _next.pressing.index ].single_click == kege::ui::ClickTrigger::OnRelease)
                        {
                            _next.focus = _next.hit = _next.pressing;
                            return;
                        }
                    }
                }
            }

            // then handle getting the next hot element.
            _next.hot = getHotElem( _click_registered );
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
                if( testPointVsRect( position, _nodes[ _next.hot.index ].quad ) )
                {
                    _next.clicks = (_mouse->isDoubleClick( kege::MouseButtonCode::Left ) ? 2 : 1);
                    _next.pressing = _next.hot;
                    _next.focus = _next.hot;

                    if (_nodes[ _next.hot.index ].single_click == kege::ui::ClickTrigger::Immediate ||
                        _nodes[ _next.hot.index ].single_click == kege::ui::ClickTrigger::Continuous)
                    {
                        _next.hit = _next.hot;
                    }
                }
            }
            else if ( _next.pressing.user_id != 0 )
            {
                //_active_index = find( _active );
                if (_nodes[ _next.pressing.index ].single_click == kege::ui::ClickTrigger::Continuous ||
                    _nodes[ _next.pressing.index ].double_click == kege::ui::ClickTrigger::Continuous)
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

    bool GUI::isMouseDragging() const
    {
        return _drag_state;
    }

    bool GUI::isSelected(kege::ui::ID id) const
    {
        return ( _selected.empty() ) ? false : _selected.find( id ) != _selected.end();
    }

    void GUI::updateSelectionList()
    {
        /**
         * The following code block is responsible for updating the selection list 
         * based on a selection rectangle. It checks if there are any elements in 
         * the `_marquee_hit_list_test` vector, which contains the IDs of UI elements 
         * that needs to be tested for selection via a marquee (selection rectangle). 
         * If there are elements in this list, it checks if the left mouse button is down and 
         * if the selection is being updated. If so, it clears the current selection. Then, 
         * it iterates through each element in the `_marquee_hit_list_test`, retrieves the 
         * corresponding node, and checks if its quad overlaps with the `_marquee_rect`. 
         * If there is an overlap and the element is not already selected, it adds the element's 
         * user ID to the `_selected` set. Finally, it clears the `_marquee_hit_list_test` 
         * vector to prepare for the next update cycle.
         */
        if ( !_marquee_hit_list_test.empty() )
        {
            if( leftClickDown() && _updating_selection )
                _selected.clear();
            for (kege::ui::NodeId node_id : _marquee_hit_list_test)
            {
                ui::Node* node = at(node_id);
                if (ui::checkOverlap(node->quad, _marquee_rect))
                {
                    auto it = _selected.find(node->user_id);
                    if ( it == _selected.end() )
                    {
                        _selected.insert( node->user_id );
                    }
                }
            }
            _marquee_hit_list_test.clear();
        }

        /**
         * The following code block is responsible for updating the selection list based on 
         * user input events, specifically mouse clicks. It checks if the left mouse button 
         * is down and if the selection is not currently being updated. If both conditions 
         * are met, it sets the `_updating_selection` flag to true. Then, it checks if there 
         * is a hit (i.e., a UI element clicked on). If there is a hit and the element is 
         * selectable, it checks if the element's user ID is already in the `_selected` set. 
         * If it is not in the set, it adds it to the selection. If the Shift or Control modifier 
         * keys are held down during the click, it allows for multi-selection by adding or 
         * removing elements from the selection set accordingly. If there is no hit, it clears 
         * the selection. Finally, if the left mouse button is released while updating selection, 
         * it resets the `_updating_selection` flag to false. This logic enables both single and 
         * multi-selection of UI elements based on user interactions.
         */
        if( leftClickDown() && !_updating_selection )
        {
            _updating_selection = true;
            if( hasHit() )
            {
                if ( _nodes[ _curr.hit.index ].selectable )
                {
                    auto it = _selected.find(getHitId());
                    Modifiers mods = Modifiers::Shift | Modifiers::Control;
                    // if item is not in list add it
                    if ( it == _selected.end() )
                    {
                        if ( contain( _input_manager->getMouse()->getModifiers(), mods ) )
                        {
                            _selected.insert( getHitId() );
                        }
                        else
                        {
                            _selected.clear();
                            _selected.insert( getHitId() );
                        }
                    }
                    // else if item already exist and shift is down them remove item
                    else if( contain(_input_manager->getMouse()->getModifiers(), mods) )
                    {
                        _selected.erase(it);
                    }
                }
            }
            else
            {
                _selected.clear();
            }
        }
        else if ( !leftClickDown() && _updating_selection )
        {
            _updating_selection = false;
        }
    }

    void GUI::updateDragState()
    {
        if ( _curr.hit.user_id == 0 && leftClickDown() && _input_manager->getMouse()->moved() )
        {
            _drag_state = true;
        }
        else if ( !leftClickDown() && _drag_state )
        {
            _drag_state = false;
        }
    }

    const kege::ui::Rect& GUI::getSelectionRect()const
    {
        return _marquee_rect;
    }

    bool GUI::isCastingSelectionRect(const kege::ui::ID& id)
    {
        /** 
         * The following code block is responsible for determining if a selection rectangle 
         * (marquee) is currently being cast (drawn) for a specific UI element identified by 
         * its user ID. It checks if the drag state is active and if the next hot element's 
         * user ID matches the provided ID. If both conditions are met, it checks if the 
         * marquee is not already occupied. If it is not occupied, it sets the marquee as 
         * occupied by the provided ID and records the starting position of the marquee based 
         * on the current mouse position. It also sets a flag indicating that selection is 
         * being updated. Finally, it returns whether the marquee is currently occupied by 
         * the provided ID, allowing other parts of the code to know if a selection rectangle 
         * is being cast for that specific UI element.
         */
        if( _drag_state == true && _next.hot.user_id == id.value )
        {
            if( _marquee_is_occupied.value == 0 )
            {
                _marquee_is_occupied = id;
                _marquee_start.x = _input_manager->getMouse()->getPosition().x;
                _marquee_start.y = _input_manager->getMouse()->getPosition().y;
            }
            _marquee_is_occupied = id;
            _updating_selection = true;
        }
        return _marquee_is_occupied == id;
    }

    void GUI::trySelectByRect( const kege::ui::NodeId& node_id )
    {
        ui::Node* node = at(node_id);
        if ( node && _marquee_is_occupied.value != 0 )
        {
            _marquee_hit_list_test.push_back( node_id );
        }
    }

    void GUI::updateSelectionRect()
    {
        /**
         * The following code block is responsible for updating the selection rectangle 
         * (marquee) based on user input events, specifically mouse dragging. It checks 
         * if the marquee is currently occupied (i.e., a selection is being made) and if 
         * the drag state is active. If both conditions are met, it calculates the current 
         * position of the mouse and updates the dimensions of the selection rectangle 
         * accordingly. The rectangle's position and size are adjusted to ensure that it 
         * remains within the bounds of the initial marquee area. If the drag state is no 
         * longer active but the marquee is still occupied, it resets the marquee state 
         * and clears the selection rectangle.
         */
        if( _marquee_is_occupied.value != 0 && _drag_state == true )
        {
            float x,y;
            _marquee_bound = _nodes[ _next.hit.index ].quad;

            x = kege::max<float>( _marquee_bound.x, _input_manager->getMouse()->getPosition().x );
            y = kege::max<float>( _marquee_bound.y, _input_manager->getMouse()->getPosition().y );
            x = kege::min<float>( _marquee_bound.x + _marquee_bound.width, x );
            y = kege::min<float>( _marquee_bound.y + _marquee_bound.height, y );

            _marquee_rect.x = kege::min<float>(_marquee_start.x, x);
            _marquee_rect.y = kege::min<float>(_marquee_start.y, y);
            _marquee_rect.width = abs(_marquee_start.x - x);
            _marquee_rect.height = abs(_marquee_start.y - y);
        }
        else if( !_drag_state && _marquee_is_occupied.value != 0 )
        {
            _marquee_is_occupied.value = 0;
            _marquee_rect = {};
        }

        /**
         * Finally, if the marquee is occupied, it renders a visual representation 
         * of the selection rectangle on top of other UI elements by pushing a new 
         * layer and drawing a semi-transparent quad that represents the selection 
         * area.
         */
        if (_marquee_is_occupied.value != 0)
        {
            pushLayer(kege::ui::LAYER_DRAGGING_OVERLAY);
            beginRoot();
            put
            ({
                .quad =
                {
                    .x = _marquee_rect.x,
                    .y = _marquee_rect.y,
                    .width = _marquee_rect.width,
                    .height = _marquee_rect.height,
                    .color = 0xffffff20,
                }
            });
            endRoot();
            popLayer();
        }
    }

    void GUI::begin( double dms )
    {
        _dms = dms;
        _widget_count = 1;
        _node_count = 0;
        _elem_count = 0;
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
        updateSelectionRect();
        updateSelectionList();
        updateDragState();
        _deferred_operations.execute( this );
        //popLayer();
    }

//    bool GUI::onTextInput(const ui::NodeId& node_id, char* str, size_t& size)
//    {
//        return _cursor.onInput(Cursor::InputType::Any, uid, _font, str, size);
//    }

    bool GUI::initialize( const GuiConfig& config )
    {
        const kege::Extent2D& extent = _window->getSize();
        _window_scale_factor = 1.0 / _window->getContentScale();
        _proj_matrix = kege::orthoproj< float >( 0, float(extent.width), 0, -float(extent.height), -200.0, 200.0 );

        _rect.height = extent.height;
        _rect.width = extent.width;
        _rect.x = 0;
        _rect.y = 0;

        _elements.resize( config.max_widgets );
        _nodes.resize( config.max_widgets );
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

        const ui::Node* node = nullptr;
        for (int32_t i = 0; i < layout.getLayerCount(); ++i)
        {
            const kege::ui::Layer& layer = layout.getLayer(i);
            for (uint32_t node_index = layer._head; node_index != 0; node_index = _nodes[ node_index ].layer.next)
            {
                node = &_nodes.at( node_index );
                if ( kege::ui::checkOverlap( layout.getRect(), node->quad ))
                {
                    renderWidget( batch, layout, node, layout.getRect() );
                }
                else
                {
                    //printf("checkOverlap\n");
                }
            }
        }
    }

    void GUI::renderLayer
    (
        kege::ui::DrawBatch* batch,
        const kege::ui::Layout& layout,
        const kege::ui::Layer& layer,
        uint32_t first_instance
    )
    {
        const ui::Node* node = nullptr;
        for (uint32_t root = layer.getHeadRoot(); root != 0; root = layer.getNextRoot( root ))
        {
            node = &_nodes.at( root );
            if ( kege::ui::checkOverlap( layout.getRect(), node->quad ))
            {
                renderWidget( batch, layout, node, layout.getRect() );
            }
            else
            {
                //printf("checkOverlap\n");
            }
        }
    }

    void GUI::renderWidget
    (
        kege::ui::DrawBatch* batch,
        const kege::ui::Layout& layout,
        const kege::ui::Node* node,
        kege::ui::Rect clip_rect
    )
    {
        kege::ui::DrawInstance& instance = _draw_buffer[ batch->instance_count++ ];
        kege::ui::Elem* elem = &_elements[ node->wid.index ];
        instance.rect.x = node->quad.x;
        instance.rect.y = node->quad.y;
        instance.rect.width = node->quad.width;
        instance.rect.height = node->quad.height;
        instance.color = isSelected(node->user_id)? 0xFFFFFF30 : node->quad.color;

        instance.clip_rect.x = clip_rect.x;
        instance.clip_rect.y = clip_rect.y;
        instance.clip_rect.width = clip_rect.width;
        instance.clip_rect.height = clip_rect.height;

        instance.texel.x = elem->texr_info.x;
        instance.texel.y = elem->texr_info.y;
        instance.texel.width = elem->texr_info.width;
        instance.texel.height = elem->texr_info.height;

        instance.border.radius = elem->corner_curves;
        instance.border.width = elem->border.width;
        instance.border.color = elem->border.color;
        //instance.clip_rect = clip_rect;
        instance.texr_info = elem->texr_id;

        if ( node->clip_overflow )
        {
            clip_rect = node->quad;
            clip_rect.x += elem->padding.left;
            clip_rect.y += elem->padding.above;
            clip_rect.width -= elem->padding.left + elem->padding.right;
            clip_rect.height -= elem->padding.above + elem->padding.below;
        }

        if ( node->text && _font )
        {
            kege::vec2 start = { node->quad.x, node->quad.y };
            const kege::ui::Padding& padding = elem->padding;

//            switch ( widget->text_align )
//            {
//                case kege::ui::AlignText::Center:
//                {
//                    //start.x += (widget->rect.width  - widget->text.width) * 0.5;
//                    //start.y += (widget->rect.height - widget->text.height) * 0.5;
//                    break;
//                }
//
//                case kege::ui::AlignText::Right:
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
//                case kege::ui::AlignText::Left:
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

            renderText
            (
                batch,
                start,
                node->quad.width - padding.left - padding.right,
                node->text.font_size,
                node->text.color,
                elem->text_wrap,
                node->text.data,
                clip_rect
            );
        }

        if ( clip_rect.width > 0 && clip_rect.height > 0 )
        {
            for ( uint32_t node_index = node->layer.head; node_index != 0; node_index = _nodes[ node_index ].layer.next )
            {
                const kege::ui::Node* child = &_nodes[ node_index ];
                if ( kege::ui::checkOverlap( child->quad, clip_rect ) )
                {
                    renderWidget( batch, layout, child, clip_rect );
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

        for (const char* c = text; 0 < *c && batch->instance_count < _nodes.size(); ++c )
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
        return (index < _nodes.size());
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

    kege::ui::WID GUI::newElem( const kege::ui::Elem& elem )
    {
        if ( !indexIsValid( _node_count ) )
        {
            kege::Log::error << "exceeding maximum ui widget capacity.";
            return {0};
        }

        //uint32_t elem_index = _elem_count;
        _elem_count += 1;
        
        kege::ui::Elem* e = &_elements.at( _elem_count );
        memcpy(e, &elem, sizeof( kege::ui::Elem ));
        e->wid.index = _elem_count;
        //e->text = elem.text;

        return e->wid;
    }

    kege::ui::Node* GUI::newNode( const kege::ui::NodeDesc& desc )
    {
        if ( !indexIsValid( _node_count ) )
        {
            kege::Log::error << "exceeding maximum ui widget capacity.";
            return nullptr;
        }

        _node_count += 1;
        
        kege::ui::Node* node = &_nodes.at( _node_count );
        node->version = node->version + 1;
        node->index   = _node_count;
        node->parent  = 0;
        node->count   = 0;
        node->head    = 0;
        node->tail    = 0;
        node->next    = 0;
        node->prev    = 0;
        node->name    = desc.name;

        node->layer = {};

        node->wid = desc.wid;
        node->user_id = desc.user_id;
        node->quad = desc.quad;
        node->text = desc.text;
        node->single_click = desc.single_click;
        node->double_click = desc.double_click;
        node->mouseover = desc.mouseover;
        node->inactive = desc.inactive;
        node->visible = desc.visible;
        node->selectable = desc.selectable;
        node->clip_overflow = desc.clip_overflow;

        if( desc.wid.index != 0 )
        {
            kege::ui::Elem& elem = _elements[ desc.wid.index ];
            if( elem.quad_color.has )
                node->quad.color = elem.quad_color.value;
            
            if( elem.text_color.has )
                node->text.color = elem.text_color.value;

            if( elem.width.type == ui::SizingType::Fixed )
                node->quad.width = elem.width.size;
            
            if( elem.height.type == ui::SizingType::Fixed )
                node->quad.height = elem.height.size;
        }
        return node;
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
    ,   _updating_selection( false )
    ,   _drag_state( false )
    {
        _mouse = input_manager->getMouse();
    }

}
