//
//  glsl-tokens.hpp
//  glsl-preprocessor
//
//  Created by Kenneth Esdaile on 1/31/25.
//

#ifndef glsl_tokens_hpp
#define glsl_tokens_hpp

#include <stdio.h>

namespace kege{namespace glsl{

    enum Token
    {
        ERROR,
        END_STREAM,
        END_TOKEN,
        START_TOKEN,

        // delimiters
        OPEN_SQUARE_BRACKET,
        CLOSE_SQUARE_BRACKET,
        OPEN_CURLY_BRACKET,
        CLOSE_CURLY_BRACKET,
        OPEN_PARENTHESIS,
        CLOSE_PARENTHESIS,
        SEMICOLON,
        COLON,
        COMMA,
        DOUBLE_QUOTE,
        SINGLE_QUOTE,
        PERIOD,
        HASTAG,

        // operators
        BAR,
        AMPERSAND,
        INVERT,
        INVERT_EQUAL,
        LESS,
        GREATER,
        LESS_EQUAL,
        GREATER_EQUAL,
        AND_EQUAL,
        AND,
        OR,
        OR_EQUAL,
        NOT,
        NOT_EQUAL,
        EQUAL,
        EQUAL_EQUAL,
        ASSIGN,
        INCREMENT,
        DECREMENT,

        ADD,
        SUB,
        MUL,
        DIV,
        MOD,

        ADD_EQUAL,
        SUB_EQUAL,
        MUL_EQUAL,
        DIV_EQUAL,
        MOD_EQUAL,

        // other tokens
        IDENTIFIER,
        BLOCK_COMMENT,
        INLINE_COMMENT,

        // glsl keywords
        VOID,
        CONST,
        BOOL,
        INT,
        UINT,
        FLOAT,
        FLOAT_E,
        DOUBLE,
        DOUBLE_E,
        DOUBLE_EX,
        VEC2,
        VEC3,
        VEC4,
        MAT2,
        MAT3,
        MAT4,
        MAT2X2,
        MAT2X3,
        MAT2X4,
        MAT3X2,
        MAT3X3,
        MAT3X4,
        MAT4X2,
        MAT4X3,
        MAT4X4,
        IVEC2,
        IVEC3,
        IVEC4,
        UVEC2,
        UVEC3,
        UVEC4,
        DVEC2,
        DVEC3,
        DVEC4,
        DMAT2,
        DMAT3,
        DMAT4,
        DMAT2X2,
        DMAT2X3,
        DMAT2X4,
        DMAT3X2,
        DMAT3X3,
        DMAT3X4,
        DMAT4X2,
        DMAT4X3,
        DMAT4X4,

        SAMPLER1D,
        TEXTURE1D,
        IMAGE1D,

        SAMPLER1D_SHADOW,

        SAMPLER1D_ARRAY,
        TEXTURE1D_ARRAY,
        IMAGE1D_ARRAY,

        SAMPLER1D_ARRAY_SHADOW,

        SAMPLER2D,
        TEXTURE2D,
        IMAGE2D,

        SAMPLER2D_SHADOW,

        SAMPLER2D_ARRAY,
        TEXTURE2D_ARRAY,
        IMAGE2D_ARRAY,

        SAMPLER2D_ARRAY_SHADOW,

        SAMPLER2DMS,
        TEXTURE2DMS,
        IMAGE2DMS,

        SAMPLER2DMS_ARRAY,
        TEXTURE2DMS_ARRAY,
        IMAGE2DMS_ARRAY,

        IMAGE3D,
        SAMPLER3D,
        TEXTURE3D,

        SAMPLER_CUBE,
        TEXTURE_CUBE,
        IMAGE_CUBE,

        SAMPLER_CUBE_SHADOW,

        SAMPLER_CUBE_ARAY,
        TEXTURE_CUBE_ARRAY,
        IMAGE_CUBE_ARRAY,

        SAMPLER_CUBE_ARRAY_SHADOW,

        SAMPLER_BUFFER,
        TEXTURE_BUFFER,
        IMAGE_BUFFER,

        SAMPLER,
        SAMPLER_SHADOW,

        IN,
        OUT,
        UNIFORM,
        BUFFER,
        LAYOUT,
        STRUCT,
        SET,
        BINDING,
        LOCATION,
        PUSH_CONSTANT,

        STORAGE_IMAGE,
        SAMPLED_IMAGE,
        UNIFORM_BUFFER,
        STORAGE_BUFFER,
        COMBINED_IMAGE_SAMPLER,
        UNIFORM_TEXEL_BUFFER,
        STORAGE_TEXEL_BUFFER,
        UNIFORM_BUFFER_DYNAMIC,
        STORAGE_BUFFER_DYNAMIC,
        INPUT_ATTACHMENT,

        VERT_IN,
        INST_IN,

        TOKEN_COUNT
    };

    const char* toString( Token token );
}}
#endif /* glsl_tokens_hpp */
