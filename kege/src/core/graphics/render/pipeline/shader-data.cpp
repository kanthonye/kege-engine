//
//  shader-data.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/18/25.
//

#include "shader-data.hpp"
namespace kege{

//
//    bool ShaderData::set( const std::string& block_name, int index, size_t size, const void* data )
//    {
//        const kege::LookUpEntry* entry = _layout->getLookUpEntry( block_name );
//        if ( entry == nullptr )
//        {
//            return false;
//        }
//
//        if ( entry->type == kege::LookUpEntry::LAYOUT )
//        {
//            const ref::SetLayout& set = _layout->getSetLayout( entry->index );
//            if ( !set ) return false;
//            const kege::BindingInfo* binding = set->getBinding( index );
//
//            memcpy( memptr, data, size );
//        }
//        else
//        {
//            memcpy( _constants.data(), data, size );
//        }
//
//        return true;
//    }
//
//    void ShaderData::setPushBlock( size_t offset, size_t size, const void* data )
//    {
//        memcpy( _constants.data() + offset, data, size );
//    }
//
//    // --- --- --- --- --- --- --- --- --- --- --- --- --- ---
//    // Direct Buffer binding using raw set-index and binding-index
//    // --- --- --- --- --- --- --- --- --- --- --- --- --- ---
//
//    bool ShaderData::setBufferBlock( int set_index, int binding_index, const BufferBindings& bindings )
//    {
//        const Ref< ShaderBindingPointSet >& binding_point = _layout->shader_set_binding_point_layouts[ set_index ]->binding_points[ binding_index ];
//        if ( binding_point->binding_type == BindingType::BUFFER )
//        {
//            *_sets[ set_index ][ binding_index ] = bindings;
//            return true;
//        }
//        kege::Log::error << "type mismatch (trying to bind image at a buffer binding point)." << kege::Log::nl;
//        return false;
//    }
//
//    bool ShaderData::setBufferBlock( int set_index, int binding_index, const BufferInfo& binding )
//    {
//        const Ref< ShaderBindingPointSet >& binding_point = _layout->shader_set_binding_point_layouts[ set_index ]->binding_points[ binding_index ];
//        if ( binding_point->binding_type == BindingType::BUFFER )
//        {
//            *_sets[ set_index ][ binding_index ] = binding;
//            return true;
//        }
//        kege::Log::error << "type mismatch (trying to bind image at a buffer binding point)." << kege::Log::nl;
//        return false;
//    }
//
//    // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- //
//    // Buffer binding using shader block name to access set-index and binding-index
//    // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- //
//
//    bool ShaderData::setBufferBlock( const std::string& block_name, const BufferBindings& bindings )
//    {
//        ShaderBindingResrc* binding_point = getShaderBindingPoint( block_name );
//        if ( binding_point != nullptr )
//        {
//            if ( binding_point->type == ShaderBindingResrc::BUFFER )
//            {
//                *binding_point = bindings;
//                return true;
//            }
//        }
//        kege::Log::error << "type mismatch (trying to bind image at a buffer binding point)." << kege::Log::nl;
//        return false;
//    }
//
//    bool ShaderData::setBufferBlock( const std::string& block_name, const BufferInfo& binding )
//    {
//        ShaderBindingResrc* binding_point = getShaderBindingPoint( block_name );
//        if ( binding_point != nullptr )
//        {
//            if ( binding_point->type == ShaderBindingResrc::BUFFER )
//            {
//                *binding_point = binding;
//                return true;
//            }
//        }
//        kege::Log::error << "type mismatch (trying to bind image at a buffer binding point)." << kege::Log::nl;
//        return false;
//    }
//
//    // --- --- --- --- --- --- --- --- --- --- --- --- --- --- //
//    // Direct Image binding using raw set-index and binding-index
//    // --- --- --- --- --- --- --- --- --- --- --- --- --- --- //
//
//    bool ShaderData::setImageSampler( int set_index, int binding_index, const ImageBindings& binding )
//    {
//        if ( _sets[ set_index ][ binding_index ]->type != ShaderBindingResrc::IMAGE )
//        {
//            *_sets[ set_index ][ binding_index ] = binding;
//            return true;
//        }
//        kege::Log::error << "type mismatch (trying to bind buffer at a texture binding point)." << kege::Log::nl;
//        return false;
//    }
//
//    bool ShaderData::setImageSampler( int set_index, int binding_index, const ImageInfo& binding )
//    {
//        if ( _sets[ set_index ][ binding_index ]->type != ShaderBindingResrc::IMAGE )
//        {
//            *_sets[ set_index ][ binding_index ] = binding;
//            return true;
//        }
//        kege::Log::error << "type mismatch (trying to bind buffer at a texture binding point)." << kege::Log::nl;
//        return false;
//    }
//
//    // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- //
//    // Image binding using shader block name to access set-index and binding-index
//    // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- //
//
//    bool ShaderData::setImageSampler( const std::string& block_name, const std::vector< ImageInfo >& images )
//    {
//        ShaderBindingResrc* binding_point = getShaderBindingPoint( block_name );
//        if ( binding_point != nullptr )
//        {
//            if ( binding_point->type != ShaderBindingResrc::IMAGE )
//            {
//                *binding_point = images;
//                return true;
//            }
//        }
//        kege::Log::error << "type mismatch (trying to bind buffer at a texture binding point)." << kege::Log::nl;
//        return false;
//    }
//
//    bool ShaderData::setImageSampler( const std::string& block_name, const ImageInfo& images )
//    {
//        ShaderBindingResrc* binding_point = getShaderBindingPoint( block_name );
//        if ( binding_point != nullptr )
//        {
//            if ( binding_point->type != ShaderBindingResrc::IMAGE )
//            {
//                *binding_point = images;
//                return true;
//            }
//        }
//        kege::Log::error << "type mismatch (trying to bind buffer at a texture binding point)." << kege::Log::nl;
//        return false;
//    }
//
//    const uint8_t* ShaderData::cpuBlob() const { return _constants.data(); }
//    size_t ShaderData::blobSize() const { return _constants.size(); }
//
//    bool ShaderData::integrate()
//    {
//        for ( int set_index = 0 ; set_index < _layout->shader_set_binding_point_layouts.size() ; ++set_index )
//        {
//            if ( !_sets[ set_index ].gpu_handle )
//            {
//                Ref< ShaderBindingPointSetLayout > ssbl = _layout->shader_set_binding_point_layouts[ set_index ];
//                _sets[ set_index ].gpu_handle = ssbl->allocateShaderBindingSet( _sets[ set_index ] );
//                if (!_sets[ set_index ].gpu_handle) return false;
//            }
//            else
//            {
//                _sets[ set_index ].gpu_handle->update( _sets[ set_index ].resources );
//            }
//        }
//        return true;
//    }
//
//    // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- //
//    // Constructor
//    // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- //
//
//    ShaderData::ShaderData( Ref< const ShaderLayout > layout )
//    :   _layout( layout )
//    {
//        /*
//        _resources.resize( layout->shader_set_binding_layouts.size() );
//        for (int i=0; i<layout->shader_set_binding_layouts.size(); ++i )
//        {
//            Ref< ShaderBindingPointSetLayout > ssbl = layout->shader_set_binding_layouts[i];
//            _resources[i].binding_points.resize( ssbl->binding_points.size() );
//        }
//         */
//
//        size_t push_block_const_size = 0;
//        for (int i=0; i<layout->shader_constant_binding_points.size(); ++i )
//        {
//            push_block_const_size += layout->shader_constant_binding_points[i]->size;
//        }
//        _constants.resize( push_block_const_size );
//    }
//
//    // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- //
//    // Destructor
//    // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- //
//
//    ShaderData::~ShaderData()
//    {
//        _sets.clear();
//        _layout.clear();
//    }
//
//    ShaderBindingResrc* ShaderData::getShaderBindingPoint( const std::string& block_name )
//    {
//        const ShaderBindingPoint* block = getBlock( block_name );
//        if ( block == nullptr )
//        {
//            kege::Log::error << "Undefined ShaderBindingPoint name [ "<< block_name <<" ]" << kege::Log::nl;
//            return nullptr;
//        }
//
//        if ( block->type != ShaderBindingType::SET )
//        {
//            kege::Log::error << "Current ShaderBindingPoint name [ "<< block_name <<" ] is not a set." << kege::Log::nl;
//            return nullptr;
//        }
//
//        ShaderBindingPointSet* set = (ShaderBindingPointSet*) block;
//        if ( _sets[ set->set ].resources.empty() )
//        {
//            size_t size = _layout->shader_set_binding_point_layouts[ set->set ]->binding_points.size();
//            _sets[ set->set ].resources.resize( size );
//        }
//        return _sets[ set->set ][ set->binding ].ref();
//    }
//
//    const ShaderStructField* ShaderData::getField( const ShaderBindingPoint* block, const std::string& field_name )const
//    {
//        const ShaderStructField* field = block->struct_block->get( field_name );
//        if ( field == nullptr )
//        {
//            kege::Log::error << "ShaderBindingPoint[ " << block->name;
//            kege::Log::error << " ] does not contain field[ " << field_name;
//            kege::Log::error << " ]" << kege::Log::nl;
//            return nullptr;
//        }
//        return field;
//    }
//
//    const ShaderBindingPoint* ShaderData::getBlock( const std::string& block_name )const
//    {
//        auto m = _layout->quick_lookup_map.find( block_name );
//        if ( m == _layout->quick_lookup_map.end() )
//        {
//            kege::Log::error << "Block[ " << block_name << " ] does not exist in ShaderLayout[ ";
//            kege::Log::error << _layout->name <<" ]" << kege::Log::nl;
//            return {};
//        }
//        return _layout->shader_binding_blocks[ m->second.index ];
//    }
//
//    int8_t* ShaderData::getBufferBlockDataPointer( int index, const ShaderBindingPoint* block )
//    {
//        ShaderBindingPointSet* set = (ShaderBindingPointSet*) block;
//        if( set->binding_type != kege::BindingType::BUFFER )
//        {
//            kege::Log::error << "Type mismatch. Binding point -> [ " << set->name <<" ] is not a buffer binding point " << kege::Log::nl;
//            return nullptr;
//        }
//        BufferInfo* info = _sets[ set->set ][ set->binding ]->buffer( index );
//        return ((int8_t*) info->buffer->data()) + info->offset;
//    }

}
