//
//  shader-set-binding-layout.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/18/25.
//

#include "shader-set-binding-point-layout.hpp"

namespace kege {

    const ShaderStructField* ShaderSetBindingPointLayout::getField( const std::string& name, const std::string& field )const
    {
        auto i = quick_lookup_map.find( name );
        if ( i == quick_lookup_map.end() )
            return nullptr;
        return binding_points[ i->second ]->struct_block->get( field );
    }

    ShaderStructField* ShaderSetBindingPointLayout::getField( const std::string& name, const std::string& field )
    {
        auto i = quick_lookup_map.find( name );
        if ( i == quick_lookup_map.end() )
            return nullptr;
        return binding_points[ i->second ]->struct_block->get( field );
    }

    const ShaderSetBindingPoint* ShaderSetBindingPointLayout::get( const std::string& name )const
    {
        auto i = quick_lookup_map.find( name );
        if ( i == quick_lookup_map.end() )
            return nullptr;
        return binding_points[ i->second ].ref();
    }

    ShaderSetBindingPoint* ShaderSetBindingPointLayout::get( const std::string& name )
    {
        auto i = quick_lookup_map.find( name );
        if ( i == quick_lookup_map.end() )
            return nullptr;
        return binding_points[ i->second ].ref();
    }

    const ShaderSetBindingPoint* ShaderSetBindingPointLayout::getBinding( int binding_index )const
    {
        return binding_points[ binding_index ].ref();
    }

    ShaderSetBindingPoint* ShaderSetBindingPointLayout::getBinding( int binding_index )
    {
        return binding_points[ binding_index ].ref();
    }

    size_t ShaderSetBindingPointLayout::count()const
    {
        return binding_points.size();
    }

    ShaderSetBindingPointLayout:: ~ShaderSetBindingPointLayout()
    {
        quick_lookup_map.clear();
        binding_points.clear();
    }

}
