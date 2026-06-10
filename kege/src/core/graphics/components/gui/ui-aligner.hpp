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

    class Aligner
    {
        public:

        static void advanceDownward( Layout& layout, const Widget& parent );
        static void advanceUpward( Layout& layout, const Widget& parent );
        static void advanceToTheRight( Layout& layout, const Widget& parent );
        static void advanceToTheLeft( Layout& layout, const Widget& parent );
        static void centerAll( Layout& layout, const Widget& parent );
        static void align( Layout& layout, uint32_t pid );
    };

}

#endif // ui_aligner_hpp
