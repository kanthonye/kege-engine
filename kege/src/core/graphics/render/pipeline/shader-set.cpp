//
//  shader-binding-set.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/18/25.
//

#include "shader-set.hpp"
#include "set-layout.hpp"

namespace kege{

    const kege::SetLayout* ShaderSet::getSetLayout()const
    {
        return _layout;
    }

    ShaderSet::ShaderSet( SetLayout* layout )
    :   _layout( layout )
    {
        //_binding_slots.resize( layout->count() );
    }
    
    ShaderSet:: ~ShaderSet()
    {
    }
}
