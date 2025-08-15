//
//  draw-commands.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/21/25.
//

#include "draw-commands.hpp"

namespace kege{

    void drawRect( const Rect& rect )
    {
        Communication::broadcast< const MsgDrawRect& >(MsgDrawRect{ rect });
    }

    void drawAABB( const AABB& aabb )
    {
        Communication::broadcast< const MsgDrawAABB& >(MsgDrawAABB{ aabb });
    }

    void drawOBB( const OBB& obb )
    {
        Communication::broadcast< const MsgDrawOBB& >(MsgDrawOBB{ obb });
    }

    void drawRay( const Ray& ray )
    {
        drawLine(Line{ ray.origin, ray.direction * 256.f });
    }
    
    void drawLine( const Line& line )
    {
        Communication::broadcast< const MsgDrawLine& >(MsgDrawLine{ line });
    }
}
