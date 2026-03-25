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

    using SetIndices = std::initializer_list< int >;
    using SetNames = std::initializer_list< std::string >;
    struct Binds{ int set, binding, type; };

    /**
     * @brief Encapsulates shader resource bindings for a shader pipeline.
     *
     * Manages buffer and image bindings for shader resource sets.
     * Provides methods to set resources by set/binding index or by name.
     */
    class ShaderData : public kege::RefCounter
    {
    public:

        template <typename T> bool setPushBlock( const std::string& block_name, const std::string& field_name, const T& value )
        {
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
            return true;
        }
        
        void setPushBlock( size_t offset, size_t size, const void* data );

        /**
         * @brief Sets buffer bindings for a specific set and binding index.
         *
         * @param set_index Index of the shader resource set.
         * @param binding_index Index of the binding within the set.
         * @param bindings Vector of buffer bindings to set.
         * @return True if the bindings were set successfully, false otherwise.
         */
        bool setBuffers( int set_index, int binding_index, const BufferBindings& bindings, int frame = 0 );

        /**
         * @brief Sets buffer bindings for a specific push block by name.
         *
         * @param block_name Name of the push block.
         * @param bindings Vector of buffer bindings to set.
         * @return True if the bindings were set successfully, false otherwise.
         */
        bool setBuffers( const std::string& block_name, const BufferBindings& bindings, int frame = 0 );

        /**
         * @brief Sets image bindings for a specific set and binding index.
         *
         * @param set_index Index of the shader resource set.
         * @param binding_index Index of the binding within the set.
         * @param bindings Vector of image bindings to set.
         * @return True if the bindings were set successfully, false otherwise.
         */
        bool setImages( int set_index, int binding_index, const ImageBindings& bindings, int frame = 0 );

        /**
         * @brief Sets image bindings for a specific push block by name.
         *
         * @param block_name Name of the push block.
         * @param bindings Vector of image bindings to set.
         * @return True if the bindings were set successfully, false otherwise.
         */
        bool setImages( const std::string& block_name, const ImageBindings& bindings, int frame = 0 );

        const std::map< int, kege::BindSet >& getShaderSets()const;
        
        /**
         * @brief Retrieves the associated shader layout.
         * @return Reference to the shader layout.
         */
        const ref::ShaderPipeline& getShaderPipeline()const;

        void update();

        /**
         * @brief Constructor for ShaderData.
         * @param p The shaderPipeline object.
         * @param s A list of SetLayout names to allocate ShaderSets from.
         */
        explicit ShaderData( const ref::ShaderPipeline& p, const kege::SetNames& s );

        /**
         * @brief Constructor for ShaderData.
         * @param p The shaderPipeline object.
         * @param s A list of SetLayout indices to allocate ShaderSets from. 
         */
        explicit ShaderData( const ref::ShaderPipeline& p, const kege::SetIndices& s );

        /**
         * @brief Destructor for ShaderData.
         */
        virtual ~ShaderData();

    private:

        std::map< int, kege::BindSet > _sets;
        ref::ShaderPipeline _pipeline;

        std::vector< uint8_t > _push_block_data;
    };
}

namespace kege::ref{
    typedef kege::Ref< kege::ShaderData > ShaderData;
}
namespace kege::cref{
    typedef kege::Ref< const kege::ShaderData > ShaderData;
}
#endif /* shader_data_hpp */
