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
#include "shader-data-types.hpp"

namespace kege{

    /**
     * @brief Represents a field within a shader buffer block.
     */
    struct ShaderStructField
    {
        std::string name;

        ShaderDataType type;

        uint32_t offset;
        uint32_t size;
        uint32_t count;
    };

    /**
     * @brief Represents a block of shader buffer data.
     */
    struct ShaderStructBlock : public kege::RefCounter
    {
        const ShaderStructField* get( const std::string& name )const;
        ShaderStructField* get( const std::string& name );

        const ShaderStructField* operator[]( int field_index )const;
        ShaderStructField* operator[]( int field_index );

        ShaderStructBlock( const std::vector< ShaderStructField >& fields );

        std::unordered_map< std::string, size_t > quick_lookup_map;
        std::vector< ShaderStructField > fields;
        std::string instance_name;
        uint32_t count;
    };

}



namespace kege {

    /**
     * @brief Types of shader binding points.
     */
    enum ShaderBindingType{ PUSH_CONSTANT, SET, STRUCT };

    /**
     * @brief Represents a shader binding block.
     */
    struct ShaderBindingBlock : public kege::RefCounter
    {
        ShaderBindingBlock( ShaderBindingType t ): type(t) {}
        virtual ~ShaderBindingBlock(){}

        Ref< ShaderStructBlock > struct_block;
        ShaderBindingType type;
        std::string name;

        ShaderStageFlag stages;
    };

}
#endif /* shader_buffer_block_hpp */
