//
//  ui-numeric.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/25/25.
//

#ifndef ui_numeric_hpp
#define ui_numeric_hpp

#include "../../core/engine/engine.hpp"
#include "../../core/gui/ui-input.hpp"
#include "../../core/gui/ui-layout.hpp"

namespace kege::ui{

    struct TreeNode
    {
        int cursor;
        bool state[2];
        std::string name;
        std::vector< ui::EID > elements;
        std::vector< TreeNode > children;
    };


    void setupDroplist( ui::Layout& layout, TreeNode& tree, const char* text );
    bool droplistOpen( ui::Layout& layout, TreeNode& tree );

    ui::EID makeRow( ui::Layout& layout );
    ui::EID makeCol( ui::Layout& layout );

    bool numeric
    (
        ui::Layout& layout,
        TreeNode& tree,
        float& num,
        const char* text
    );

    bool numeric2
    (
        ui::Layout& layout,
        TreeNode& tree,
        float& x, float& y,
        const char* text
    );

    bool numeric3
    (
        ui::Layout& layout,
        TreeNode& tree,
        float& x, float& y, float& z,
        const char* text
    );

    bool numeric4
    (
        ui::Layout& layout,
        TreeNode& tree,
        float& x, float& y, float& z, float& w,
        const char* text
    );

    
    bool numericD( ui::Layout& layout, TreeNode& tree, double& num, const char* text );
}

#endif /* ui_numeric_hpp */
