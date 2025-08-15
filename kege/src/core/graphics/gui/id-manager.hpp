//
//  id-manager.hpp
//  gui
//
//  Created by Kenneth Esdaile on 8/8/25.
//

#ifndef id_manager_hpp
#define id_manager_hpp

#include <vector>

namespace kege::ui{

    class IDManager;

    struct ID
    {
        // Move assignment operator
        ID& operator =(ID&& other) noexcept;
        ID& operator =( const ID& m );

        operator size_t()const;
        operator uint32_t()const;
        operator int32_t()const;
        operator bool()const;

        // Move constructor
        ID(ID&& other) noexcept;

        ID( IDManager* m, int i );
        ID( const ID& m );
        ID();

        ~ID();

        int32_t index = -1; // -1 indicates end of list
        IDManager* mgr;
    };

    class IDManager
    {
    public:

        /**
         * Duplicate ID. from src to dst.
         */
        void dup( const ID* src, ID* dst );

        /**
         * Generates a new ID from the pool.
         */
        ID gen();

        /**
         * User can specify the max number of dynamic IDs needed.
         */
        explicit IDManager(size_t pool_size);

        // Not copyable or movable.
        IDManager(const IDManager&) = delete;
        IDManager& operator=(const IDManager&) = delete;

    private:

        /**
         * Returns an ID to the free list.
         */
        void release( ID* id );

    private:

        struct NodeID
        {
            ID id;
            int16_t duplicates = 0; // reference counter
        };

        std::vector< NodeID > _pool;
        int32_t _free_list_head = -1;
        friend ID;
    };

}
#endif /* id_manager_hpp */
