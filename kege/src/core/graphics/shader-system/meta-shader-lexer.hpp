//
//  meta-shader-lexer.hpp
//  editor
//
//  Created by Kenneth Esdaile on 2/20/26.
//

#ifndef meta_shader_lexer_hpp
#define meta_shader_lexer_hpp

#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <variant>
#include <memory>
#include <optional>

namespace kege::meta::shader{

    /**
     * ---- Token Types ----
     */

    enum class TokenType {
        // Control symbols
        AT,           // @
        LBRACE,       // {
        RBRACE,       // }
        LBRACKET,     // [
        RBRACKET,     // ]
        LPAREN,       // (
        RPAREN,       // )
        EQUALS,       // =
        COMMA,        // ,
        SEMICOLON,    // ;
        COLON,        // :
        VERTICAL_BAR, // |
        NEWLINE,      // \n
        SPACE,      // 

        // Literals
        IDENTIFIER,
        STRING_LITERAL,
        NUMBER,
        BOOL_LITERAL,

        // Keywords
        KW_METADATA,
        KW_VERT,
        KW_FRAG,
        KW_GEOM,
        KW_TESE,
        KW_TESC,
        KW_VERTEX_LAYOUT,
        KW_SETS,
        KW_BINDING,
        KW_SET,
        KW_SPECIALIZATION_CONSTANTS,
        KW_CONST,
        KW_INPUT_ASSEMBLY,
        KW_ATTRIBUTE,
        KW_DEPTH_BIAS,
        KW_DEPTH,
        KW_PUSH_BLOCKS,
        KW_RANGE,
        KW_STENCIL,
        KW_STRUCTS,
        KW_STRUCT,
        KW_IMPORT,
        KW_END,
        KW_ATTACHMENT,
        KW_RASTERIZATION,
        KW_DEPTH_STENCIL,
        KW_FRONT,
        KW_BACK,
        KW_COLOR_BLEND,
        KW_MULTISAMPLE,
        KW_DYNAMIC_STATES,
        KW_DYNAMIC_RENDERING,

        // Special
        UNKNOWN, // |
        END_OF_FILE,
        ERROR
    };

    struct Token {
        TokenType type;
        std::string lexeme;
        size_t line;
        size_t column;

        Token(TokenType t, std::string l, size_t ln, size_t col)
            : type(t), lexeme(std::move(l)), line(ln), column(col) {}
    };

    /**
     * ---- Lexer ----
     */

    class Lexer
    {
    public:

        Lexer(const std::string& source)
            : _source(source), _current(0), _line(1), _column(1), _white_space(" \n\t") {}

        Token nextToken();

        void setWhiteSpace(const char* white_space);
        const char* getWhiteSpace()const;

    private:

        char advance() {
            char c = _source[_current++];
            if (c == '\n') { _line++; _column = 1; }
            else { _column++; }
            return c;
        }

        char peek() const {
            return _current < _source.length() ? _source[_current] : '\0';
        }

        bool isAtEnd() const { return _current >= _source.length(); }

        Token stringLiteral();
        Token identifier();
        Token number();
        void skipWhitespace();
        void skipComment();

    private:

        std::string _source;
        size_t _current;
        size_t _line;
        size_t _column;
        const char* _white_space;
    };
}

#endif /* meta_shader_lexer_hpp */
