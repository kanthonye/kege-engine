//
//  uid.hpp
//  editor
//
//  Created by Kenneth Esdaile on 12/5/25.
//

#ifndef kege_uid_hpp
#define kege_uid_hpp

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

namespace kege::ui{
    //typedef uint64_t ui::ID;

    class Layout;

    union Id
    {
        Id(): num(0) {}
        struct
        {
            uint32_t index;
            uint32_t version;
        };
        uint64_t num;
    };
    constexpr inline bool operator==(const ui::Id &a, const ui::Id &b) { return a.num == b.num; }
    constexpr inline bool operator!=(const ui::Id &a, const ui::Id &b) { return a.num != b.num; }

//    struct Handle
//    {
//        //private:
//        ui::ID user_id;
//        Id handle;
//    };


    struct EID
    {
        ui::Id global;
        ui::Id local;
    };

    constexpr inline bool operator==(const ui::EID &a, const ui::EID &b) { return a.global == b.global && a.local == b.local; }
    constexpr inline bool operator!=(const ui::EID &a, const ui::EID &b) { return a.global != b.global && a.local == b.local; }
}
namespace kege::ui{

    struct ID;

    struct UID
    {
    private:

        // 64-bit structure:
        // Bits 63-32: Base ID (allocated from pool)
        // Bits 31-0:  Derived ID (user-managed)
        using ValueType = uint64_t;

        static constexpr ValueType DERIVED_MASK = 0xFFFFFFFF;
        static constexpr ValueType BASE_SHIFT = 32;

        struct Value
        {
            uint64_t value;
            int32_t duplicates;
            Value* next;
        };

        struct Recycler
        {
            Value* create();
            ~Recycler();
            Recycler();

            uint64_t _id_counter;
            Value* head;
            Value* tail;
        };

    public:

        inline friend bool operator==(const ui::UID &a, const ui::UID &b) { return a._value == b._value; }
        inline friend bool operator!=(const ui::UID &a, const ui::UID &b) { return a._value != b._value; }

        inline static UID create(){ return UID( _recycler.create() ); }

        ID operator[]( uint32_t index )const;
        operator uint64_t()const;

        UID& operator=(const UID& uid);
        UID& operator=( UID&& uid);

        UID(const UID& uid);
        UID( UID&& uid);

        ~UID();
        UID();

    private:

        UID( Value* value );
        void clear();

    private:

        mutable Value* _value;
        static Recycler _recycler;
    };

    struct ID
    {
        operator uint64_t()const
        {
            return value;
        }

        ID& operator=(const ID& other)
        {
            user_id = other.user_id;
            value = other.value;
            return *this;
        }

        ID& operator=( ID&& other)
        {
            user_id = other.user_id;
            value = other.value;
            return *this;
        }

        ID( const UID& user_id, uint64_t value )
        :   user_id( user_id )
        ,   value( value )
        {
        }

        ID( const ID& other )
        :   user_id( other.user_id )
        ,   value( other.value )
        {
        }

        ID( ID&& other)
        :   user_id( other.user_id )
        ,   value( other.value )
        {
        }

        ID()
        :   user_id()
        ,   value( 0 )
        {}

        ~ID()
        {
        }

        UID user_id;
        uint64_t value;
        friend struct UID;
    };

}
#endif  /* kege_uid_hpp */
