//
//  ui-transform.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/24/25.
//

#ifndef ui_transform_hpp
#define ui_transform_hpp

#include "ui-numeric.hpp"

namespace kege::ui
{
    bool rotation( ui::Layout& layout, TreeNode& tree, float& x, float& y, float& z, float& w, const char* text );
    bool transform( ui::Layout& layout, ui::Tree& tree, kege::Entity& entity );
}
#endif /* ui_transform_hpp */
