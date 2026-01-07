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
        static void align( Layout& layout, uint32_t pid );
    };


    inline Alignment getAlignment(const Widget* p)
    {
        if (p->style)
        {
            return p->style->align;
        }
        return p->alignment;
//        return Alignment
//        {
//            .flow.x = AlignDirX::WTE,
//            .flow.y = AlignDirY::NTS,
//            .origin.x = AlignPosX::LEFT,
//            .origin.y = AlignPosY::TOP,
//            .content.x = AlignPosX::LEFT,
//            .content.y = AlignPosY::TOP,
//            .direction = AlignDir::HORIZONTAL,
//            .wrap_around = true,
//        };
    }
}

#endif // ui_aligner_hpp
