//
//  tree-node.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/24/25.
//

#ifndef tree_node_hpp
#define tree_node_hpp

#include "../../core/scene/ecs.hpp"
#include "../../core/graphics/mesh/mesh-primitives.hpp"
#include "../../core/graphics/light/light.hpp"

#include "../../core/input/input-context-manager.hpp"
#include "../../core/graphics/gui/ui-layout.hpp"
#include "../../core/graphics/gui/ui-viewer.hpp"
#include "../../core/graphics/particle/effect/particle-effect.hpp"

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

    typedef bool(*InspectorComponentUIBuilder)( kege::AssetManager* am, int16_t layer, kege::GUI*, kege::ECS* ecs, ecs::Entity& );
    typedef std::map< int, InspectorComponentUIBuilder > ComponentBuilderFactory;



    bool mesh( kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity );
    bool light( kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity );
    bool particleEffect( kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity );
}
#endif /* tree_node_hpp */
