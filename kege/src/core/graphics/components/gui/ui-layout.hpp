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

        void beginRoot();
        void endRoot();

        /**
         * Insert node as a parent UI node. Any nodes insert after this node
         * will be a child or decendent of this node until it is pop.
         *
         * @param node The node to insert and make into a parent node
         *
         * @return reference to the ui element.
         */
        kege::ui::NodeId push( kege::ui::Node* node );

        /**
         * Creates a UI element with the give description.
         *
         * @param node The node element
         *
         * @return reference to the ui element.
         */
        kege::ui::NodeId put( kege::ui::Node* node );

        /**
         * Pops the current parent UI element from the parent stack.
         *
         * @return The index of the popped UI element in the elements array.
         */
        uint32_t pop();

        kege::ui::NodeId text( const kege::ui::Text& text );

        void pushLayer( uint32_t index );
        bool popLayer();

        void onWindowResize(int width, int height);

        const kege::ui::Elem* elem( const kege::ui::NodeId& id ) const;
        kege::ui::Elem* elem( const kege::ui::NodeId& id );

        const kege::ui::Elem* elem( const ui::Node* node )const;
        kege::ui::Elem* elem( const ui::Node* node );

        /**
         * Retrieves a UI element by its index (const version).
         *
         * @param index The ui element index.
         *
         * @return The UI element at the specified index.
         */
        const kege::ui::Node* operator[](uint32_t index) const;

        /**
         * Retrieves a UI element by its index (non-const version).
         *
         * @param index The ui element index.
         *
         * @return The UI element at the specified index.
         */
        kege::ui::Node* operator[](uint32_t index);

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

        void insert( kege::ui::Node* node );

    private:

        std::stack< uint32_t > _layer_stack;
        kege::array< Layer > _layers;

        kege::array< int32_t > _root_stack;
        int32_t _root_stack_count;
        
        kege::GUI* _gui;

        uint32_t _curr_parent;
        uint32_t _curr_layer;
        
        uint32_t _head;
        uint32_t _tail;
        uint32_t _count;

        kege::mat44 _transform;

        kege::ui::Rect _rect;

        double _dms;

        friend kege::ui::Layer;
        friend kege::GUI;
    };


}
#endif /* ui_layout_hpp */
