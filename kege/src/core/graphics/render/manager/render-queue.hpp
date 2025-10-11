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
#include "../../mesh/mesh.hpp"
#include "../../../renderer/core/graphics.hpp"

namespace kege{

    struct PushConstantBlock
    {
        float data[64];
        int offset;
        int size;
        ShaderStage stages = ShaderStage::AllGraphics;
    };

    struct RenderObject
    {
        Ref< MaterialSource > material;
        Ref< MeshSource > mesh;
        PushConstantBlock constant;
    };

    // A container for objects in a specific pass
    struct RenderPassQueue
    {
        typedef std::pair< ShaderPipeline, std::vector< RenderObject > > RenderObjects;
        typedef std::map< int, RenderObjects > RenderObjectMap;
        typedef RenderObjectMap::const_iterator iterator;
        
        RenderObjectMap objects;
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
