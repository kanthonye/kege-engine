//
//  core-vfs.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/14/25.
//

#include "engine.hpp"
#include "core-vfs.hpp"

namespace kege{

    VirtualDirectoryModule::VirtualDirectoryModule( kege::Engine* engine )
    :   kege::Module( engine, "virtual-directory" )
    {}

    const kege::VirtualDirectory* VirtualDirectoryModule::operator ->()const
    {
        return _module.ref();
    }

    kege::VirtualDirectory* VirtualDirectoryModule::operator ->()
    {
        return _module.ref();
    }
    
    bool VirtualDirectoryModule::initialize()
    {
        if ( _module != nullptr )
        {
            kege::Log::error << "CoreVFS already initialized!" << Log::nl;
            return false;
        }
        
        _module = new VirtualDirectory();
        _module->add( "assets", "/Users/kae/Developer/vscode/kege-engine/kege/assets" );
        _module->add( "graphics-shaders", "/Users/kae/Developer/vscode/kege-engine/kege/assets/shaders/glsl/graphics" );
        _module->add( "compute-shaders", "/Users/kae/Developer/vscode/kege-engine/kege/assets/shaders/glsl/compute" );

        kege::Log::info << "virtual-directory -> initialization success" << Log::nl;
        return true;
    }

    void VirtualDirectoryModule::shutdown()
    {
        _module.clear();
    }

    void VirtualDirectoryModule::add()
    {
        _engine->addModule( this );
        kege::Log::info << "VirtualDirectory module added to engine" << Log::nl;
    }
    
}

