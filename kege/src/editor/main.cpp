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
                _input_context_manager.ref()
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
