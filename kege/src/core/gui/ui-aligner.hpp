//
//  ui-aligner.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 2/27/25.
//

#ifndef ui_aligner_hpp
#define ui_aligner_hpp

#include <stdlib.h>
#include "ui-primatives.hpp"

namespace kege::ui{

    class Layout;

    class Aligner
    {
    public:

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
//
//
//        float getFixedWidth( Layout& layout, uint32_t pid, uint32_t cid, CoverageData& data );
//        float getFixedHeight( Layout& layout, uint32_t parent, uint32_t child, CoverageData& data );
//        void integrateCoverageHorizontally( Layout& layout, uint32_t parent, uint32_t child, CoverageData& data );
//        void integrateCoverageVertically( Layout& layout, uint32_t parent, uint32_t child, CoverageData& data );
//        void computeCoverageData( Layout& layout, uint32_t pid, CoverageData& data );
//
//        /**
//         * @brief
//         * Compute the lengths of a flexible ui element. flexible elements adjust their width and height
//         * to have a minimum fit to enclose its children.
//         *
//         * @param eid The ui element we need to compute the dimensions for.
//         */
//        void computeFlexibleLengths( Layout& layout, uint32_t eid );
//
//        Extent2D computeExtent( Layout& layout, uint32_t pid );
//
//        kege::vec2 getAlignmentStartingPosition( Layout& layout, uint32_t pid, float width, float height );
//        //void align( Layout& layout, uint32_t pid, uint32_t cid, const kege::vec2& start, kege::vec2& length, const Extent2D& extent );
//        void alignChildren( Layout& layout, uint32_t pid, Extent2D& extent );
//
//        void alignElements( Layout& layout, uint32_t eid );
//
//        bool shouldPreCalcFlexibleSize( Layout& layout, uint32_t eid )const;
//
//        vec2 getAlignmentStartingPosition( Layout& layout, uint32_t cid, const Extent2D& extent, uint32_t pid );
//        Extent2D getColumnExtent( Layout& layout, uint32_t curr_index, uint32_t pid );
//        Extent2D getRowExtent( Layout& layout, uint32_t curr_index, uint32_t pid );
//        void alignHorizontally( Layout& layout, uint32_t pid );
//        void alignVertically( Layout& layout, uint32_t pid );
//
//        
        void align( Layout& layout, NodeIndex eid );
        Aligner();
    };

}

#endif // ui_aligner_hpp
