//
//  tree-node.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/24/25.
//

#ifndef tree_node_hpp
#define tree_node_hpp

#include "../../core/ecs/entity.hpp"
#include "../../core/input/input-context-manager.hpp"
#include "../../core/graphics/gui/ui-input.hpp"
#include "../../core/graphics/gui/ui-layout.hpp"
#include "../../core/graphics/gui/ui-viewer.hpp"

namespace kege::ui{

    struct TreeNode
    {
        bool state[2];
        std::string name;
        std::vector< ui::Elem > elements;
        std::vector< TreeNode > children;
        ui::TextFieldState text_state;
    };

    typedef std::map< void*, ui::TreeNode > Tree;
    typedef std::map< int, bool(*)( ui::Layout&, ui::Tree&, Entity& ) > UIElementCreator;

}
#endif /* tree_node_hpp */
