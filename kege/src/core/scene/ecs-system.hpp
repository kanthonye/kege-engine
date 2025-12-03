//
//  ecs-system.hpp
//  assets
//
//  Created by Kenneth Esdaile on 11/28/25.
//

#ifndef ecs_system_hpp
#define ecs_system_hpp

#include "scene.hpp"
#include "ecs-entity-registry.hpp"
#include "../utils/communication.hpp"

namespace kege{
    class ECS;
}

namespace kege::ecs{

    class System : public kege::RefCounter
    {
    public:

        enum StateBitFlag
        {
            REQUIRE_UPDATE = 1,
            REQUIRE_RENDER = 2,
            REQUIRE_INPUT  = 4,
        };

        template<typename... C> ecs::ViewT<C...>& view();

        bool checkFlag( StateBitFlag flag ){ return (_flags & flag) == flag; }
        const std::string& getName() const { return _name; }
        virtual void update( double dms ){}
        virtual void render( double dms ){}
        virtual void input( double dms ){}

        kege::Scene* getScene();
        void onSceneChanged();

        virtual bool initialize();
        virtual void shutdown();

        System(kege::ECS* e, std::string n, uint32_t f): _ecs(e), _name(n), _flags(f) {};
        virtual ~System(){};

    protected:

        std::string _name;
        kege::ECS* _ecs;
        uint32_t _flags;
    };

    template<typename... C> ecs::ViewT<C...>& System::view()
    {
        return getScene()->view<C...>();
    }
}

#endif /* ecs_system_hpp */
