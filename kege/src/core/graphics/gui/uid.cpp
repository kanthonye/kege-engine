//
//  uid.cpp
//  editor
//
//  Created by Kenneth Esdaile on 12/5/25.
//

#include "uid.hpp"
#include "ui-layout.hpp"

namespace kege::ui{

    UID::Value* UID::Recycler::create()
    {
        Value* value;
        if (_recycler.head != nullptr)
        {
            value = _recycler.head;
            _recycler.head = _recycler.head->next;
            value->duplicates = 0;
        }
        else
        {
            value = new UID::Value{ UID::_id_counter++, 0, nullptr };
        }
        return value;
    }
    UID::Recycler::~Recycler()
    {
        while (head != nullptr)
        {
            UID::Value* h = head;
            head = head->next;
            delete h;
        }
        tail = nullptr;
    }
    UID::Recycler::Recycler()
    :   head(nullptr)
    ,   tail(nullptr)
    {}


    UID::Recycler UID::_recycler;
    uint32_t UID::_id_counter = 1;


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

    UID::operator uint32_t()const
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
