//
//  shader-resource-set.cpp
//  editor
//
//  Created by Kenneth Esdaile on 4/28/26.
//

#include "shader-resource-set.hpp"
namespace kege{

    const kege::Binding* ShaderResourceSet::getBinding( uint32_t binding_index )const
    {
        return &_bindings[ binding_index ];
    }

    kege::Binding* ShaderResourceSet::getBinding( uint32_t binding_index )
    {
        return &_bindings[ binding_index ];
    }
}
