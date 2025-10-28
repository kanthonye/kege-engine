//
//  shader-layout.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/18/25.
//

#include "shader-layout.hpp"

namespace kege{

    const ShaderStructField* ShaderLayout::getField( const std::string& name, const std::string& field )const
    {
        auto i = quick_lookup_map.find( name );
        if ( i == quick_lookup_map.end() )
            return nullptr;
        return shader_binding_blocks[ i->second.index ]->struct_block->get( field );
    }

    ShaderStructField* ShaderLayout::getField( const std::string& name, const std::string& field )
    {
        auto i = quick_lookup_map.find( name );
        if ( i == quick_lookup_map.end() )
            return nullptr;
        return shader_binding_blocks[ i->second.index ]->struct_block->get( field );
    }

    const ShaderSetBindingPoint* ShaderLayout::get( const std::string& name )const
    {
        auto i = quick_lookup_map.find( name );
        if ( i == quick_lookup_map.end() )
            return nullptr;
        if ( i->second.type == LAYOUT )
        {
            return (const ShaderSetBindingPoint*) shader_binding_blocks[ i->second.index ];
        }
        return nullptr;
    }

    ShaderSetBindingPoint* ShaderLayout::get( const std::string& name )
    {
        auto i = quick_lookup_map.find( name );
        if ( i == quick_lookup_map.end() )
            return nullptr;
        if ( i->second.type == LAYOUT )
        {
            return (ShaderSetBindingPoint*) shader_binding_blocks[ i->second.index ];
        }
        return nullptr;
    }

    const ShaderSetBindingPoint* ShaderLayout::getBinding( int set_index, int binding_index )const
    {
        return shader_set_binding_point_layouts[ set_index ]->getBinding( binding_index );
    }

    ShaderSetBindingPoint* ShaderLayout::getBinding( int set_index, int binding_index )
    {
        return shader_set_binding_point_layouts[ set_index ]->getBinding( binding_index );
    }

    ShaderLayout::ShaderLayout
    (
        const std::string& name,
        std::vector< ref::ShaderSetBindingPointLayout >& shader_set_binding_layouts,
        std::vector< Ref< ShaderConstantBindingPoint > >& push_constant_blocks
    )
    :   shader_set_binding_point_layouts( shader_set_binding_layouts )
    ,   shader_constant_binding_points( push_constant_blocks )
    ,   name( name )
    {
        size_t count = push_constant_blocks.size();
        for (int i=0; i<shader_set_binding_layouts.size(); ++i)
        {
            count += shader_set_binding_layouts[i]->binding_points.size();
        }

        size_t index = 0;
        shader_binding_blocks.resize( count );
        for (int i=0; i<shader_set_binding_layouts.size(); ++i)
        {
            for (int j=0; j<shader_set_binding_layouts[i]->count(); ++j)
            {
                shader_binding_blocks[ index ] = shader_set_binding_layouts[i]->binding_points[j].ref();
                quick_lookup_map[ shader_binding_blocks[ index ]->name ] = LookUpEntry
                {
                    .type = LookUpType::LAYOUT,
                    .index = index
                };
                index += 1;
            }
        }
        
        for (int i=0; i<push_constant_blocks.size(); ++i)
        {
            shader_binding_blocks[ index ] = push_constant_blocks[i].ref();
            quick_lookup_map[ shader_binding_blocks[ index ]->name ] = LookUpEntry
            {
                .type = LookUpType::PUSH_CONSTANT,
                .index = index
            };
            index += 1;
        }
    }


    ShaderLayout:: ~ShaderLayout()
    {}

}
