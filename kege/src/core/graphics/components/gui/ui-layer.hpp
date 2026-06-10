//
//  ui-layer.hpp
//  editor
//
//  Created by Kenneth Esdaile on 5/12/26.
//

#ifndef ui_layer_hpp
#define ui_layer_hpp

#include "ui-core.hpp"
#include "ui-resizer.hpp"
#include "ui-aligner.hpp"

namespace kege::ui{

    struct GUI;
    struct Layout;
    class Layer
    {
    public:

        kege::ui::Record getHotElem(uint32_t root, bool button = false);
        kege::ui::Record getHotElem(bool button = false);

        uint32_t find(uint64_t user_id, uint32_t index);
        uint32_t find(uint64_t user_id);

        /**
         * Creates a parent UI element with the give info.
         *
         * @param desc This refers to the ui element id
         *
         * @return reference to the ui element.
         */
        kege::ui::WidgetId pushRoot( const kege::ui::WidgetDesc& desc );
        kege::ui::WidgetId putRoot( const kege::ui::WidgetDesc& desc );

        /**
         * Pops the current parent UI element from the parent stack.
         */
        void popRoot();

        /**
         * Creates a parent UI element with the give info.
         *
         * @param desc This refers to the ui element id
         *
         * @return reference to the ui element.
         */
        kege::ui::WidgetId push( const kege::ui::WidgetDesc& desc );
        /**
         * Creates a UI element with the give description.
         *
         * @param desc Description of the ui element
         *
         * @return reference to the ui element.
         */
        kege::ui::WidgetId put( const kege::ui::WidgetDesc& desc );
        kege::ui::WidgetId text( const kege::ui::Text& text );

        /**
         * Pops the current parent UI element from the parent stack.
         *
         * @return The index of the popped UI element in the elements array.
         */
        uint32_t pop();

        void init( const kege::ui::WidgetDesc& desc, kege::ui::Widget* widget );

        /**
         * Begins the UI layout construction. Must be called before creating any UI elements.
         */
        void begin( double dms );

        /**
         * Ends the UI layout construction. Must be called after all UI elements are created.
         */
        void end();

        void initalize( kege::GUI* gui, kege::ui::Layout* layout );

        uint32_t getHeadRoot()const;
        uint32_t getTailRoot()const;
        uint32_t getNextRoot( uint32_t root )const;
        uint32_t getPrevRoot( uint32_t root )const;

        Layer();
        
    private:

        kege::GUI* _gui;
        kege::ui::Layout* _layout;

        uint32_t _head_root;
        uint32_t _tail_root;
        uint32_t _root_count;
        uint32_t _count;

        kege::array< int32_t > _root_stack;
        int32_t _root_stack_count;

        uint32_t _current_parent;
        uint32_t _curr_root;

        friend kege::ui::Layout;
    };

}

#endif /* ui_layer_hpp */
