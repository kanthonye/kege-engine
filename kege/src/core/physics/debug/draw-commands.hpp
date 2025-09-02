//
//  draw-commands.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/21/25.
//

#ifndef draw_commands_hpp
#define draw_commands_hpp

#include "../../../core/utils/communication.hpp"
#include "../../../core/math/algebra/vmath.hpp"
#include "../../../core/math/geometry/primitive-3D-shapes.hpp"

namespace kege{

    struct MsgDrawRect
    {
        Rect rect;
    };

    struct MsgDrawOBB
    {
        OBB obb;
    };

    struct MsgDrawLine
    {
        Line line;
    };

    struct MsgDrawAABB
    {
        AABB aabb;
    };


    void drawRect( const Rect& rect );
    void drawAABB( const AABB& aabb );
    void drawOBB( const OBB& obb );
    void drawRay( const Ray& ray );
    void drawLine( const Line& line );

}
#endif /* draw_commands_hpp */
