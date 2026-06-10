//
//  shader-resource-set-layout.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/31/25.
//

#include "shader-resource-set-layout.hpp"

namespace kege{

    const kege::ShaderSetDesc& ShaderResourceSetLayout::getDesc()const
    {
        return _bindings;
    }

    kege::UniformKey ShaderResourceSetLayout::getUniformKey()const
    {
        return _global_uniform_key;
    }

    const std::string& ShaderResourceSetLayout::getName()const
    {
        return _name;
    }

    uint32_t ShaderResourceSetLayout::getId()const
    {
        return _id;
    }

    ShaderResourceSetLayout::~ShaderResourceSetLayout()
    {
    }

    ShaderResourceSetLayout::ShaderResourceSetLayout
    (
        kege::UniformKey key,
        uint32_t size,
        const kege::ShaderSetBindingDesc* descs
    )
    :   _global_uniform_key( key )
    {
        _bindings.resize( size );
        for (int i=0; i<size; ++i)
        {
            _bindings[i] = descs[i];
        }
    }

}

