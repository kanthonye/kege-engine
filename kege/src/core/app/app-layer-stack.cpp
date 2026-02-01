//
//  app-layer-stack.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/22/25.
//

#include "app-layer-stack.hpp"

namespace kege{

    bool AppLayerStack::push( ref::AppLayer layer )
    {
        _layers.push_back( layer );
        kege::Log::info << "[ "<< layer->name() <<" ]: initializing...\n";
        if ( !layer->initialize() )
        {
            return false;
        }
        kege::Log::info << "[ "<< layer->name() <<" ]: initialization complete..." << kege::Log::nl;
        return true;
    }

    bool AppLayerStack::initialize()
    {
        for (size_t i = 0; i < _layers.size(); i += 1 )
        {
            if( !_layers[ i ]->initialize() )
            {
                return false;
            }
        }
        return true;
    }

    void AppLayerStack::shutdown()
    {
        for (size_t i = _layers.size() - 1; i > 0; i -= 1 )
        {
            kege::Log::info << "[ "<< _layers[ i ]->name() <<" ]: shutting down...\n";
            _layers[ i ]->shutdown();
            kege::Log::info << "[ "<< _layers[ i ]->name() <<" ]: shutdown complete..." << kege::Log::nl;
        }
        _layers.clear();
    }

    bool AppLayerStack::update()
    {
        for (int64_t i = _layers.size() - 1; 0 <= i; --i )
        {
            if( !_layers[ i ]->update() )
            {
                return false;
            }
        }
        return true;
    }

    AppLayerStack::~AppLayerStack()
    {
        shutdown();
    }
    
    AppLayerStack::AppLayerStack()
    {}

}
