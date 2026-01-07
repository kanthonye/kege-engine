//
//  tree-node.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/24/25.
//

#ifndef tree_node_hpp
#define tree_node_hpp

#include "../../core/scene/ecs.hpp"
#include "../../core/input/input-context-manager.hpp"
#include "../../core/graphics/gui/ui-input.hpp"
#include "../../core/graphics/gui/ui-layout.hpp"
#include "../../core/graphics/gui/ui-viewer.hpp"

#include "../gui.hpp"

namespace kege::ui{

    struct TextFieldState
    {
        size_t cursor_pos = 0;
        size_t selection_start = 0;
        size_t selection_end = 0;
        bool has_focus = false;
        bool is_selected = false;
    };

    struct TreeNode
    {
        bool state[2];
        std::string name;
        std::vector< UID > elements;
        std::vector< TreeNode > children;
        ui::TextFieldState text_state;
    };

    typedef std::map< void*, ui::TreeNode > Tree;
    typedef std::map< int, bool(*)( kege::ECS* ecs, ui::Layout&, ui::Tree&, ecs::Entity& ) > UIElementCreator;

}
#endif /* tree_node_hpp */
