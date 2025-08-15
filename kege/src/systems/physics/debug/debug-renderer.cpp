//
//  debug-renderer.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 4/19/25.
//

#include "debug-renderer.hpp"

namespace kege {

    void DebugLineRenderSystem::operator()( kege::RenderPassContext* context )
    {
        kege::PipelineHandle pipeline = context->getGraphics()->getShaderPipelineManager()->get( "line-shader" );
        if ( !pipeline ) return;

        DescriptorSetHandle camera_descriptor;
        if ( !pipeline ) return;

        CommandEncoder* encoder = context->getCommandBuffer()->createCommandEncoder();
        encoder->bindGraphicsPipeline( pipeline );
        encoder->bindDescriptorSets( camera_descriptor );
        encoder->bindVertexBuffers( 0, { _vbo }, { 0 } );
        encoder->bindIndexBuffer( _ibo, 0, false );
        encoder->drawIndexed( _icount, 1, 0, 0, 0 );
        reset();
    }

    void DebugLineRenderSystem::operator()( const MsgDrawRect& command )
    {
        drawRect( command.rect );
    }

    void DebugLineRenderSystem::operator()( const MsgDrawLine& command )
    {
        drawLine( command.line );
    }

    void DebugLineRenderSystem::operator()( const MsgDrawAABB& command )
    {
        drawAABB( command.aabb );
    }

    void DebugLineRenderSystem::operator()( const MsgDrawOBB& command )
    {
        drawOBB( command.obb );
    }

    void DebugLineRenderSystem::drawRect( const kege::Rect& rect )
    {
        vec3 ex = rect.extents.x * rect.axie[0];
        vec3 ey = rect.extents.y * rect.axie[1];

        kege::vec3 corners[4];
        corners[ 0 ] = rect.center + ex + ey;
        corners[ 1 ] = rect.center + ex + ey;
        corners[ 2 ] = rect.center + ex - ey;
        corners[ 3 ] = rect.center + ex - ey;

        uint32_t indices[5];
        indices[ 0 ] = _vcount;
        indices[ 1 ] = _vcount + 1;
        indices[ 2 ] = _vcount + 2;
        indices[ 3 ] = _vcount + 3;
        indices[ 4 ] = 0xFFFFFFFF;

        uint64_t vsize = sizeof( corners );
        uint64_t isize = sizeof( indices );

        pushVertices(4, vsize, corners);
        pushIndices(5, isize, indices);
    }

    void DebugLineRenderSystem::drawBox( const kege::vec3 corners[8] )
    {
        uint32_t indices[24];
        indices[ 0 ] = _vcount;
        indices[ 1 ] = _vcount + 1;
        indices[ 2 ] = _vcount + 2;
        indices[ 3 ] = _vcount + 3;
        indices[ 4 ] = indices[ 0 ];
        indices[ 5 ] = 0xFFFFFFFF;

        indices[  6 ] = _vcount + 4;
        indices[  7 ] = _vcount + 5;
        indices[  8 ] = _vcount + 6;
        indices[  9 ] = _vcount + 7;
        indices[ 10 ] = indices[ 6 ];
        indices[ 11 ] = 0xFFFFFFFF;

        indices[ 12 ] = _vcount;
        indices[ 13 ] = _vcount + 4;
        indices[ 14 ] = 0xFFFFFFFF;

        indices[ 15 ] = _vcount + 1;
        indices[ 16 ] = _vcount + 5;
        indices[ 17 ] = 0xFFFFFFFF;

        indices[ 18 ] = _vcount + 2;
        indices[ 19 ] = _vcount + 6;
        indices[ 20 ] = 0xFFFFFFFF;

        indices[ 21 ] = _vcount + 3;
        indices[ 22 ] = _vcount + 7;
        indices[ 23 ] = 0xFFFFFFFF;

        uint64_t vsize = 8 * sizeof( kege::vec3 );
        uint64_t isize = sizeof( indices );

        pushVertices(8, vsize, corners);
        pushIndices(24, isize, indices);
    }

    void DebugLineRenderSystem::drawLine( const kege::Line& line )
    {
        if ( _vcount + 2 >= MAX_VERTEX_COUNT ) return;
        if ( _icount + 3 >= MAX_INDICE_COUNT ) return;

        uint32_t indices[3] = {_vcount, _vcount + 1, 0xFFFFFFFF};
        uint64_t size = sizeof(Line);
        
        pushVertices(2, size, &line);
        pushIndices(3, 12, indices);
    }

    void DebugLineRenderSystem::drawAABB( const kege::AABB& aabb )
    {
        if ( _vcount + 8  >= MAX_VERTEX_COUNT ) return;
        if ( _icount + 22 >= MAX_INDICE_COUNT ) return;

        kege::vec3 corners[8];
        corners[ 0 ] = kege::vec3(aabb.max.x, aabb.max.y, aabb.max.z);
        corners[ 1 ] = kege::vec3(aabb.max.x, aabb.max.y, aabb.min.z);
        corners[ 2 ] = kege::vec3(aabb.max.x, aabb.min.y, aabb.min.z);
        corners[ 3 ] = kege::vec3(aabb.max.x, aabb.min.y, aabb.max.z);

        corners[ 4 ] = kege::vec3(aabb.min.x, aabb.max.y, aabb.max.z);
        corners[ 5 ] = kege::vec3(aabb.min.x, aabb.max.y, aabb.min.z);
        corners[ 6 ] = kege::vec3(aabb.min.x, aabb.min.y, aabb.min.z);
        corners[ 7 ] = kege::vec3(aabb.min.x, aabb.min.y, aabb.max.z);

        drawBox( corners );
    }

    void DebugLineRenderSystem::drawOBB( const kege::OBB& obb )
    {
        vec3 ex = obb.extents.x * obb.axes[0];
        vec3 ey = obb.extents.y * obb.axes[1];
        vec3 ez = obb.extents.z * obb.axes[2];

        kege::vec3 corners[8];
        corners[ 0 ] = obb.center + ex + ey + ez;
        corners[ 1 ] = obb.center + ex + ey - ez;
        corners[ 2 ] = obb.center + ex - ey - ez;
        corners[ 3 ] = obb.center + ex - ey + ez;

        corners[ 4 ] = obb.center - ex + ey + ez;
        corners[ 5 ] = obb.center - ex + ey - ez;
        corners[ 6 ] = obb.center - ex - ey - ez;
        corners[ 7 ] = obb.center - ex - ey + ez;
        drawBox( corners );
    }

    void DebugLineRenderSystem::pushVertices( uint32_t count, size_t size, const void* data )
    {
        if ( _vsize + size < _vbo_capacity )
        {
            _engine->getGraphics()->updateBuffer( _vbo, _isize, size, data );
            //_vbo->writeBufferData( _vsize, size, data );
            _vsize += size;
            _vcount += count;
        }
    }
    void DebugLineRenderSystem::pushIndices( uint32_t count, size_t size, const void* data )
    {
        if ( _isize + size < _ibo_capacity )
        {
            _engine->getGraphics()->updateBuffer( _ibo, _isize, size, data );
            //_ibo->writeBufferData( _isize, size, data );
            _isize += size;
            _icount += count;
        }
    }

    bool DebugLineRenderSystem::initialize()
    {
        Communication::add< kege::RenderPassContext*, DebugLineRenderSystem>( this );
        Communication::add< const MsgDrawRect&, DebugLineRenderSystem >( this );
        Communication::add< const MsgDrawLine&, DebugLineRenderSystem >( this );
        Communication::add< const MsgDrawAABB&, DebugLineRenderSystem >( this );
        Communication::add< const MsgDrawOBB&,  DebugLineRenderSystem >( this );

        _vbo_capacity = MAX_VERTEX_COUNT * sizeof( vec3 );
        _ibo_capacity = MAX_VERTEX_COUNT * sizeof( uint32_t );

        _vbo = _engine->getGraphics()->createBuffer
        ({
            .size = _vbo_capacity,
            .data = nullptr,
            .usage = kege::BufferUsage::VertexBuffer,
            .memory_usage = kege::MemoryUsage::CpuToGpu
        });
        _ibo = _engine->getGraphics()->createBuffer
        ({
            .size = _vbo_capacity,
            .data = nullptr,
            .usage = kege::BufferUsage::IndexBuffer,
            .memory_usage = kege::MemoryUsage::CpuToGpu
        });

        return true;
    }

    void DebugLineRenderSystem::shutdown()
    {
        Communication::remove< kege::RenderPassContext*, DebugLineRenderSystem >( this );
        Communication::remove< const MsgDrawRect&, DebugLineRenderSystem >( this );
        Communication::remove< const MsgDrawLine&, DebugLineRenderSystem >( this );
        Communication::remove< const MsgDrawAABB&, DebugLineRenderSystem >( this );
        Communication::remove< const MsgDrawOBB&,  DebugLineRenderSystem >( this );
        _engine->getGraphics()->destroyBuffer( _vbo );
        _engine->getGraphics()->destroyBuffer( _ibo );
    }

    void DebugLineRenderSystem::render()
    {}

    void DebugLineRenderSystem::reset()
    {
        _vcount = 0;
        _vsize = 0;
        _icount = 0;
        _isize = 0;
    }

    DebugLineRenderSystem::DebugLineRenderSystem( kege::Engine* engine )
    :   kege::System( engine, "line-renderer", 0 )
    ,   _vcount( 0 )
    ,   _vsize( 0 )
    ,   _icount( 0 )
    ,   _isize( 0 )
    {}

    DebugLineRenderSystem::~DebugLineRenderSystem()
    {
        shutdown();
    }


    KEGE_REGISTER_SYSTEM( DebugLineRenderSystem, "drawline" );
}


