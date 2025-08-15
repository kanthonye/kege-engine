//
//  system.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/14/25.
//

#include "engine.hpp"
#include "system.hpp"

namespace kege{

    bool System::checkFlag( StateBitFlag flag )
    {
        return (_requirements & flag) == flag;
    }

    void System::input( const std::vector< Input >& inputs )
    {
    }

    void System::update( double dms )
    {
    }

    void System::render( double dms )
    {
    }

    void System::onSceneChange()
    {
    }

    bool System::initialize()
    {
        return true;
    }

    void System::shutdown()
    {
    }

    void System::setRenderGraph( kege::Ref< kege::RenderGraph > graph )
    {
        _engine->_graph = graph;
    }
    
    void System::setWindow( kege::Ref< kege::GraphicsWindow > window )
    {
        _engine->_window = window;
    }
    
    void System::setGraphics( kege::Ref< kege::Graphics > graphics )
    {
        _engine->_graphics = graphics;
    }

    const std::string& System::getName() const
    {
        return _name;
    }

    System::System( kege::Engine* engine, const std::string& name, uint32_t requirements )
    :   _engine( engine )
    ,   _name( name )
    ,   _requirements( requirements )
    {
    }

    System:: ~System()
    {
    }


}
