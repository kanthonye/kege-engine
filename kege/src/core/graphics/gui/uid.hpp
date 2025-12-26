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
    class Layout;

    union Id
    {
        Id(): id(0) {}
        struct
        {
            uint32_t index;
            uint32_t version;
        };
        uint64_t id;
    };

}
namespace kege{

    struct UID
    {
        UID& operator=(const UID& uid);
        UID& operator=( UID&& uid);

        UID(const UID& uid);
        UID( UID&& uid);

        UID();
        ~UID();

        ui::Id id;
        ui::Id elem_id;

    private:

        mutable int* duplicates;
        static int32_t _head;
        static int32_t _tail;
        static uint32_t _enumerator;
        static std::vector<ui::Id> _availables;
    };


    constexpr inline bool operator==(const UID &a, const UID &b) { return a.id.id == b.id.id; }
    constexpr inline bool operator!=(const UID &a, const UID &b) { return a.id.id != b.id.id; }

}
#endif  /* kege_uid_hpp */
