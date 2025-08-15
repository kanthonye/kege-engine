//
//  ui-canvas.cpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#include "ui-canvas.hpp"

namespace kege::ui{

    float Canvas::getClickToCursorOffset( ui::UIElem& elem, const std::string& text, int font_size, int32_t* cursor, const kege::Font& font )
    {
        float length = 0;

        *cursor = 0;
        float char_width;
        float max_length = clickPosition().x - elem.x;
        for ( const char* s = text.data(); *s != 0; ++s )
        {
            char_width = font->getCharWidth( font_size, *s );
            if ( ((length + char_width) - max_length) > 0.5 )
            {
                break;
            }
            length += char_width;
            *cursor += 1;
        }

        return length;
    }

    bool Canvas::onTextInput( Input::Type type, std::string* text, int32_t* cursor, bool* has_focused )
    {
        return _input.onTextInput( type, text, cursor, has_focused );
    }

    bool Canvas::mouseover( uint32_t id )const
    {
        return _layout->mouseover( id );
    }

    bool Canvas::doubleClick( uint32_t id )const
    {
        return _layout->doubleClick( id );
    }

    bool Canvas::click( uint32_t id )const
    {
        return _layout->click( id );
    }

    void Canvas::setFocus( uint32_t id )
    {
        return _layout->setFocus( id );
    }

    void Canvas::clearFocus()
    {
        //_layout->_click[ CURR ].id = 0;
    }

    void Canvas::updateInputStates( const InputEvents& inputs )
    {
        _input.processInputs( inputs );
    }

    const kege::dvec2& Canvas::releasedPosition()const
    {
        return _input.releasedPosition();
    }

    const kege::dvec2& Canvas::clickPosition()const
    {
        return _input.clickPosition();
    }

    const kege::dvec2& Canvas::previousPosition()const
    {
        return _input.previousPosition();
    }

    const kege::dvec2& Canvas::currentPosition()const
    {
        return _input.currentPosition();
    }

    const kege::dvec2& Canvas::deltaPosition()const
    {
        return _input.deltaPosition();
    }

    const kege::dvec2& Canvas::scrollOffset()const
    {
        return _input.scrollOffset();
    }

    const bool Canvas::pointerDragging()const
    {
        return _input.pointerDragging();
    }

    const bool Canvas::primaryClick()const
    {
        return _input.primaryClick();
    }

    const bool Canvas::doubleClick()const
    {
        return _input.doubleClick();
    }

    bool Canvas::testPointVsRect( const kege::dvec2& p, const ui::Rect2D& rect )const
    {
        return
        (
            p.x > rect.x &&
            p.y > rect.y &&
            p.x < rect.x + rect.width &&
            p.y < rect.y + rect.height
        );
    }

    EID Canvas::make( const Info& info )
    {
        return _layout->make( info );
    }

    UIElem& Canvas::put( const EID& id )
    {
        return _layout->put( id );
    }

    UIElem& Canvas::push( const EID& id )
    {
        return _layout->push( id );
    }

    uint32_t Canvas::pop()
    {
        return _layout->pop();
    }

    const UIElem* Canvas::operator[]( uint32_t node_id )const
    {
        return &_layout->operator[]( node_id );
    }

    UIElem* Canvas::operator[]( uint32_t index )
    {
        return &_layout->operator[]( index );
    }

//    Style& Canvas::styles( uint32_t style_index )
//    {
//        return _layout->styles( style_index );
//    }

    bool Canvas::validate( uint32_t node_id )const
    {
        return _layout->validate( node_id );
    }

    bool Canvas::parent( uint32_t node_id )const
    {
        return _layout->validate( node_id );
    }

    void Canvas::beginLayout( ui::Layout *layout )
    {
        _layouts.push_back( layout );
        _layout = layout;
        _layout->begin( &_input );

    }

    void Canvas::endLayout()
    {
        _layouts.back()->end();
        _layouts.pop_back();
        if ( !_layouts.empty() )
        {
            _layout = _layouts.back();
        }
    }

    bool Canvas::initialize( Graphics* graphics )
    {
        if ( _viewer.initialize( graphics ) )
        {
            return true;
        }
        return false;
    }

    void Canvas::shutdow()
    {
        _viewer.shutdow();
    }

    void Canvas::view( kege::CommandBuffer* command, ui::Layout& layout )
    {
        _viewer.view( command, layout );
    }

    Canvas::Canvas()
    {
    }

}
