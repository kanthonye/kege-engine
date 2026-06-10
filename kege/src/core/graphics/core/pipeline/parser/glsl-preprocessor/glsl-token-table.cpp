//
//  glsl-token-table.cpp
//  glsl-preprocessor
//
//  Created by Kenneth Esdaile on 1/30/25.
//

#include "glsl-lexer.hpp"
#include "glsl-token-table.hpp"

namespace kege{namespace glsl{

HashTable< uint32_t >* getGLSLTokenTable()
{
    static HashTable< uint32_t > token_table;
    if ( !token_table.empty() )
    {
        return &token_table;
    }

    // Custom Directives: custom preprocessor directives to explicitly specify the descriptor type
    token_table[ "sampler" ] = Token::SAMPLER;
    token_table[ "sampled_image" ] = Token::SAMPLED_IMAGE;
    token_table[ "storage_image" ] = Token::STORAGE_IMAGE;
    token_table[ "uniform_buffer" ] = Token::UNIFORM_BUFFER;
    token_table[ "uniform_buffer_dynamic" ] = Token::UNIFORM_BUFFER_DYNAMIC;
    token_table[ "storage_buffer" ] = Token::STORAGE_BUFFER;
    token_table[ "storage_buffer_dynamic" ] = Token::STORAGE_BUFFER_DYNAMIC;
    token_table[ "storage_texel_buffer" ] = Token::STORAGE_TEXEL_BUFFER;
    token_table[ "unifrom_texel_buffer" ] = Token::UNIFORM_TEXEL_BUFFER;
    token_table[ "combined_image_sampler" ] = Token::COMBINED_IMAGE_SAMPLER;
    token_table[ "input_attachment" ] = Token::INPUT_ATTACHMENT;

    token_table[ "vert_in" ] = Token::VERT_IN;
    token_table[ "inst_in" ] = Token::INST_IN;

    token_table[ "in"       ] = Token::IN;
    token_table[ "out"      ] = Token::OUT;

    token_table[ "buffer"   ] = Token::BUFFER;
    token_table[ "uniform"  ] = Token::UNIFORM;
    token_table[ "layout"   ] = Token::LAYOUT;
    token_table[ "struct"   ] = Token::STRUCT;
    token_table[ "location" ] = Token::LOCATION;
    token_table[ "set"      ] = Token::SET;
    token_table[ "binding"  ] = Token::BINDING;
    token_table[ "push_constant" ] = Token::PUSH_CONSTANT;

    // glsl keywords
    token_table[ "void"     ] = Token::VOID;
    token_table[ "float"    ] = Token::FLOAT;
    token_table[ "int"      ] = Token::INT;
    token_table[ "double"   ] = Token::DOUBLE;
    token_table[ "vec2"     ] = Token::VEC2;
    token_table[ "vec3"     ] = Token::VEC3;
    token_table[ "vec4"     ] = Token::VEC4;
    token_table[ "mat2"     ] = Token::MAT2;
    token_table[ "mat3"     ] = Token::MAT3;
    token_table[ "mat4"     ] = Token::MAT4;
    token_table[ "mat2x2"   ] = Token::MAT2X2;
    token_table[ "mat2x3"   ] = Token::MAT2X3;
    token_table[ "mat2x4"   ] = Token::MAT2X4;
    token_table[ "mat3x2"   ] = Token::MAT3X3;
    token_table[ "mat3x3"   ] = Token::MAT3X3;
    token_table[ "mat3x4"   ] = Token::MAT3X4;
    token_table[ "mat4x2"   ] = Token::MAT4X2;
    token_table[ "mat4x3"   ] = Token::MAT4X3;
    token_table[ "mat4x4"   ] = Token::MAT4X4;
    token_table[ "ivec2"    ] = Token::IVEC2;
    token_table[ "ivec3"    ] = Token::IVEC3;
    token_table[ "ivec4"    ] = Token::IVEC4;
    token_table[ "uvec2"    ] = Token::UVEC2;
    token_table[ "uvec3"    ] = Token::UVEC3;
    token_table[ "uvec4"    ] = Token::UVEC4;
    token_table[ "dvec2"    ] = Token::DVEC2;
    token_table[ "dvec3"    ] = Token::DVEC3;
    token_table[ "dvec4"    ] = Token::DVEC4;
    token_table[ "dmat2"    ] = Token::DMAT2;
    token_table[ "dmat3"    ] = Token::DMAT3;
    token_table[ "dmat4"    ] = Token::DMAT4;
    token_table[ "dmat2x2"  ] = Token::DMAT2X2;
    token_table[ "dmat2x3"  ] = Token::DMAT2X3;
    token_table[ "dmat2x4"  ] = Token::DMAT2X4;
    token_table[ "dmat3x2"  ] = Token::DMAT3X3;
    token_table[ "dmat3x3"  ] = Token::DMAT3X3;
    token_table[ "dmat3x4"  ] = Token::DMAT3X4;
    token_table[ "dmat4x2"  ] = Token::DMAT4X2;
    token_table[ "dmat4x3"  ] = Token::DMAT4X3;
    token_table[ "dmat4x4"  ] = Token::DMAT4X4;
    token_table[ "const"    ] = Token::CONST;

    // a handle for accessing a 1D texture

    token_table[ "sampler1D" ] = Token::SAMPLER1D;
    token_table[ "texture1D" ] = Token::TEXTURE1D;
    token_table[ "image1D"   ] = Token::IMAGE1D;

    // a handle for accessing a 1D depth texture with comparison

    token_table[ "sampler1DShadow" ] = Token::SAMPLER1D_SHADOW;

    // a handle for accessing a 1D array texture

    token_table[ "sampler1DArray" ] = Token::SAMPLER1D_ARRAY;
    token_table[ "texture1DArray" ] = Token::TEXTURE1D_ARRAY;
    token_table[ "image1DArray"   ] = Token::IMAGE1D_ARRAY;

    // a handle for accessing a 1D array depth texture with comparison

    token_table[ "sampler1DArrayShadow" ] = Token::SAMPLER1D_ARRAY_SHADOW;

    // a handle for accessing a 2D texture

    token_table[ "sampler2D" ] = Token::SAMPLER2D;
    token_table[ "texture2D" ] = Token::TEXTURE2D;
    token_table[ "image2D"   ] = Token::IMAGE2D;

    // a handle for accessing a 2D depth texture with comparison

    token_table[ "sampler2DShadow" ] = Token::SAMPLER2D_SHADOW;

    // a handle for accessing a 2D array texture

    token_table[ "sampler2DArray" ] = Token::SAMPLER2D_ARRAY;
    token_table[ "texture2DArray" ] = Token::TEXTURE2D_ARRAY;
    token_table[ "image2DArray"   ] = Token::IMAGE2D_ARRAY;

    // a handle for accessing a 2D array depth texture with comparison

    token_table[ "sampler2DArrayShadow" ] = Token::SAMPLER2D_ARRAY_SHADOW;

    // a handle for accessing a 2D multisample texture

    token_table[ "sampler2DMS" ] = Token::SAMPLER2DMS;
    token_table[ "texture2DMS" ] = Token::TEXTURE2DMS;
    token_table[ "image2DMS"   ] = Token::IMAGE2DMS;

    // a handle for accessing a 2D multisample array texture

    token_table[ "sampler2DMSArray" ] = Token::SAMPLER2DMS_ARRAY;
    token_table[ "texture2DMSArray" ] = Token::TEXTURE2DMS_ARRAY;
    token_table[ "image2DMSArray"   ] = Token::IMAGE2DMS_ARRAY;

    // a handle for accessing a 3D texture

    token_table[ "sampler3D" ] = Token::SAMPLER3D;
    token_table[ "texture3D" ] = Token::TEXTURE3D;
    token_table[ "image3D"   ] = Token::IMAGE3D;

    // a handle for accessing a cube mapped texture

    token_table[ "samplerCube" ] = Token::SAMPLER_CUBE;
    token_table[ "textureCube" ] = Token::TEXTURE_CUBE;
    token_table[ "imageCube"   ] = Token::IMAGE_CUBE;

    // a handle for accessing a cube map depth texture with comparison

    token_table[ "samplerCubeShadow" ] = Token::SAMPLER_CUBE_SHADOW;

    // a handle for accessing a cube map array texture

    token_table[ "samplerCubeArray" ] = Token::SAMPLER_CUBE_ARAY;
    token_table[ "textureCubeArray" ] = Token::TEXTURE_CUBE_ARRAY;
    token_table[ "imageCubeArray"   ] = Token::IMAGE_CUBE_ARRAY;

    // a handle for accessing a cube map array depth texture with comparison

    token_table[ "samplerCubeArrayShadow"   ] = Token::IMAGE3D;

    // a handle for accessing a buffer texture

    token_table[ "samplerBuffer"   ] = Token::SAMPLER_BUFFER;
    token_table[ "textureBuffer"   ] = Token::TEXTURE_BUFFER;
    token_table[ "imageBuffer"     ] = Token::IMAGE_BUFFER;
    return &token_table;
}

}}
