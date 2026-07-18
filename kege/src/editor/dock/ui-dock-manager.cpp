//
//  ui-dock-manager.cpp
//  editor
//
//  Created by Kenneth Esdaile on 4/2/26.
//

#include "ui-dock-manager.hpp"

#include "../panels/asset-manager/ui-asset-manager.hpp"
#include "../panels/hierarchy/hierarchy-panel.hpp"
#include "../panels/inspector/inspector-panel.hpp"
#include "../panels/viewport/viewport-panel.hpp"
#include "../panels/ui-file-browser.hpp"

#include "../panels/ui-console.hpp"

#include "../editor-layer.hpp"


namespace kege::ui{

    DockSplit* DockManager::split
    (
        float slit_ratio,
        ui::Dock::SplitDirection dir,
        const std::vector< std::string >& a,
        const std::vector< std::string >& b
    )
    {
        return nullptr;
    }

    DockSplit* DockManager::split
    (
        float slit_ratio,
        ui::Dock::SplitDirection dir,
        const std::vector< int >& a,
        const std::vector< int >& b
    )
    {
        return nullptr;
    }


    std::vector< int32_t > DockManager::getPanelIndice( const std::vector< std::string >& names)
    {
        std::vector< int32_t > indices;
        for (const std::string& name : names)
        {
            auto itr = _panel_name_index_map.find( name );
            if( itr == _panel_name_index_map.end() )
            {
                indices.push_back( getPanelIndex( name ) );
            }
        }
        return indices;
    }

    int32_t DockManager::getPanelIndex( const std::string& name )
    {
        auto itr = _panel_name_index_map.find( name );
        if( itr != _panel_name_index_map.end() )
        {
            return itr->second;
        }
        return uint32_t(-1);
    }

    ui::Panel* DockManager::getPanel( uint32_t index )
    {
        if ( index < _panels.size() )
        {
            return _panels[ index ].ref();
        }
        return nullptr;
    }

    void DockManager::operator()(const kege::ui::AssetMetadataDropOff& event)
    {
        kege::ui::Dock* dock = getDock( event.position );
        if ( dock != nullptr )
        {
            uint32_t panel_index = dock->_tab.panel_indices[ dock->_tab.selection ];
            ui::Panel* panel = getPanel( panel_index );
            panel->operator()( event );
        }
    }

    void DockManager::operator()(const kege::WindowFrameBufferSizeEvent& event)
    {
        //_gui->onWindowFrameBufferResize(event.width, event.height);
    }

    void DockManager::operator()(const kege::WindowSizeEvent& event)
    {
        _gui->onWindowResize(event.width, event.height);
    }

    void DockManager::addPanel( Ref< ui::Panel > panel )
    {
        _panel_name_index_map[ panel->getName() ] = (int32_t)_panels.size();
        _panels.push_back( panel );
    }

    void DockManager::displayPanel( uint32_t index )
    {
        if ( index < _panels.size() )
        {
            _panels[ index ]->update();
        }
    }

    ui::GhostObject* DockManager::getGhostObject()
    {
        return &_ghost;
    }

    kege::ui::Dock* DockManager::getDock(const kege::dvec2& pointer)
    {
        return _root.getDock( pointer );
    }

    void DockManager::onWindowResize(int width, int height)
    {
        _root.onWindowResize(width, height);
    }

    void DockManager::update()
    {
//        _gui->begin( 0.016 );
//        {
//            _gui->pushRoot({ .rect = _rect });
//            {
        _root.update();
//            }
//            _gui->popRoot();
//        }
//        _gui->end();
    }

    kege::GraphicsDevice* DockManager::getGraphicsDevice()
    {
        return _gui->getGraphicsDevice();
    }

    kege::ProjectManager* DockManager::getProjectManager()
    {
        return _project_manager;
    }

    kege::AssetManager* DockManager::getAssetManager()
    {
        return _gui->getAssetManager();
    }

    kege::ECS* DockManager::getECS()
    {
        return _ecs;
    }
    
    kege::UI* DockManager::getUI()
    {
        return _ui;
    }

    DockManager::~DockManager()
    {
        _panel_name_index_map.clear();
        _panels.clear();
        kege::Communication::remove< const kege::ui::AssetMetadataDropOff&, DockManager >( this );
        kege::Communication::remove<const kege::WindowFrameBufferSizeEvent&, DockManager>(this);
        kege::Communication::remove<const kege::WindowSizeEvent&, DockManager>(this);
    }

    DockManager::DockManager
    (
        const kege::ui::Rect& rect,
        kege::GUI* gui,
        kege::UI* ui,
        kege::ProjectManager* pm,
        kege::ECS* ecs
    )
    :   _gui( gui )
    ,   _rect( rect )
    ,   _project_manager( pm )
    ,   _ecs( ecs )
    ,   _ui( ui )
    {
        kege::Communication::add< const kege::ui::AssetMetadataDropOff&, DockManager >( this );
        kege::Communication::add<const kege::WindowFrameBufferSizeEvent&, DockManager>(this);
        kege::Communication::add<const kege::WindowSizeEvent&, DockManager>(this);

        addPanel(new ui::HierarchyPanel( this ));
        addPanel(new kege::InspectorPanel( this ));
        addPanel(new kege::ViewportPanel( this ));
        addPanel(new ui::FileBrowser( this, "/Users/kae/Developer/vscode/kege-engine/kege/assets" ));
        addPanel(new ui::AssetManagerUI( this ));
        addPanel(new ui::Console( this ));
        //addPanel(new ui::MenuBar( this ));

        _root = ui::Dock(this, _rect);

        ui::DockSplit* split[2];

        //split[0] = _root.split( 0.25, ui::Dock::SplitDirection::HORIZONTAL );

        split[0] = _root.split( 0.25, ui::Dock::SplitDirection::HORIZONTAL );
        split[0]->nodes[0].split( 0.50, ui::Dock::SplitDirection::VERTICAL, {"Hierarchy"}, {"Properties"} );
        split[1] = split[0]->nodes[1].split( 0.70, ui::Dock::SplitDirection::VERTICAL, {"Viewport"}, {"AssetManager", "Console", "FileBrowser"} );

        split[0]->slit_ratio = 0.25;
        split[0]->onReshape();

        split[1]->slit_ratio = 0.70;
        split[1]->onReshape();
    }

}
