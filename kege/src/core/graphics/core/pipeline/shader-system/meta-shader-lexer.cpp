//
//  meta-shader-lexer.cpp
//  editor
//
//  Created by Kenneth Esdaile on 2/20/26.
//

#include "meta-shader-lexer.hpp"

namespace kege::meta::shader{

    // ========== Lexer Implementation ==========
    Token Lexer::nextToken()
    {
        skipWhitespace();
        skipComment();

        if (isAtEnd()) {
            return Token(TokenType::END_OF_FILE, "", _line, _column);
        }

        char c = peek();

        // Single character tokens
        switch (c) {
            case '@': advance(); return Token(TokenType::AT, "@", _line, _column);
            case '[': advance(); return Token(TokenType::LBRACKET, "[", _line, _column);
            case ']': advance(); return Token(TokenType::RBRACKET, "]", _line, _column);
            case '{': advance(); return Token(TokenType::LBRACE, "{", _line, _column);
            case '}': advance(); return Token(TokenType::RBRACE, "}", _line, _column);
            case '(': advance(); return Token(TokenType::LPAREN, "(", _line, _column);
            case ')': advance(); return Token(TokenType::RPAREN, ")", _line, _column);
            case '=': advance(); return Token(TokenType::EQUALS, "=", _line, _column);
            case ',': advance(); return Token(TokenType::COMMA, ",", _line, _column);
            case ';': advance(); return Token(TokenType::SEMICOLON, ";", _line, _column);
            case ':': advance(); return Token(TokenType::COLON, ":", _line, _column);
            case '|': advance(); return Token(TokenType::VERTICAL_BAR, "|", _line, _column);
            case '\n': advance(); return Token(TokenType::NEWLINE, "\n", _line, _column);
            case ' ': advance(); return Token(TokenType::SPACE, " ", _line, _column);
        }

        // Identifiers and keywords
        if (std::isalpha(c) || c == '_') {
            return identifier();
        }

        // Numbers
        if (std::isdigit(c)) {
            return number();
        }

        // String literals
        if (c == '"') {
            return stringLiteral();
        }

        advance();

        return Token(TokenType::UNKNOWN, std::string(1, c), _line, _column);
    }

    void Lexer::setWhiteSpace(const char* white_space)
    {
        _white_space = white_space;
    }
    const char* Lexer::getWhiteSpace()const
    {
        return _white_space;
    }

    Token Lexer::stringLiteral()
    {
        advance();
        size_t start = _current;
        while (peek() != '"') {
            advance();
        }

        std::string text = _source.substr(start, _current - start);
        advance();
        return Token(TokenType::STRING_LITERAL, text, _line, _column);
    }

    Token Lexer::identifier()
    {
        size_t start = _current;
        while (std::isalnum(peek()) || peek() == '_') {
            advance();
        }

        std::string text = _source.substr(start, _current - start);

        // Check for keywords
        static std::unordered_map<std::string, TokenType> keywords = {
            {"metadata", TokenType::KW_METADATA},
            {"vert", TokenType::KW_VERT},
            {"frag", TokenType::KW_FRAG},
            {"geom", TokenType::KW_GEOM},
            {"tesc", TokenType::KW_TESC},
            {"tese", TokenType::KW_TESE},
            {"vertex_layout", TokenType::KW_VERTEX_LAYOUT},
            {"binding", TokenType::KW_BINDING},
            {"sets", TokenType::KW_SETS},
            {"set", TokenType::KW_SET},
            {"push_blocks", TokenType::KW_PUSH_BLOCKS},
            {"range", TokenType::KW_RANGE},
            {"attrib", TokenType::KW_ATTRIBUTE},
            {"input_assembly", TokenType::KW_INPUT_ASSEMBLY},
            {"rasterization", TokenType::KW_RASTERIZATION},
            {"depth_bias", TokenType::KW_DEPTH_BIAS},
            {"depth", TokenType::KW_DEPTH},
            {"multisample", TokenType::KW_MULTISAMPLE},
            {"depth_stencil", TokenType::KW_DEPTH_STENCIL},
            {"stencil", TokenType::KW_STENCIL},
            {"import", TokenType::KW_IMPORT},
            {"specialization_constants", TokenType::KW_SPECIALIZATION_CONSTANTS},
            {"color_blend", TokenType::KW_COLOR_BLEND},
            {"dynamic_states", TokenType::KW_DYNAMIC_STATES},
            {"attachment", TokenType::KW_ATTACHMENT},
            {"structs", TokenType::KW_STRUCTS},
            {"struct", TokenType::KW_STRUCT},
            {"const", TokenType::KW_CONST},
            {"end", TokenType::KW_END},
            {"front", TokenType::KW_FRONT},
            {"back", TokenType::KW_BACK},
            {"dynamic_rendering", TokenType::KW_DYNAMIC_RENDERING},
        };

        auto it = keywords.find(text);
        if (it != keywords.end()) {
            return Token(it->second, text, _line, _column);
        }

        return Token(TokenType::IDENTIFIER, text, _line, _column);
    }

    Token Lexer::number()
    {
        bool period = false;
        size_t start = _current;
        if (peek() == '-' || peek() == '+')
        {
            advance();
        }
        if (peek() == '.')
        {
            advance();
            period = true;
        }

        while (std::isalnum(peek()))
        {
            advance();
        }

        if (peek() == '.')
        {
            if (!period)
            {
                advance();
            }
            else
            {
                return Token(TokenType::ERROR, nullptr, _line, _column);
            }
        }

        while (std::isalnum(peek()))
        {
            advance();
        }

        if (peek() == 'e' || peek() == 'E')
        {
            advance();
        }
        if (peek() == '-' || peek() == '+')
        {
            advance();
        }

        while (std::isalnum(peek()))
        {
            advance();
        }

        std::string text = _source.substr(start, _current - start);
        return Token(TokenType::NUMBER, text, _line, _column);
    }

    void Lexer::skipWhitespace()
    {
        while (!isAtEnd() && strchr(_white_space, peek()) != 0) {
            advance();
        }
    }

    void Lexer::skipComment()
    {
        while (peek() == '/' && _current + 1 < _source.length())
        {
            if (_source[_current + 1] == '/')
            {
                // Line comment
                while (!isAtEnd() && peek() != '\n') {
                    advance();
                }
                //if (!isAtEnd()) advance(); // Skip newline
                skipWhitespace();
            }
            else if (_source[_current + 1] == '*')
            {
                // Block comment
                advance(); // Skip /
                advance(); // Skip *
                while (!isAtEnd() && !(peek() == '*' &&
                       _current + 1 < _source.length() &&
                       _source[_current + 1] == '/')) {
                    advance();
                }
                if (!isAtEnd()) advance(); // Skip *
                if (!isAtEnd()) advance(); // Skip /
                skipWhitespace();
            }
            else break;
        }
    }

}
