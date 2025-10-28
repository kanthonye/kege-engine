//
//  glsl-tokens.cpp
//  glsl-preprocessor
//
//  Created by Kenneth Esdaile on 1/31/25.
//

#include "glsl-tokens.hpp"

namespace kege{namespace glsl{

    const char* toString( Token token )
    {
        switch ( token )
        {
            case Token::ERROR: return "ERROR";
            case Token::END_STREAM: return "END_STREAM";
            case Token::END_TOKEN: return "END_TOKEN";
            case Token::START_TOKEN: return "START_TOKEN";

            case Token::OPEN_SQUARE_BRACKET: return "OPEN_SQUARE_BRACKET";
            case Token::CLOSE_SQUARE_BRACKET: return "CLOSE_SQUARE_BRACKET";
            case Token::OPEN_CURLY_BRACKET: return "OPEN_CURLY_BRACKET";
            case Token::CLOSE_CURLY_BRACKET: return "CLOSE_CURLY_BRACKET";
            case Token::OPEN_PARENTHESIS: return "OPEN_PARENTHESIS";
            case Token::CLOSE_PARENTHESIS: return "CLOSE_PARENTHESIS";
            case Token::SEMICOLON: return "SEMICOLON";
            case Token::COLON: return "COLON";
            case Token::COMMA: return "COMMA";
            case Token::DOUBLE_QUOTE: return "DOUBLE_QUOTE";
            case Token::SINGLE_QUOTE: return "SINGLE_QUOTE";
            case Token::PERIOD: return "PERIOD";
            case Token::HASTAG: return "HASTAG";

            case Token::BAR: return "BAR";
            case Token::AMPERSAND: return "AMPERSAND";
            case Token::INVERT: return "INVERT";
            case Token::INVERT_EQUAL: return "INVERT_EQUAL";
            case Token::LESS: return "LESS";
            case Token::GREATER: return "GREATER";
            case Token::LESS_EQUAL: return "LESS_EQUAL";
            case Token::GREATER_EQUAL: return "GREATER_EQUAL";
            case Token::AND_EQUAL: return "AND_EQUAL";
            case Token::AND: return "AND";
            case Token::OR: return "OR";
            case Token::OR_EQUAL: return "OR_EQUAL";
            case Token::NOT: return "NOT";
            case Token::NOT_EQUAL: return "NOT_EQUAL";
            case Token::EQUAL: return "EQUAL";
            case Token::EQUAL_EQUAL: return "EQUAL_EQUAL";
            case Token::ASSIGN: return "ASSIGN";
            case Token::INCREMENT: return "INCREMENT";
            case Token::DECREMENT: return "DECREMENT";

            case Token::ADD: return "ADD";
            case Token::SUB: return "SUB";
            case Token::MUL: return "MUL";
            case Token::DIV: return "DIV";
            case Token::MOD: return "MOD";

            case Token::ADD_EQUAL: return "ADD_EQUAL";
            case Token::SUB_EQUAL: return "SUB_EQUAL";
            case Token::MUL_EQUAL: return "MUL_EQUAL";
            case Token::DIV_EQUAL: return "DIV_EQUAL";
            case Token::MOD_EQUAL: return "MOD_EQUAL";

            case Token::IDENTIFIER: return "IDENTIFIER";
            case Token::BLOCK_COMMENT: return "BLOCK_COMMENT";
            case Token::INLINE_COMMENT: return "INLINE_COMMENT";

            case Token::VOID: return "VOID";
            case Token::CONST: return "CONST";
            case Token::BOOL: return "BOOL";
            case Token::INT: return "INT";
            case Token::UINT: return "UINT";
            case Token::FLOAT: return "FLOAT";
            case Token::FLOAT_E: return "FLOAT_E";
            case Token::DOUBLE: return "DOUBLE";
            case Token::DOUBLE_E: return "DOUBLE_E";
            case Token::DOUBLE_EX: return "DOUBLE_EX";
            case Token::VEC2: return "VEC2";
            case Token::VEC3: return "VEC3";
            case Token::VEC4: return "VEC4";
            case Token::MAT2: return "MAT2";
            case Token::MAT3: return "MAT3";
            case Token::MAT4: return "MAT4";
            case Token::MAT2X2: return "MAT2X2";
            case Token::MAT2X3: return "MAT2X3";
            case Token::MAT2X4: return "MAT2X4";
            case Token::MAT3X2: return "MAT3X2";
            case Token::MAT3X3: return "MAT3X3";
            case Token::MAT3X4: return "MAT3X4";
            case Token::MAT4X2: return "MAT4X2";
            case Token::MAT4X3: return "MAT4X3";
            case Token::MAT4X4: return "MAT4X4";
            case Token::IVEC2: return "IVEC2";
            case Token::IVEC3: return "IVEC3";
            case Token::IVEC4: return "IVEC4";
            case Token::UVEC2: return "UVEC2";
            case Token::UVEC3: return "UVEC3";
            case Token::UVEC4: return "UVEC4";
            case Token::DVEC2: return "DVEC2";
            case Token::DVEC3: return "DVEC3";
            case Token::DVEC4: return "DVEC4";
            case Token::DMAT2: return "DMAT2";
            case Token::DMAT3: return "DMAT3";
            case Token::DMAT4: return "DMAT4";
            case Token::DMAT2X2: return "DMAT2X2";
            case Token::DMAT2X3: return "DMAT2X3";
            case Token::DMAT2X4: return "DMAT2X4";
            case Token::DMAT3X2: return "DMAT3X2";
            case Token::DMAT3X3: return "DMAT3X3";
            case Token::DMAT3X4: return "DMAT3X4";
            case Token::DMAT4X2: return "DMAT4X2";
            case Token::DMAT4X3: return "DMAT4X3";
            case Token::DMAT4X4: return "DMAT4X4";

            case Token::SAMPLER1D: return "SAMPLER1D";
            case Token::TEXTURE1D: return "TEXTURE1D";
            case Token::IMAGE1D: return "IMAGE1D";

            case Token::SAMPLER1D_SHADOW: return "SAMPLER1D_SHADOW";

            case Token::SAMPLER1D_ARRAY: return "SAMPLER1D_ARRAY";
            case Token::TEXTURE1D_ARRAY: return "TEXTURE1D_ARRAY";
            case Token::IMAGE1D_ARRAY: return "IMAGE1D_ARRAY";

            case Token::SAMPLER1D_ARRAY_SHADOW: return "SAMPLER1D_ARRAY_SHADOW";

            case Token::SAMPLER2D: return "SAMPLER2D";
            case Token::TEXTURE2D: return "TEXTURE2D";
            case Token::IMAGE2D: return "IMAGE2D";

            case Token::SAMPLER2D_SHADOW: return "SAMPLER2D_SHADOW";

            case Token::SAMPLER2D_ARRAY: return "SAMPLER2D_ARRAY";
            case Token::TEXTURE2D_ARRAY: return "TEXTURE2D_ARRAY";
            case Token::IMAGE2D_ARRAY: return "IMAGE2D_ARRAY";

            case Token::SAMPLER2D_ARRAY_SHADOW: return "SAMPLER2D_ARRAY_SHADOW";

            case Token::SAMPLER2DMS: return "SAMPLER2DMS";
            case Token::TEXTURE2DMS: return "TEXTURE2DMS";
            case Token::IMAGE2DMS: return "IMAGE2DMS";

            case Token::SAMPLER2DMS_ARRAY: return "SAMPLER2DMS_ARRAY";
            case Token::TEXTURE2DMS_ARRAY: return "TEXTURE2DMS_ARRAY";
            case Token::IMAGE2DMS_ARRAY: return "IMAGE2DMS_ARRAY";

            case Token::IMAGE3D: return "IMAGE3D";
            case Token::SAMPLER3D: return "SAMPLER3D";
            case Token::TEXTURE3D: return "TEXTURE3D";

            case Token::SAMPLER_CUBE: return "SAMPLER_CUBE";
            case Token::TEXTURE_CUBE: return "TEXTURE_CUBE";
            case Token::IMAGE_CUBE: return "IMAGE_CUBE";

            case Token::SAMPLER_CUBE_SHADOW: return "SAMPLER_CUBE_SHADOW";

            case Token::SAMPLER_CUBE_ARAY: return "SAMPLER_CUBE_ARAY";
            case Token::TEXTURE_CUBE_ARRAY: return "TEXTURE_CUBE_ARRAY";
            case Token::IMAGE_CUBE_ARRAY: return "IMAGE_CUBE_ARRAY";

            case Token::SAMPLER_CUBE_ARRAY_SHADOW: return "SAMPLER_CUBE_ARRAY_SHADOW";

            case Token::SAMPLER_BUFFER: return "SAMPLER_BUFFER";
            case Token::TEXTURE_BUFFER: return "TEXTURE_BUFFER";
            case Token::IMAGE_BUFFER: return "IMAGE_BUFFER";

            case Token::SAMPLER: return "SAMPLER";
            case Token::SAMPLER_SHADOW: return "SAMPLER_SHADOW";

            case Token::IN: return "IN";
            case Token::OUT: return "OUT";
            case Token::UNIFORM: return "UNIFORM";
            case Token::BUFFER: return "BUFFER";
            case Token::LAYOUT: return "LAYOUT";
            case Token::STRUCT: return "STRUCT";
            case Token::SET: return "SET";
            case Token::BINDING: return "BINDING";
            case Token::LOCATION: return "LOCATION";
            case Token::PUSH_CONSTANT: return "PUSH_CONSTANT";
            default:
                break;
        }
        return "UNKNOWN";
    };

}}
