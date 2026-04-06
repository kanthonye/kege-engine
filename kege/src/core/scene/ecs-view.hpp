//
//  ecs-view.hpp
//  assets
//
//  Created by Kenneth Esdaile on 11/17/25.
//

#ifndef kege_ecs_view_hpp
#define kege_ecs_view_hpp

#include "ecs-entity-manager.hpp"

namespace kege::ecs{

    struct EntityStorage : public kege::RefCounter
    {
        void push(const Entity& entity)
        {
            if ( count >= entities.size() )
            {
                entities.resize((count == 0)? 64 : count * 2);
            }
            entities[ count ] = entity;
            count += 1;
        }

        void erase(uint32_t index)
        {
            if ( count != 0 )
            {
                entities[ index ] = entities[ count - 1 ];
                count -= 1;
            }
        }

        kege::ecs::Signature signature;
        kege::vector< Entity > entities;
        uint32_t count;
    };
}


namespace kege::ecs{

    class View : public kege::RefCounter
    {
    public:

        View(EntityManager* ecs, kege::vector< EntityStorage* >& storages)
        :   _ecs( ecs )
        ,   _count( 0 )
        {
            _storages.replace( storages );
        }

        void remove( EntityStorage* es )
        {
            for(size_t i=0; i<_count; ++i)
            {
                if(_storages[i] == es)
                {
                    EntityStorage* s = _storages[i];
                    _storages[i] = _storages[_storages.size() - 1];
                    _storages[_storages.size() - 1] = s;
                    _count -= 1;
                    break;
                }
            }
        }

        void add( EntityStorage* es )
        {
            if (_count >= _storages.size())
            {
                _storages.resize(2 * _storages.size() + 1);
            }
            _storages[ _count++ ] = es;
        }

        virtual ~View() = default;

        kege::vector< EntityStorage* > _storages;
        EntityManager* _ecs;
        int _count;
    };
}


namespace kege::ecs{

    template<typename... C> class ViewT : public View
    {
    public:

        class iterator
        {
        public:

            iterator(ViewT<C...>* view, int32_t index, int32_t list_index )
            :   _view(view)
            ,   _list_index(list_index)
            ,   _index(index)
            ,   _end(0)
            {
                if(list_index < _view->_storages.size())
                    _end = _view->_storages[ _view->_storages.size() - 1 ]->count;
            }

            iterator()
            :   _view(nullptr), _index(0), _list_index(0)
            {
            }

            std::tuple<const Entity&, const C*...> operator*() const
            {
                if ( !valid() ) return std::tuple<Entity, C*...>{};
                Entity entity = _view->_storages[ _list_index ]->entity[ _index ];
                return _view->_ecs->fetch<C...>( entity );
            }

            inline std::tuple<Entity, C*...> operator*()
            {
                if ( !valid() ) return std::tuple<Entity, C*...>{};
                Entity entity = _view->_storages[ _list_index ]->entities[ _index ];
                return _view->_ecs->fetch<C...>( entity );
            }

            inline bool operator!=(const iterator& other) const
            {
                return _list_index != other._list_index || _index != other._index;
            }

            iterator& operator++()
            {
                //if ( !valid() ) return *this;
                _index += 1;

                if (_index >= _end)
                {
                    _list_index += 1;
                    if (_list_index < _view->_storages.size())
                    {
                        _end = (int32_t)_view->_storages[ _list_index ]->count;
                        _index = 0;
                    }
                }
                return *this;
            }

            inline bool valid() const
            {
                return (_list_index < _view->_storages.size())
                ? _index <= _end
                : false;
            }

        private:

            ViewT<C...>* _view;
            int32_t _list_index;
            int32_t _index;
            int32_t _end;
        };

        iterator begin()
        {
            return iterator(this, 0, 0);
        }

        iterator end()
        {
            if(_storages.empty()) return iterator(this, 0, 0);
            return iterator(this, _storages[ _storages.size() - 1 ]->count, (int32_t)_storages.size());
        }

        ViewT(EntityManager* ecs, kege::vector< EntityStorage* >& storages)
        :   View( ecs, storages )
        {}
    };

}


#endif /* kege_ecs_view_hpp */
