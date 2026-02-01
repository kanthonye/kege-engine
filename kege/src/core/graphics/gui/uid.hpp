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
        UID& operator=(const UID& uid);
        UID& operator=( UID&& uid);

        UID(const UID& uid);
        UID( UID&& uid);

        UID();
        ~UID();

        ui::Id id;
        uint32_t widget_index;

    private:

        void recycle();

        mutable int* duplicates;
    };


    constexpr inline bool operator==(const UID &a, const UID &b) { return a.id == b.id; }
    constexpr inline bool operator!=(const UID &a, const UID &b) { return a.id != b.id; }

}
#endif  /* kege_uid_hpp */
