//
//  glsl-token-table.hpp
//  glsl-preprocessor
//
//  Created by Kenneth Esdaile on 1/30/25.
//

#ifndef glsl_token_table_hpp
#define glsl_token_table_hpp

#include "hash-table.hpp"
#include "glsl-tokens.hpp"

namespace kege{namespace glsl{

    HashTable< uint32_t >* getGLSLTokenTable();

}}
#endif /* glsl_token_table_hpp */
