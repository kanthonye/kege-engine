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
    typedef uint64_t UserId;

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

    struct Handle
    {
        //private:
        UserId user_id;
        Id handle;
    };


    struct EID
    {
        ui::Id global;
        ui::Id local;
    };

    constexpr inline bool operator==(const ui::EID &a, const ui::EID &b) { return a.global == b.global && a.local == b.local; }
    constexpr inline bool operator!=(const ui::EID &a, const ui::EID &b) { return a.global != b.global && a.local == b.local; }
}
namespace kege::ui{

    struct UID
    {
    private:

        struct Value
        {
            uint32_t value;
            int32_t duplicates;
            Value* next;
        };

        struct Recycler
        {
            Value* create();
            ~Recycler();
            Recycler();

            Value* head;
            Value* tail;
        };

    private:

        inline friend bool operator==(const ui::UID &a, const ui::UID &b) { return a._value == b._value; }
        inline friend bool operator!=(const ui::UID &a, const ui::UID &b) { return a._value != b._value; }

        inline static UID create(){ return UID( _recycler.create() ); }

        operator uint32_t()const;

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

        Value* _value;
        static Recycler _recycler;
        static uint32_t _id_counter;
    };

}
#endif  /* kege_uid_hpp */
