//
//  core-vfs.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/14/25.
//

#include "core-vfs.hpp"
namespace kege{

    CoreVFS::CoreVFS( kege::Engine* engine )
    :   kege::CoreSystem< kege::VirtualDirectory >( engine, "core-vfs" )
    {}

    bool CoreVFS::initialize()
    {
        _module = new kege::VirtualDirectory;
        _module->add( "assets", "/Users/kae/Developer/xcode/kege-engine/kege-engine/kege/assets" );
        _module->add( "graphics-shaders", "/Users/kae/Developer/xcode/kege-engine/kege-engine/kege/assets/shaders/glsl/graphics" );
        _module->add( "compute-shaders", "/Users/kae/Developer/xcode/kege-engine/kege-engine/kege/assets/shaders/glsl/compute" );
        return true;
    }

    void CoreVFS::shutdown()
    {
        _module.clear();
    }

    //KEGE_REGISTER_SYSTEM( CoreECS, "ecs" );
}

