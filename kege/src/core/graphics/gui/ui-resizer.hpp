//
//  ui-resizer.hpp
//  editor
//
//  Created by Kenneth Esdaile on 12/10/25.
//

#ifndef ui_resizer_hpp
#define ui_resizer_hpp

#include <stdlib.h>
#include "ui-core.hpp"

namespace kege::ui{

    class Resizer
    {
        public:
        static void resize( Layout& layout, uint32_t widget_index );
    };

}

#endif /* ui_resizer_hpp */
