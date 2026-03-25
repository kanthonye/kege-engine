//
//  set-layout.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/30/25.
//

#include "set-layout.hpp"

namespace kege {

    const ShaderStructField* SetLayout::getField( const std::string& name, const std::string& field )const
    {
        auto i = _binding_set_lookup.find( name );
        if ( i == _binding_set_lookup.end() )
            return nullptr;
        return _bindings[ i->second ].block->get( field );
    }

    ShaderStructField* SetLayout::getField( const std::string& name, const std::string& field )
    {
        auto i = _binding_set_lookup.find( name );
        if ( i == _binding_set_lookup.end() )
            return nullptr;
        return _bindings[ i->second ].block->get( field );
    }

    const kege::BindPointDesc* SetLayout::getBinding( const std::string& name )const
    {
        auto i = _binding_set_lookup.find( name );
        if ( i == _binding_set_lookup.end() )
            return nullptr;
        return getBinding(  i->second );
    }

    kege::BindPointDesc* SetLayout::getBinding( const std::string& name )
    {
        auto i = _binding_set_lookup.find( name );
        if ( i == _binding_set_lookup.end() )
            return nullptr;
        return getBinding(  i->second );
    }

    const kege::BindPointDesc* SetLayout::getBinding( int binding_index )const
    {
        return &_bindings[ binding_index ];
    }

    kege::BindPointDesc* SetLayout::getBinding( int binding_index )
    {
        return &_bindings[ binding_index ];
    }

    const std::string& SetLayout::getName()const
    {
        return _name;
    }

    int SetLayout::totalBindingCount()const
    {
        return int( _bindings.size() );
    }

    int SetLayout::totalBufferCount()const
    {
        return _total_buffers;
    }

    int SetLayout::totalImageCount()const
    {
        return _total_images;
    }

    size_t SetLayout::count()const
    {
        return _bindings.size();
    }

    SetLayout::SetLayout( const kege::BindPointDescs& bindings )
    :   _bindings( bindings )
    ,   _total_buffers( 0 )
    ,   _total_images( 0 )
    {
        for (int i = 0; i < _bindings.size(); ++i)
        {
            _binding_set_lookup[ _bindings[i].name ] = i;
            switch ( _bindings[i].usage )
            {
                case kege::BindingUsage::UniformBuffer:
                case kege::BindingUsage::UniformBufferDynamic:
                case kege::BindingUsage::UniformTexelBuffer:
                case kege::BindingUsage::StorageBuffer:
                case kege::BindingUsage::StorageBufferDynamic:
                case kege::BindingUsage::StorageTexelBuffer:
                    _total_buffers += _bindings[i].count;
                    _bindings[i].type = kege::BindType::Buffer;
                    break;

                case kege::BindingUsage::CombinedImageSampler:
                case kege::BindingUsage::StorageImage:
                case kege::BindingUsage::SampledImage:
                case kege::BindingUsage::InputAttachment:
                case kege::BindingUsage::Sampler:
                    _total_images += _bindings[i].count;
                    _bindings[i].type = kege::BindType::Image;
                    break;

                default: break;
            }
        }
    }

    SetLayout:: ~SetLayout()
    {
        _binding_set_lookup.clear();
        _bindings.clear();
    }

}
