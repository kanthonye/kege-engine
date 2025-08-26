//
//  ui-canvas.cpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#include "ui-canvas.hpp"
//
//namespace kege::ui{
//
//
//
//    void Canvas::begin( ui::Layout* layout )
//    {
//        _layout_stack.push_back( layout );
//        _layout = layout;
//        _layout->begin( &_input );
//    }       
//    void Canvas::end()
//    {
//        if ( !_layout_stack.empty() )
//        {
//            _layout->end();
//            _layout_stack.pop_back();
//            if ( !_layout_stack.empty() )
//            {
//                _layout = _layout_stack.back();
//            }
//        }
//    }
//
//    const Elem* Canvas::operator[]( const EID& id )const
//    {
//        if ( id.index >= _layout->_nodes.size() )
//        {
//            KEGE_LOG_ERROR << "Invalid node index: " << id.index;
//            return nullptr;
//        }
//        return &_layout->_nodes[ id.index ].elem;
//    }
//
//    Elem* Canvas::operator[]( const EID& id )
//    {
//        if ( id.index >= _layout->_nodes.size() )
//        {
//            KEGE_LOG_ERROR << "Invalid node index: " << id.index;
//            return nullptr;
//        }
//        return &_layout->_nodes[ id.index ].elem;
//    }
//
//    void Canvas::view( kege::CommandBuffer* command, ui::Layout& layout )
//    {
//        _viewer.view( command, layout );
//    }
//
//    bool Canvas::initialize( Graphics* graphics, kege::PipelineHandle pipeline, kege::Font font )
//    {
//        if ( _viewer.initialize( graphics, pipeline, font ) )
//        {
//            return true;
//        }
//        return false;
//    }
//
//    void Canvas::shutdow()
//    {
//        _layout = nullptr;
//        _layout_stack.clear();
//        _viewer.shutdow();
//    }
//
//    Canvas::~Canvas()
//    {
//        shutdow();
//    }
//
//    Canvas::Canvas()
//    :   _layout( nullptr )
//    {
//    }
//
//}
