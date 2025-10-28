//
//  ui-aligner.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 2/27/25.
//

#ifndef ui_aligner_hpp
#define ui_aligner_hpp

#include <stdlib.h>
#include "ui-core.hpp"

namespace kege::ui{

    class Layout;

    struct CoverageData
    {
        Extent2D padding;
        Extent2D bounding;
        Extent2D gap;
        Extent2D sum;
        Extent2D max;
        Extent2D region;
        int extendable_width_count;
        int extendable_height_count;
    };

    void align( Layout& layout, NodeIndex eid );
}

#endif // ui_aligner_hpp
