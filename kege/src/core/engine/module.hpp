

#ifndef kege_module_hpp
#define kege_module_hpp

#include <string>
#include "../memory/ref.hpp"
#include "../utils/log.hpp"
#include "../utils/communication.hpp"

namespace kege{

    class Engine;

    class Module: public kege::RefCounter
    {
    public:

        /**
         * Constructor for the Module class.
         * @param engine Pointer to the Engine instance that this module belongs to.
         * @param name The name of the module.
         */
        explicit Module( kege::Engine* engine, const std::string& name );

        /** 
         * Get the name of the module.
         * @return The name of the module.
         */
        const std::string& getName() const;
        /**
         * Initialize the module.
         * @return True if initialization was successful, false otherwise.
         */
        virtual bool initialize() = 0;

        virtual operator bool()const = 0;

        /**
         * Shutdown the module.
         * This is called when the module is no longer needed.
         */
        virtual void shutdown() = 0;

        virtual void add() = 0;

        virtual~ Module();

    public:

        Module& operator=(const Module&) = delete;
        Module(const Module&) = delete;

    protected:

        kege::Communication _comm;
        kege::Engine* _engine;
        std::string _name;

        friend kege::Engine;
    };

}

namespace kege{

    class ModuleFactory
    {
    public:

        using AddModuleFunct = std::function< void( kege::Engine* ) >;

        bool addModule( const std::string& name, kege::Engine* engine ) const;
        void registerModule( const std::string& name, ModuleFactory::AddModuleFunct func );
        
        static ModuleFactory& instance();

    private:

        std::unordered_map< std::string, ModuleFactory::AddModuleFunct > registry;
    };

}


#define KEGE_REGISTER_MODULE( NAME, FUNCTNAME )                         \
struct AutoRegisterModule##FUNCTNAME                                    \
{                                                                       \
    AutoRegisterModule##FUNCTNAME()                                     \
    {                                                                   \
        SystemFactory::instance().registerSystem( NAME, FUNCTNAME );    \
    }                                                                   \
};                                                                      \
static AutoRegisterModule##FUNCTNAME register_##FUNCTNAME;              \


#endif
