//
//  render-queue.cpp
//  physics
//
//  Created by Kenneth Esdaile on 9/9/25.
//

#include "render-queue.hpp"

namespace kege{

    const RenderPassQueue& RenderQueue::getQueueForPass( RenderPassType pass ) const
    {
        static const RenderPassQueue emptyQueue;
        auto it = _queues.find( pass );
        if (it != _queues.end())
        {
            return it->second;
        }
        return emptyQueue;
    }

    void RenderQueue::submit( const RenderObject& object )
    {
        _queues[ object.material->pass ].objects[ object.material->pipeline ].push_back( object );
    }

    void RenderQueue::clear()
    {
        for (auto& pair : _queues)
        {
            pair.second.objects.clear();
        }
    }

}

