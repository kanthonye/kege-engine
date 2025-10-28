//
//  ui-rigidbody.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/24/25.
//

#ifndef ui_rigidbody_hpp
#define ui_rigidbody_hpp

#include "ui-numeric.hpp"
#include "../../core/physics/3d/bodies/rigidbody.hpp"

namespace kege::ui
{
    bool rigidbody( ui::Layout& layout, ui::Tree& tree, kege::Entity& entity );
}
#endif /* ui_rigidbody_hpp */
