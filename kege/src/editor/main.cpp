//
//  main.cpp
//  ui
//
//  Created by Kenneth Esdaile on 3/5/25.
//

#include "entry-point.hpp"
#include "editor-layer.hpp"

namespace kege{

    class TestEditor : public kege::Application
    {
    public:

        bool initialize()
        {
            if ( !kege::Application::initialize() )
            {
                return false;
            }
            kege::ref::AppLayer ui_app_layer = new EditorLayer
            (
                _asset_manager.ref(),
                _render_graph.ref(),
                _project_manager.ref(),
                _input_context_manager.ref(),
                _ecs.ref()
            );
            _running = _app_layer_stack->push( ui_app_layer );
            return _running;
        }
    };
    
    kege::Ref< Application > createApplication()
    {
        return new TestEditor;
    }

}




struct SizingStyle
{
};
struct SpacingStyle
{
    //Padding padding;
    //Gap gap;
};
struct Style
{
//    Rect rect;
//    Color color;
    SizingStyle* visual;
    SpacingStyle* spacing;
//    VisualStyle* visual;
//    AlignmentStyle* alignment;
//    ButtonConfig button_config; style.flag = WIDTH | HEIGH | GAP | PADDING | ALIGNMENT;
};



//.text_str = ""
//.text_rect = layout->coverage("string");

kege::ui::Layout* layout = 0;
void vmain(){
    layout->put({.text = layout->text("label", 20)});
    layout->put({.text = "label"});
}
