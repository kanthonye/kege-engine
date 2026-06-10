//
//  buffer-usages.cpp
//  editor
//
//  Created by Kenneth Esdaile on 5/3/26.
//

#include "../../../../../utils/log.hpp"
#include "buffer-usages.hpp"

namespace kege{

    BufferUsages stringToBufferUsage( const std::string& name )
    {
        static std::map< std::string, BufferUsages > types;
        if ( types.empty() )
        {
            types[ "None" ] = BufferUsages::None;
            types[ "CopySrc" ] = BufferUsages::CopySrc;
            types[ "CopyDst" ] = BufferUsages::CopyDst;
            types[ "VertexBuffer" ] = BufferUsages::VertexBuffer;
            types[ "IndexBuffer" ] = BufferUsages::IndexBuffer;
            types[ "UniformBuffer" ] = BufferUsages::UniformBuffer;
            types[ "StorageBuffer" ] = BufferUsages::StorageBuffer;
            types[ "IndirectBuffer" ] = BufferUsages::IndirectBuffer;
            types[ "UniformTexelBuffer" ] = BufferUsages::UniformTexelBuffer;
            types[ "StorageTexelBuffer" ] = BufferUsages::StorageTexelBuffer;
            types[ "ShaderDeviceAddress" ] = BufferUsages::ShaderDeviceAddress;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }
        kege::Log::error << "unsupported BufferUsages -> " <<name <<kege::Log::nl;
        return {};
    }

}
