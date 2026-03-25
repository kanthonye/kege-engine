//
//  asset-manager-tool-bar.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/15/26.
//

#include "../ui-asset-manager.hpp"
#include "asset-manager-tool-bar.hpp"

namespace kege::ui{

    struct OpenAddAssetWindow{};

    struct OpenFileBrowser{};

    struct UpdateFilteredAssets{};

    struct RefreshAssetList{};

    struct ChangeAssetView{ViewMode view_mode;};

    void AssetManagerToolBar::update()
    {
        // Start a row for navigation controls
        _gui->push({.layer = 0, .style = &_gui->_theme.navbar});
        {
            if ( _gui->charButn(UI_BASE_ID(), "+",7,2) )
            {
                //Communication::broadcast<const OpenAddAssetWindow&>({});
                _manager->handle(this, "OpenAddAssetWindow",nullptr);
            }

            if ( _gui->charButn(UI_BASE_ID(), "^",7, 2) )
            {
                _manager->handle(this, "OpenFileBrowser",nullptr);
            }

            // Search field
            uint64_t search_id = _gui->getAddressAsInt(&_search_buffer);// + _ref_counter;
            if (_gui->textField(0, search_id, _search_buffer, 255, _search_size))
            {
                _manager->handle(this, "UpdateFilteredAssets",_search_buffer);
            }

            // Refresh button
            if ( _gui->charButn(UI_BASE_ID(), "@",7,0) )
            {
                _manager->handle(this, "RefreshAssetList",nullptr);
            }

            // View mode toggle
            const char* str =  (_view_mode == ViewMode::GRID) ? "#" : "=";
            if ( _gui->charButn(UI_BASE_ID(), str, 7, 2) )
            {
                ViewMode view_mode = (_view_mode == ViewMode::GRID)
                ? ViewMode::LIST : ViewMode::GRID;
                if (_view_mode != view_mode)
                {
                    _view_mode = view_mode;
                    _manager->handle(this, "ChangeAssetView",&_view_mode);
                }
            }
        }
        _gui->pop();
    }
}

