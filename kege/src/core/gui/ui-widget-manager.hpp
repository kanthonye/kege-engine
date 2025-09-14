//
//  ui-widget-manager.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/27/25.
//

#ifndef ui_widget_manager_hpp
#define ui_widget_manager_hpp

#include "ui-core.hpp"

namespace kege::ui{

    class WidgetManager
    {
    private:

        struct Content
        {
            kege::ui::Widget widget;

            // the node index
            uint32_t node_index;

            // the number of duplicates, reference counter
            int16_t duplicates;

            // next free id
            int16_t next;

            // previous free id
            int16_t prev;
        };

        struct List
        {
            int16_t head;
            int16_t tail;
            int16_t count;
        };

    public:

        void duplicate( int32_t src_index, int32_t* dst_index );

        void setNodeIndex( int32_t index, int32_t nodex_index );
        uint32_t getNodeIndex( int32_t index )const;

        /**
         * Retrieves a UI element by its index (const version).
         *
         * @param index The ui element index.
         *
         * @return The UI element at the specified index.
         */
        const kege::ui::Widget& operator[]( int32_t index ) const;

        /**
         * Retrieves a UI element by its index (non-const version).
         *
         * @param index The ui element index.
         *
         * @return The UI element at the specified index.
         */
        kege::ui::Widget& operator[]( int32_t index );

        /**
         * Creates a UI element with the give info.
         *
         * @param widget contains the attribute the ui requires.
         *
         * @return The element id.
         */
        int32_t make( const Widget& widget );

        /**
         * recycle the give index for reuse.
         *
         * @param index the given index.
         */
        void recycle( int32_t index );

        /**
         * Resize total number of layout elements.
         *
         * @param max_quantity The maximum number of UI elements the system can manage.
         */
        void resize( int32_t max_quantity );


        void refresh();

        WidgetManager();

    private:

        int32_t generate();

    private:

        std::vector< Content > _contents;

        /**
         * The recycles list of elements are transfered to this list so that they
         * can be reused in the next frame. This is necessary to prevent elements
         * recycled in the current frame is not recycled in the same frame.
         */
        List _available;

        /**
         * List of the currently recycled elements
         */
        List _recycled;

//        int32_t _recycled_count;
//        int32_t _recycled_head;
//        int32_t _recycled_tail;
//        int32_t _available_id;
//        int32_t _recycled_id;
    };

}
#endif /* ui_widget_manager_hpp */
