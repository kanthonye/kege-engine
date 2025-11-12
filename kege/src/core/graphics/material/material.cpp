//
//  material.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/21/25.
//

#include "material.hpp"

namespace kege{

    const ref::ShaderPipeline& Material::getShaderPipeline()const
    {
        return _pipeline;
    }
    
    kege::RenderPassType Material::getPass()const
    {
        return _pass;
    }

    Material::Material
    (
        kege::RenderPassType pass,
        const ref::ShaderPipeline& pipeline,
        const SetNames& binding_names
    )
    :   kege::ShaderData( pipeline, binding_names )
    ,   _parameters({})
    ,   _textures({})
    ,   _pass( pass )
    {
        setImages( "", ImageBindings{} );
        setImages( "", ImageBindings{} );
        setImages( "", ImageBindings{} );
        setImages( "", ImageBindings{} );
        setImages( "", ImageBindings{} );
    }

}
