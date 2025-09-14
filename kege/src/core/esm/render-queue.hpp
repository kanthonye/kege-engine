//
//  render-queue.hpp
//  physics
//
//  Created by Kenneth Esdaile on 9/9/25.
//

#ifndef render_queue_hpp
#define render_queue_hpp

#include <vector>
#include <map>
#include "../graphics/mesh/mesh.hpp"
#include "../renderer/core/graphics.hpp"

namespace kege{

    // Enhanced RenderObject with pass support
    struct RenderObject
    {
        Ref< MaterialSource > material;
        Ref< MeshSource > mesh;
        Transform transform;
    };

    // A container for objects in a specific pass
    struct RenderPassQueue
    {
        std::vector< RenderObject > objects;
    };

    // The main render queue that organizes objects by pass
    class RenderQueue
    {
    public:

        const RenderPassQueue& getQueueForPass( RenderPassType pass ) const;
        void submit( const RenderObject& object );
        void clear();

    private:

        std::map< RenderPassType, RenderPassQueue > _queues;
    };

}

#endif /* render_queue_hpp */
