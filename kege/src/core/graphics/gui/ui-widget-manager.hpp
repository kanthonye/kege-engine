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

        struct EntryInfo
        {
            // the node index
            uint32_t node_index;

            // the number of duplicates, reference counter
            int16_t duplicates;
            
            // the node index
            uint16_t version;

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

        void setNodeIndex( const Handle& handle, int32_t nodex_index );
        uint32_t getNodeIndex( const Handle& handle )const;

        /**
         * Retrieves a UI element by its index (const version).
         *
         * @param handle The ui element index.
         *
         * @return The UI element at the specified index.
         */
        const kege::ui::Widget& operator[]( const Handle& handle ) const;

        /**
         * Retrieves a UI element by its index (non-const version).
         *
         * @param handle The ui element index.
         *
         * @return The UI element at the specified index.
         */
        kege::ui::Widget& operator[]( const Handle& handle );


            void duplicate( const Handle& handle );

        /**
         * Creates a UI element with the give info.
         *
         * @param widget contains the attribute the ui requires.
         *
         * @return The element id.
         */
        Handle make( const Widget& widget );

        /**
         * recycle the give index for reuse.
         *
         * @param index the given index.
         */
        void recycle( const Handle& index );

        /**
         * Resize total number of layout elements.
         *
         * @param max_quantity The maximum number of UI elements the system can manage.
         */
        void resize( int32_t max_quantity );

        void refresh();

        WidgetManager();

    private:

        std::vector< kege::ui::Widget > _widgets;
        std::vector< EntryInfo > _contents;

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
    };

}
#endif /* ui_widget_manager_hpp */
