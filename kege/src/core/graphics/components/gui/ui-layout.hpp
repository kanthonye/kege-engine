//
//  ui-layout.hpp
//  ui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#ifndef ui_layout_hpp
#define ui_layout_hpp

#include <stack>
#include "ui-layer.hpp"
#include "ui-cursor.hpp"

namespace kege::ui{

    class Layout
    {
    public:

        kege::ui::Record getHotElem(bool button = false);
        uint32_t find(uint64_t user_id);

        const kege::ui::Layer& getLayer( uint32_t i )const;
        uint32_t getLayerCount()const;

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
         * Pops the current parent UI element from the parent stack.
         *
         * @return The index of the popped UI element in the elements array.
         */
        uint32_t pop();

        /**
         * Creates a UI element with the give description.
         *
         * @param desc Description of the ui element
         *
         * @return reference to the ui element.
         */
        kege::ui::WidgetId put( const kege::ui::WidgetDesc& desc );
        kege::ui::WidgetId text( const kege::ui::Text& text );

        void pushLayer( uint32_t index );
        bool popLayer();

        void onWindowResize(int width, int height);

        /**
         * Retrieves a UI element by its index (const version).
         *
         * @param index The ui element index.
         *
         * @return The UI element at the specified index.
         */
        const kege::ui::Widget* operator[](uint32_t index) const;

        /**
         * Retrieves a UI element by its index (non-const version).
         *
         * @param index The ui element index.
         *
         * @return The UI element at the specified index.
         */
        kege::ui::Widget* operator[](uint32_t index);

        /**
         * Retrieves the parent index of a UI element.
         */
        uint32_t parent( uint32_t index )const;

        /**
         * Retrieves the head index of a UI element.
         */
        uint32_t head( uint32_t index )const;

        /**
         * Retrieves the tail index of a UI element.
         */
        uint32_t tail( uint32_t index )const;

        /**
         * Retrieves the next sibling index of a UI element.
         */
        uint32_t next( uint32_t index )const;

        /**
         * Retrieves the previous sibling index of a UI element.
         */
        uint32_t prev( uint32_t index )const;

        const kege::ui::Rect& getRect()const;
        /**
         * Begins the UI layout construction. Must be called before creating any UI elements.
         */
        void begin( double dms );

        /**
         * Ends the UI layout construction. Must be called after all UI elements are created.
         */
        void end();

        const kege::mat44& getTransform()const;

        void initialize
        (
            kege::GUI* gui,
            const kege::mat44& transform,
            const kege::ui::Rect& rect,
            uint32_t max_layers
        );

        void shutdown();

        Layout();
        ~Layout();
        
    private:

        std::stack< uint32_t > _layer_stack;
        kege::array< Layer > _layers;
        kege::GUI* _gui;

        uint32_t _curr_parent;
        uint32_t _curr_layer;

        kege::mat44 _transform;

        kege::ui::Rect _rect;

        double _dms;

        friend kege::ui::Layer;
    };


}
#endif /* ui_layout_hpp */
