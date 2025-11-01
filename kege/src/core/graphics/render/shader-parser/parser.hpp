//
//  parser.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/21/25.
//

#ifndef kege_shader_code_parser_hpp
#define kege_shader_code_parser_hpp

#include <string>
#include <vector>
#include <unordered_map>
#include "tokenizer.hpp"
#include "shader-data-types.hpp"
#include "shader-binding-point-set.hpp"
#include "shader-constant-binding-point.hpp"

#include "shader-reflect.hpp"

namespace kege::glsl{

    class Parser{
    public:

        Parser( const std::vector< glsl::Token >& tks );

        const ShaderReflect* getReflection()const;
        const std::string& getGLSLOutput();
        void parse();

    private:

        bool delimiter(const TokenType& tok);
        bool shouldSpaceAfterToken(const TokenType& tok);
        void emitToken(const Token& tok);
        // Utility functions
        bool isAtEnd() const;
        const Token& prev() const;
        const Token& next() const;
        const Token& peek() const;
        const Token& advance();
        bool check(TokenType t) const;
        bool match(TokenType t);
        void expect(TokenType t, const std::string& msg);
        void parseVertexLayout();
        void parseSetLayout();
        void parsePushConstantBlock();
        void emitGLSLBlock( Ref< ShaderStructBlock >& struct_block );
        void parseOutLayout();
        void parseInLayout();
        Ref< ShaderStructBlock > parseShaderBlock();
        int consumeIfArraySize();
        std::string consumeAssignNumber( const char* what );
        VertexInputRate consumeVertexInputRate( const char* what );
        std::string consumeIdentifier(const std::string& what);
        kege::ShaderDataType consumeType(const std::string& what);

    private:

        ShaderReflect reflection;
        std::string generated_glsl;

        const std::vector<Token>& tokens;
        std::string tab_spacing;
        bool should_tab = false;
        int tab_level_count = 0;
        size_t pos;
    };
}
#endif /* kege_shader_code_parser_hpp */
