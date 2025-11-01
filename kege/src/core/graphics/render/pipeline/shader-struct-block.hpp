//
//  shader-buffer-block.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/18/25.
//

#ifndef shader_buffer_block_hpp
#define shader_buffer_block_hpp

#include <memory>
#include <vector>
#include <string>
#include <cstdint>
#include <unordered_map>
#include "../core/graphics-core.hpp"

namespace kege{

    /**
     * @brief Represents a field within a shader buffer block.
     */
    enum struct ShaderStructType
    {
        DataBlock, Buffer, Image, Sampler
    };

    ShaderStructType stringToShaderStructType( const std::string& s );

    /**
     * @brief Represents a field within a shader buffer block.
     */
    struct ShaderStructField
    {
        std::string name;

        ShaderVarType type;

        uint32_t offset;
        uint32_t size;
        uint32_t count;
    };

    /**
     * @brief Represents a block of shader buffer data.
     */
    class ShaderStructBlock : public kege::RefCounter
    {
    public:

        //ShaderStructBlock& operator =( const kege::ShaderStructBlock& other );
        //ShaderStructBlock& operator =( kege::ShaderStructBlock&& other );
        
        const kege::ShaderStructField* get( const std::string& name )const;
        kege::ShaderStructField* get( const std::string& name );

        const kege::ShaderStructField* operator[]( int field_index )const;
        kege::ShaderStructField* operator[]( int field_index );


        kege::ShaderStructType getType();

        ShaderStructBlock
        (
            ShaderStructType t,
            const std::string& n,
            const std::vector< kege::ShaderStructField >& f
        );

        //ShaderStructBlock( const kege::ShaderStructBlock& other );
        //ShaderStructBlock( kege::ShaderStructBlock&& other );

    private:

        std::unordered_map< std::string, size_t > quick_lookup_map;
        std::vector< ShaderStructField > fields;
        std::string name;

        kege::ShaderStructType type;
    };

}
namespace kege::ref{
    typedef kege::Ref< ShaderStructBlock > ShaderStructBlock;
}
#endif /* shader_buffer_block_hpp */
