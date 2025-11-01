//
//  shader-data.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/18/25.
//

#ifndef shader_data_hpp
#define shader_data_hpp

#include "shader-layout.hpp"

namespace kege{

    /**
     * @brief Represents the data associated with a shader, including buffer and image bindings.
     */
    class ShaderData : public kege::RefCounter
    {
    public:

//        template <typename T> bool setField( const std::string& block_name, const std::string& field_name, const T& value )
//        {
//            const ShaderBindingPoint* block = getBlock( block_name );
//            if ( block == nullptr )
//            {
//                return false;
//            }
//
//            const ShaderStructField* field = getField( block, field_name );
//            if ( !field ) return false;
//
//            size_t size = sizeof(T);
//            assert( size <= field->size && "Field size mismatch!" );
//
//            if ( block->type == ShaderBindingType::SET )
//            {
//                int8_t* dst = getBufferBlockDataPointer( 0, block );
//                if ( dst == nullptr ) return false;
//                memcpy( dst + field->offset, &value, size );
//            }
//            else
//            {
//                memcpy( _constants.data() + field->offset, &value, sizeof(T) );
//            }
//            return true;
//        }
//
//        bool set( const std::string& block_name, int index, size_t size, const void* data );
//
//        void setPushBlock( size_t offset, size_t size, const void* data );
//
//        // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- //
//        // Buffer binding using shader block name to access set-index and binding-index
//        // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- //
//
//        bool setBufferBlock( const std::string& block_name, const BufferBindings& bindings );
//
//        bool setBufferBlock( const std::string& block_name, const BufferInfo& binding );
//
//        // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- //
//        // Image binding using shader block name to access set-index and binding-index
//        // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- //
//
//        bool setImageSampler( const std::string& block_name, const std::vector< ImageInfo >& images );
//
//        bool setImageSampler( const std::string& block_name, const ImageInfo& images );
//
//        // --- --- --- --- --- --- --- --- --- --- --- --- --- --- //
//        // Direct Buffer binding using raw set-index and binding-index
//        // --- --- --- --- --- --- --- --- --- --- --- --- --- --- //
//
//        bool setBufferBlock( int set_index, int binding_index, const BufferBindings& bindings );
//
//        bool setBufferBlock( int set_index, int binding_index, const BufferInfo& binding );
//
//        // --- --- --- --- --- --- --- --- --- --- --- --- --- --- //
//        // Direct Image binding using raw set-index and binding-index
//        // --- --- --- --- --- --- --- --- --- --- --- --- --- --- //
//
//        bool setImageSampler( int set_index, int binding_index, const ImageBindings& binding );
//
//        bool setImageSampler( int set_index, int binding_index, const ImageInfo& binding );
//
//        const uint8_t* cpuBlob() const;
//        size_t blobSize() const;
//
//        bool integrate();
//
//
//        const std::vector< ShaderResrcBindingSet >& getSets()const;
//
//        // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- //
//        // Constructor
//        // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- //
//
//        explicit ShaderData( ref::ShaderLayout layout );
//
//        // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- //
//        // Destructor
//        // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- //
//
//        ~ShaderData();
//
//    private:
//
//        ShaderBindingResrc* getShaderBindingPoint( const std::string& block_name );
//
//        const ShaderStructField* getField( const ShaderBindingPoint* block, const std::string& field_name )const;
//
//        const ShaderBindingPoint* getBlock( const std::string& block_name )const;
//
//        int8_t* getBufferBlockDataPointer( int index, const ShaderBindingPoint* block );
//
//    private:
//
//        std::map< int, int > _set_remap_table;
//        std::vector< ShaderResrcBindingSet > _sets;
//
//        kege::Ref< const ShaderLayout > _layout;
//        std::vector< uint8_t > _constants;
//        kege::Graphics* _graphics;
    };
}

namespace kege::ref{
    typedef kege::Ref< kege::ShaderData > ShaderData;
}
#endif /* shader_data_hpp */
