//
//  glsl-parser.hpp
//  glsl-preprocessor
//
//  Created by Kenneth Esdaile on 1/31/25.
//

#ifndef glsl_parser_hpp
#define glsl_parser_hpp

#include "glsl-lexer.hpp"

namespace kege{namespace glsl{

    struct Instanced
    {
        std::string name;
        int count;
    };

    struct Qualifier
    {
        std::string name;  // The layout keyword (e.g., location, binding, set)
        int value;         // Parsed integer value (if applicable)
    };

    struct Variable
    {
        std::string type;
        std::string name;
        int count;
    };

    struct Layout
    {
        std::vector< int > stages;
        std::vector< Qualifier > qualifiers; // Parsed layout qualifiers

        /**
         storage_qualifier​ can be one of the following Storage Qualifiers:
         - uniform
         - in or out
         - buffer
         */
        std::string storage_qualifier;

        std::string type; // GLSL type (e.g., vec3, mat4, sampler2D)
        std::string name; // Variable name
        int count; // ... uniform sampler2D Images[ 4 ];

        std::vector< Variable > members; // Variables inside the buffer block

        Instanced instanced;

    };

    struct Struct
    {
        int stage;
        std::string name;
        std::vector< Variable > members;
    };

    struct Meta
    {
        typedef int ShaderStage;
        typedef int LayoutIndex;

        std::map<ShaderStage, HashTable< Struct >> struct_blocks;
        std::map<ShaderStage, std::vector< LayoutIndex >> inputs;
        std::map<ShaderStage, std::vector< LayoutIndex >> outputs;

        HashTable< LayoutIndex > push_constants;
        HashTable< LayoutIndex > uniforms;

        std::vector< Layout > layouts;  // uniforms
    };

    std::string parser( int shader_stage, const std::vector< char >* buffer, Meta* meta );
    std::ostream& operator<<(std::ostream& s, const kege::glsl::Meta& meta );

}}

#endif /* glsl_parser_hpp */
