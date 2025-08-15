//
//  rendering-system.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 1/27/25.
//

#include "json-parser.hpp"
#include "render-pass.hpp"
#include "rendering-system.hpp"

namespace kege{

    void RenderingSystem::operator()( const MsgExecuteRenderProcess& pass )
    {
    }

    bool RenderingSystem::initialize()
    {
        _comm.add< const MsgExecuteRenderProcess&, RenderingSystem >( this );
        return System::initialize();
    }

    void RenderingSystem::shutdown()
    {
        _comm.remove< const MsgExecuteRenderProcess&, RenderingSystem >( this );
        System::shutdown();
    }

    RenderingSystem::RenderingSystem( kege::Engine* engine )
    :   kege::System( engine, "rendering-system", 0 )
    {
    }
}
