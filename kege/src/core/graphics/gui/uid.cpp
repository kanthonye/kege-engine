//
//  uid.cpp
//  editor
//
//  Created by Kenneth Esdaile on 12/5/25.
//

#include "uid.hpp"

namespace kege::ui{

    UID::Value* UID::Recycler::create()
    {
        Value* value;
        if (head != nullptr)
        {
            value = head;
            head = head->next;
            value->duplicates = 0;
        }
        else
        {
            value = new UID::Value{ ((uint64_t) _id_counter++ << 32), 1, nullptr };
        }
        return value;
    }

    UID::Recycler::~Recycler()
    {
        uint64_t count = _id_counter;
        while (head != nullptr)
        {
            UID::Value* h = head;
            head = head->next;
            delete h;
            count -= 1;
        }
        tail = nullptr;
    }

    UID::Recycler::Recycler()
    :   head(nullptr)
    ,   tail(nullptr)
    ,   _id_counter(1)
    {}

}


namespace kege::ui{

    UID::Recycler UID::_recycler;

    void UID::clear()
    {
        if(_value)
        {
            _value->duplicates -= 1;
            if(_value->duplicates <= 0 )
            {
                _value->next = nullptr;
                if (_recycler.head == nullptr)
                {
                    _recycler.tail = _recycler.head = _value;
                }
                else
                {
                    _recycler.tail->next = _value;
                    _recycler.tail = _value;
                }
                _value = nullptr;
            }
        }
    }

    ID UID::operator[]( uint32_t index )const
    {
        if ( _value == nullptr )
        {
            _value = _recycler.create();
        }
        return { *this, _value->value | uint64_t( index ) };
    }

    UID::operator uint64_t()const
    {
        if (_value) return _value->value;
        return 0;
    }

    UID& UID::operator=( const UID& other )
    {
        if (this == &other || _value == other._value) return *this;
        clear();
        _value = other._value;
        if(_value != nullptr) _value->duplicates += 1;
        return *this;
    }

    UID& UID::operator=( UID&& other )
    {
        _value = other._value;
        other._value = nullptr;
        return *this;
    }

    UID::UID(const UID& other )
    :   _value(nullptr)
    {
        *this = other;
    }

    UID::UID( UID&& other )
    {
        _value = other._value;
        other._value = nullptr;
    }

    UID::~UID()
    {
        clear();
    }

    UID::UID( Value* value )
    :   _value( value )
    {}

    UID::UID()
    :   _value( nullptr )
    {
    }

}
