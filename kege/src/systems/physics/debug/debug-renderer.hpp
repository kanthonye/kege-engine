//
//  debug-renderer.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 4/19/25.
//

#ifndef debug_renderer_hpp
#define debug_renderer_hpp

#include "../../../core/graphics/graph/render-graph.hpp"
#include "../../../core/system/system.hpp"
#include "../../../core/engine/engine.hpp"
#include "draw-commands.hpp"

namespace kege {

    class DebugLineRenderSystem : public kege::System
    {
    public:

        enum{ MAX_VERTEX_COUNT = 32768, MAX_INDICE_COUNT = 65536 };

        void operator()( kege::RenderPassContext* context );
        void operator()( const MsgDrawRect& command );
        void operator()( const MsgDrawLine& command );
        void operator()( const MsgDrawAABB& command );
        void operator()( const MsgDrawOBB& command );
        void drawBox( const kege::vec3 points[8] );

        void drawRect( const kege::Rect& line );
        void drawLine( const kege::Line& line );
        void drawAABB( const kege::AABB& obb );
        void drawOBB( const kege::OBB& obb );

        bool initialize();
        void shutdown();

        void render();

        void reset();
        ~ DebugLineRenderSystem();
        DebugLineRenderSystem( kege::Engine* engine );

    private:

        void pushVertices( uint32_t count, size_t size, const void* data );
        void pushIndices( uint32_t count, size_t size, const void* data );

        kege::Graphics* _graphics;
        uint64_t _vbo_capacity;
        uint64_t _ibo_capacity;
        
        kege::BufferHandle _vbo;
        kege::BufferHandle _ibo;

        uint64_t _vsize;
        uint64_t _isize;

        uint32_t _vcount;
        uint32_t _icount;
    };

}
#endif /* debug_renderer_hpp */
