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
        _ui->push({.style = &_ui->theme()->navbar});
        {
            if ( _ui->charButn(_uid[0], "+",7,2) )
            {
                //Communication::broadcast<const OpenAddAssetWindow&>({});
                _manager->handle(this, "OpenAddAssetWindow",nullptr);
            }

            if ( _ui->charButn(_uid[1], "^",7, 2) )
            {
                _manager->handle(this, "OpenFileBrowser",nullptr);
            }

            // Search field
//            uint64_t search_id = _ui->getAddressAsInt(&_search_buffer);// + _ref_counter;
//            if (_ui->textField(_uid[2], search_id, _search_buffer, 255, _search_size))
//            {
//                _manager->handle(this, "UpdateFilteredAssets",_search_buffer);
//            }

            if (_ui->input(_uid[2], ui::Cursor::InputType::Any, mode, text))
            {
                memcpy(_search_buffer, _ui->getCharBufr(), _ui->getCharBufrLen());
            }
            //bool UI::input(const ui::ID& user_id, uTextFieldMode& mode, ui::Text& text )


            // Refresh button
            if ( _ui->charButn(_uid[3], "@",7,0) )
            {
                _manager->handle(this, "RefreshAssetList",nullptr);
            }

            // View mode toggle
            const char* str =  (_view_mode == ViewMode::GRID) ? "#" : "=";
            if ( _ui->charButn(_uid[4], str, 7, 2) )
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
        _ui->pop();
    }

    AssetManagerToolBar::AssetManagerToolBar(AssetManagerUI* m,kege::UI* g)
    :   AssetManagerModule(m,g)
    ,   _search_size(0)
    {
        _search_buffer[0] = 0;
        text.ptr = _search_buffer;
        text.width = 100;
        text.height = 20;
        text.x = 0;
        text.y = 0;
        text.color = 0xffffffff;
        text.font_size = 20;
    }
}

