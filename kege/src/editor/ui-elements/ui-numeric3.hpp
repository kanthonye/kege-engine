//
//  ui-numeric3.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/25/25.
//

#ifndef ui_numeric3_hpp
#define ui_numeric3_hpp

#include "ui-numeric.hpp"

namespace kege::ui{

    bool orientation
    (
        ui::Layout& layout,
        TreeNode& tree,
        float& x, float& y, float& z, float& w,
        const char* text
    );

    bool transform
    (
        ui::Layout& layout,
        TreeNode& tree,
        kege::Transform* transform
    );

    bool rigidbody( ui::Layout& layout, TreeNode& tree, kege::Rigidbody* body );


    bool camera( ui::Layout& layout, TreeNode& tree, Camera* camera );
}

#endif /* ui_numeric3_hpp */
