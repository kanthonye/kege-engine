//
//  ui-canvas.hpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#ifndef ui_canvas_hpp
#define ui_canvas_hpp

#include "ui-input.hpp"
#include "ui-viewer.hpp"
#include "ui-layout.hpp"
//
//namespace kege::ui{
//
//    class Canvas
//    {
//    public:
//        
//        bool initialize( Graphics* graphics, kege::PipelineHandle pipeline, kege::Font font );
//        void shutdow();
//
//        void view( kege::CommandBuffer* command, ui::Layout& layout );
//
//        inline Layout* operator ->()const { return _layout; }
//        inline Layout* operator ->() { return _layout; }
//
//        const UIElem* operator[]( const EID& id )const;
//        UIElem* operator[]( const EID& id );
//
//        void begin( ui::Layout* layout );
//        void end();
//
//        ~Canvas();
//        Canvas();
//
//    public:
//
//        std::vector< ui::Layout* > _layout_stack;
//        ui::Layout* _layout;
//        ui::Viewer _viewer;
//        ui::Input _input;
//    };
//
//}
#endif /* ui_canvas_hpp */
