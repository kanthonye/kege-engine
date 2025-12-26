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
        static void resolveSizing(uint32_t parent, const Sizing& sizing, float& size, float available_space, int& extend_count );
        static void calcHorizontalExtent( Layout& layout, uint32_t widget_index );
        static void calcVerticalExtent( Layout& layout, uint32_t widget_index );
        static void calcHorizontalFlex( Layout& layout, uint32_t widget_index );
        static void calcVerticalFlex( Layout& layout, uint32_t widget_index );
        static void resize( Layout& layout, uint32_t widget_index );
        static void resize( Layout& layout );
    };

}

#endif /* ui_resizer_hpp */
