//
//  entity-logic.hpp
//  physics
//
//  Created by Kenneth Esdaile on 9/8/25.
//

#ifndef entity_logic_hpp
#define entity_logic_hpp

#include "../ecs/entity.hpp"
#include "../ecs/entity-registry.hpp"

namespace kege{

    class Engine;

    class EntityLogic : public kege::RefCounter
    {
    public:

        EntityLogic( kege::Engine* engine, const std::string& name );
        
        const kege::EntitySignature& getEntitySignature()const;
        const std::string& getName() const;
        EntityView* getEntities();
        void onSceneChange();

        virtual ~EntityLogic();

    protected:

        kege::EntitySignature _signature;
        EntityView* _entities;
        Engine* _engine;
        
        std::string _name;
    };

}
#endif /* entity_logic_hpp */
