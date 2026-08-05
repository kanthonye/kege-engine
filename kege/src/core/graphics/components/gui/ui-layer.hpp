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

        void beginRoot();
        void endRoot();

        void push( kege::ui::Node* node );
        void put( kege::ui::Node* node );
        void pop();

        void reset( double dms );

        void initalize( kege::GUI* gui );

        uint32_t getHeadRoot()const;
        uint32_t getTailRoot()const;
        uint32_t getNextRoot( uint32_t root )const;
        uint32_t getPrevRoot( uint32_t root )const;

        Layer();


        void insertChild( kege::ui::Widget* root );
        void insertRoot( kege::ui::Widget* root );
    private:

        kege::GUI* _gui;

        uint32_t _parent;
        uint32_t _head;
        uint32_t _tail;
        uint32_t _count;

        kege::array< int32_t > _root_stack;
        int32_t _root_stack_count;

        friend kege::ui::Layout;
        friend kege::GUI;
    };

}

#endif /* ui_layer_hpp */
