//
//  entity-logic.hpp
//  physics
//
//  Created by Kenneth Esdaile on 9/8/25.
//

#ifndef entity_logic_hpp
#define entity_logic_hpp

#include "../scene/scene.hpp"

namespace kege{

    class Scene;

    class EntityLogic : public kege::RefCounter
    {
    public:

        EntityLogic( const std::string& name );

        const kege::EntitySignature& getEntitySignature()const;
        const std::string& getName() const;
        EntityView* getEntities();

        void setScene( kege::Scene* scene );
        kege::Scene* scene();

        virtual ~EntityLogic();

    protected:

        kege::EntitySignature _signature;
        EntityView* _entities;
        kege::Scene* _scene;

        std::string _name;
    };

}
#endif /* entity_logic_hpp */
