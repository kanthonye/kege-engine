//
//  uid.cpp
//  editor
//
//  Created by Kenneth Esdaile on 12/5/25.
//

#include "uid.hpp"
#include "ui-layout.hpp"

namespace kege::ui{

    int32_t UID::_head = -1;
    int32_t UID::_tail = -1;
    uint32_t UID::_enumerator = 0;
    std::vector<ui::Id> UID::_availables;

    UID& UID::operator=(const UID& other)
    {
        global = other.global;
        duplicates = other.duplicates;

        if(duplicates) *duplicates += 1;
        else
        {
            duplicates = other.duplicates = new int(2);
        }
        return *this;
    }

    UID& UID::operator=( UID&& uid)
    {
        global = uid.global;
        duplicates = uid.duplicates;

        uid.global.id = 0;
        uid.duplicates = nullptr;

        return *this;
    }

    UID::UID(const UID& other)
    :   global(other.global)
    ,   duplicates( other.duplicates )
    {
        if(duplicates) *duplicates += 1;
        else
        {
            duplicates = other.duplicates = new int(2);
        }
    }

    UID::UID( UID&& uid)
    :   global(uid.global)
    ,   duplicates( uid.duplicates )
    {
        uid.global.id = 0;
        uid.duplicates = nullptr;
    }

    UID::~UID()
    {
        if(duplicates)
        {
            *duplicates -= 1;
            if(*duplicates <= 0)
            {
                delete duplicates;
            }
            duplicates = nullptr;
        }

        if ( global.id != 0 )
        {
            if (_availables.size() == 0)
            {
                _availables.resize(64);
            }
            else if (_tail >= static_cast<int32_t>(_availables.size()))
            {
                _availables.resize(_availables.size() * 2);
            }

            if (_head < 0)
            {
                _head = _tail = 0;
            }

            global.version += 1;
            _availables[_tail] = global;
            _tail += 1;

            global.id = 0;
        }
    }
    
    UID::UID()
    :   duplicates( nullptr )
    {
        if (_head != -1)
        {
            global = _availables[_head];
            _head += 1;

            if (_head >= _tail)
            {
                _head = _tail = -1;
            }
            return;
        }

        global.version = 1;
        global.index = _enumerator;
        _enumerator += 1;
    }

}
