
#include "module.hpp"
#include "engine.hpp"

namespace kege{

    const std::string& Module::getName() const
    {
        return _name;
    }

    Module::Module( kege::Engine* engine, const std::string& name )
    :   _engine( engine )
    ,   _name( name )
    {
    }

    Module:: ~Module()
    {
    }

}


namespace kege{

    void ModuleFactory::registerModule( const std::string& name, ModuleFactory::AddModuleFunct func )
    {
        registry[ name ] = std::move( func );
    }

    bool ModuleFactory::addModule( const std::string& name, kege::Engine* engine ) const
    {
        auto it = registry.find(name);
        if ( it != registry.end() )
        {
            it->second( engine );
            return true;
        }

        kege::Log::error << "Module " << name << " does not exist!" << Log::nl;
        return false;
    }

    ModuleFactory& ModuleFactory::instance()
    {
        static ModuleFactory factory;
        return factory;
    }

}