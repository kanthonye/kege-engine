//
//  uid.cpp
//  editor
//
//  Created by Kenneth Esdaile on 12/5/25.
//

#include "uid.hpp"
#include "ui-layout.hpp"

namespace kege::ui{

    static int32_t global_head = -1;
    static int32_t global_tail = -1;
    static uint32_t global_enumerator = 0;
    static std::vector<ui::Id> global_availables;

//    int32_t UID::_head = -1;
//    int32_t UID::_tail = -1;
//    uint32_t UID::_enumerator = 0;
//    std::vector<ui::Id> UID::_availables;

    UID& UID::operator=(const UID& other)
    {
        id = other.id;
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
        id = uid.id;
        duplicates = uid.duplicates;

        uid.id.num = 0;
        uid.duplicates = nullptr;

        return *this;
    }

    void UID::recycle()
    {
        if ( id.num == 0 ) return;

        if (global_availables.size() == 0)
        {
            global_availables.resize(64);
        }
        else if (global_tail >= static_cast<int32_t>(global_availables.size()))
        {
            global_availables.resize(global_availables.size() * 2);
        }

        if (global_head < 0)
        {
            global_head = global_tail = 0;
        }

        id.version += 1;
        global_availables[global_tail] = id;
        global_tail += 1;

        id.num = 0;
    }

    UID::UID(const UID& other)
    :   id(other.id)
    ,   duplicates( other.duplicates )
    {
        if(duplicates) *duplicates += 1;
        else
        {
            duplicates = other.duplicates = new int(2);
        }
    }

    UID::UID( UID&& uid)
    :   id(uid.id)
    ,   duplicates( uid.duplicates )
    {
        uid.id.num = 0;
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
                duplicates = nullptr;
                recycle();
            }
        }
        else
        {
            recycle();
        }
    }
    
    UID::UID()
    :   duplicates( nullptr )
    {
        if (global_head != -1)
        {
            id = global_availables[global_head];
            global_head += 1;

            if (global_head >= global_tail)
            {
                global_head = global_tail = -1;
            }
            return;
        }

        id.version = 1;
        id.index = global_enumerator;
        global_enumerator += 1;
    }

}
