//
//  ecs.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/21/25.
//

#include "entity.hpp"

namespace kege{

    kege::EntityManager* Entity::_mgr;

    kege::EntityManager& Entity::getManager()
    {
        return *_mgr;
    }

    std::ostream& operator <<(std::ostream& os, const Entity& entity)
    {
        return entity._mgr->print( os, entity._id );
    }

    const EntityComponentMap& Entity::getEntityComponents()const
    {
        return _mgr->getEntityComponents( _id );
    }

    const EntitySignature& Entity::signature() const
    {
        return _mgr->signature( _id );
    }

    void Entity::attach( const Entity& child )
    {
        return _mgr->attach( _id, child._id );
    }

    void Entity::detach()
    {
        return _mgr->detach( _id );
    }

    Entity Entity::getParent() const
    {
        return Entity( _mgr->getParent( _id ) );
    }

    Entity Entity::getRoot() const
    {
        return Entity( _mgr->getRoot( _id ) );
    }

    bool Entity::isParent() const
    {
        return _mgr->isParent( _id );
    }

    bool Entity::isChild() const
    {
        return _mgr->isChild( _id );
    }

    Entity Entity::begin() const
    {
        return Entity( _mgr->begin( _id ) );
    }

    Entity Entity::end() const
    {
        return Entity( _mgr->end( _id ) );
    }

    Entity Entity::next() const
    {
        return Entity( _mgr->next( _id ) );
    }

    Entity Entity::prev() const
    {
        return Entity( _mgr->prev( _id ) );
    }

    Entity::operator bool()const
    {
        return _mgr->isvalid( _id );
    }

    bool Entity::isvalid() const
    {
        return _mgr->isvalid( _id );
    }

    uint32_t Entity::getID() const
    {
        return _id;
    }
    
    void Entity::destroy()
    {
        _mgr->destroy( _id );
        _id = 0;
    }

    Entity Entity::create()
    {
        return Entity( _mgr->create() );
    }

    void Entity::setManager( kege::EntityManager* mgr )
    {
        _mgr = mgr;
    }

    Entity::~Entity(){}

    Entity::Entity(uint32_t id): _id( id ) {}

    Entity::Entity(): _id( 0 ) {}

}
