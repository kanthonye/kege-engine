//
//  id-manager.cpp
//  gui
//
//  Created by Kenneth Esdaile on 8/8/25.
//

#include "id-manager.hpp"

namespace kege::ui{

    // Generates a new ID from the pool.
    void IDManager::dup( const ID* src, ID* dst )
    {
        if ( 1 <= src->index )
        {
            dst->index = src->index;
            _pool[ src->index ].duplicates++;
            //std::cout << "duplicated: " << src->index <<"\n";
        }
    }

    ID IDManager::gen()
    {
        if (_free_list_head == -1)
        {
            // No more free IDs available! Handle error appropriately.
            // For now, let's throw or return an invalid ID.
            return {};
        }
        // Take the ID from the head of the free list.
        int index = _free_list_head;

        // Move the head to the next free node.
        _free_list_head = _pool[ index ].id.index;
        _pool[ index ].duplicates = 0;
        //std::cout << "create: " << index <<"\n";

        return {this, index};
    }

    // User can specify the max number of dynamic IDs needed.
    IDManager::IDManager(size_t pool_size)
    {
        _pool.resize(pool_size);
        // Initialize the free list: each node points to the next.
        for (int32_t i = 0; i < pool_size - 1; ++i)
        {
            _pool[i].id.index = i + 1;
        }
        _pool[pool_size - 1].id.index = -1; // Last node is the end.
        _free_list_head = 1; // The first available ID is 0.
    }

    void IDManager::release( ID* id )
    {
        if (id->index < 0 || id->index >= _pool.size()) return; // Invalid ID
        // Add the released ID to the front of the free list.
        if ( _pool[ id->index ].duplicates >= 0 )
        {
            _pool[ id->index ].duplicates--;
            if ( _pool[ id->index ].duplicates < 0 )
            {
                //std::cout << "freed: " << id->index <<"\n";
                _pool[ id->index ].id.index = _free_list_head;
                _free_list_head = id->index;
            }
        }
    }



    ID& ID::operator =( ID&& other ) noexcept
    {
        // Self-assignment check
        if (this != &other)
        {
            // Release our current resource before taking the new one.
            if (mgr) {
                mgr->release(this);
            }

            // Steal the resources from the other object.
            index = other.index;
            mgr = other.mgr;

            // Null out the other object.
            other.mgr = nullptr;
            other.index = -1;
        }
        return *this;
    }

    ID& ID::operator =( const ID& m )
    {
        m.mgr->dup( &m, this );
        mgr = m.mgr;
        return *this;
    }

    ID::operator size_t()const
    {
        return index;
    }

    ID::operator uint32_t()const
    {
        return index;
    }

    ID::operator int32_t()const
    {
        return index;
    }
    
    ID::operator bool()const
    {
        return 0 <= index;
    }

    ID::ID( IDManager* m, int i )
    :   mgr(m)
    ,   index(i)
    {}

    ID::ID(ID&& other) noexcept
    :   mgr(other.mgr)
    ,   index(other.index)
    {
        // Leave the moved-from object in a valid but null state
        // so its destructor doesn't release the ID we just stole.
        other.mgr = nullptr;
        other.index = -1;
    }

    ID::ID()
    :   mgr(nullptr)
    ,   index(-1)
    {}

    ID::~ID()
    {
        if ( mgr )
        {
            mgr->release( this );
            mgr = nullptr;
        }
    }

    ID::ID( const ID& m )
    {
        m.mgr->dup( &m, this );
        mgr = m.mgr;
    }
}
