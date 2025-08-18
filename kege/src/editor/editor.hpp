//
//  editor.hpp
//  ui
//
//  Created by Kenneth Esdaile on 3/7/25.
//

#ifndef editor_hpp
#define editor_hpp

#include "../core/engine/engine.hpp"
#include "../core/gui/ui-input.hpp"
#include "../core/gui/ui-viewer.hpp"
#include "../core/gui/ui-layout.hpp"
#include "../core/gui/ui-font-creator.hpp"

namespace kege{

    class Editor
    {
    public:
        /**
         * @brief Run the editor application
         *
         * Initializes the editor, sets up the UI, and starts the main loop.
         *
         * @return true if the editor runs successfully, false otherwise.
         */
        bool run();
        Editor();

        Editor( const Editor& ) = delete;
        Editor& operator=( const Editor& ) = delete;
        Editor( Editor&& ) = delete;
        Editor& operator=( Editor&& ) = delete;

    public:

        void operator()( kege::RenderPassContext* context );

        void drawHierarchyPanel();
        void drawInspectorPanel();
        void drawAssetBrowser();
        void drawViewportPanel();
        void buildEditorPanels();
        void initLayout();
        bool initalize();
        void shutdown();
        void loop();

        ui::Layout _layout;
        ui::Viewer _viewer;
        ui::Input _input;

        kege::Engine _engine;
        bool _paused;

        ui::EID navbar_panel;
        ui::EID hierarchy_panel;
        ui::EID inspector_panel;
        ui::EID asset_browser;
        ui::EID viewport_panel;
        ui::EID main_panel;
    };
}

#endif // editor_hpp
