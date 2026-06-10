//
//  ui-state-manager.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 6/10/26.
//

#include "ui-state-manager.hpp"

namespace kege::ui{

    uint32_t StateMgr::_frame_number = 0;
    std::vector< StateMgr* > StateManager::_mgrs;

}
