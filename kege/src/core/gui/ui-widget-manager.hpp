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
            int16_t duplicates = 0;

            // next free id
            int16_t next;

            // previous free id
            int16_t prev;
        };

    public:

        void duplicate( uint32_t src_index, uint32_t* dst_index );

        void setNodeIndex( uint32_t index, uint32_t nodex_index );
        uint32_t getNodeIndex( uint32_t index )const;

        /**
         * Retrieves a UI element by its index (const version).
         *
         * @param index The ui element index.
         *
         * @return The UI element at the specified index.
         */
        const kege::ui::Widget& operator[]( uint32_t index ) const;

        /**
         * Retrieves a UI element by its index (non-const version).
         *
         * @param index The ui element index.
         *
         * @return The UI element at the specified index.
         */
        kege::ui::Widget& operator[]( uint32_t index );

        /**
         * Creates a UI element with the give info.
         *
         * @param widget contains the attribute the ui requires.
         *
         * @return The element id.
         */
        uint32_t make( const Widget& widget );

        /**
         * recycle the give index for reuse.
         *
         * @param index the given index.
         */
        void recycle( uint32_t index );

        /**
         * Resize total number of layout elements.
         *
         * @param max_quantity The maximum number of UI elements the system can manage.
         */
        void resize( uint32_t max_quantity );


        void refresh();

        WidgetManager();

    private:

        uint32_t generate();

    private:

        std::vector< Content > _contents;

        int32_t _recycled_count;
        int32_t _recycled_head;
        int32_t _recycled_tail;
        int32_t _available_id;
        int32_t _recycled_id;
    };

}
#endif /* ui_widget_manager_hpp */
