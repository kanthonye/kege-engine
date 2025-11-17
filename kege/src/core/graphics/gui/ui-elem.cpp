//
//  ui-elem.cpp
//  editor
//
//  Created by Kenneth Esdaile on 11/14/25.
//

#include "ui-elem.hpp"
#include "ui-layout.hpp"

namespace kege::ui{

    bool operator ==( const Elem& a, const Elem& b )
    {
        return a._handle.value == b._handle.value;
    }

    bool operator !=( const Elem& a, const Elem& b )
    {
        return a._handle.value != b._handle.value;
    }

    const kege::ui::Widget* Elem::operator->() const
    {
        return &_layout->_widget_manager[ _handle ];
    }

    /**
     * Retrieves a UI element by its index (non-const version).
     *
     * @return The UI element at the specified index.
     */
    kege::ui::Widget* Elem::operator->()
    {
        return &_layout->_widget_manager[ _handle ];
    }

    Elem& Elem::operator =(Elem&& other) noexcept
    {
        _handle = other._handle;
        _layout = other._layout;
        //node_index = other.node_index;

        other._handle = {};
        //other.node_index = 0;
        other._layout = nullptr;

        return *this;
    }

    Elem& Elem::operator =( const Elem& other )
    {
        _handle = other._handle;
        _layout = other._layout;
        //node_index = other.node_index;
        _layout->_widget_manager.duplicate( other._handle );
        return *this;
    }

    Elem::operator size_t()const
    {
        return _handle.value;
    }

    Elem::operator bool()const
    {
        return 0 != _handle.value;
    }

    Elem::Elem( Elem&& other ) noexcept
    :   _handle( other._handle )
    ,   _layout( other._layout )
    {
        other._handle = {};
        //other.node_index = 0;
        other._layout = nullptr;
    }

    Elem::Elem( Handle handle, Layout* layout )
    :   _handle( handle )
    ,   _layout( layout )
    {}

    Elem::Elem( const Elem& other )
    :   _handle( other._handle )
    ,   _layout( other._layout )
    //,   node_index( other.node_index )
    {
        _layout->_widget_manager.duplicate( other._handle );
    }

    Elem::Elem()
    :   _layout( nullptr )
    ,   _handle({})
    {}

    Elem::~Elem()
    {
        if( _layout )
        {
            _layout->_widget_manager.recycle( _handle );
            _layout = nullptr;
        }
    }

}
