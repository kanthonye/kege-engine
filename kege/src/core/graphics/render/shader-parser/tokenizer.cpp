//
//  tokenizer.cpp
//  logr
//
//  Created by Kenneth Esdaile on 10/19/25.
//

#include "tokenizer.hpp"

namespace kege::glsl{

    Tokenizer::Tokenizer(size_t size, const char* src)
    :   source(src)
    ,   size(size)
    ,   pos(0)
    ,   line(1)
    ,   column(1)
    {}

    const Token& Tokenizer::getCurrToken()const
    {
        return curr_token;
    }

    const Token& Tokenizer::getNextToken()
    {
        if ( !token_queue.empty() )
        {
            curr_token.type = token_queue.front().type;
            curr_token.column = token_queue.front().column;
            curr_token.line = token_queue.front().line;
            curr_token.lexeme = token_queue.front().lexeme;
            token_queue.pop();
            return curr_token;
        }

        skipWhitespaceAndComments();
        if ( isAtEnd() ) return setToken( TokenType::EndOfFile, "" );

        char c = peek();

        if (isalpha(c) || c == '_')
        {
            return identifier();
        }
        else if (isdigit(c))
        {
            return number();
        }
        else
        {
            return symbol();
        }
    }

    std::vector< Token > Tokenizer::tokenize()
    {
        std::vector< Token > tokens;
        const Token* token = &getNextToken();
        while ( token->type != TokenType::EndOfFile )
        {
            tokens.push_back( *token );
            token = &getNextToken();
        }
        tokens.push_back( setToken( TokenType::EndOfFile, "" ) );
        return tokens;
    }

    bool Tokenizer::isAtEnd() const
    {
        return pos >= size;
    }

    char Tokenizer::peek() const
    {
        return isAtEnd() ? '\0' : source[pos];
    }

    char Tokenizer::advance()
    {
        char c = source[ pos++ ];
        if (c == '\n')
        {
            line++;
            column = 1;
        }
        else column++;
        return c;
    }

    bool isSpace(int c)
    {
        switch (c)
        {
            case ' ':
            case '\f':
            case '\r':
            case '\t':
            case '\v': return true;
            default: return false;
        }
    }

    void Tokenizer::skipWhitespaceAndComments()
    {
        while ( !isAtEnd() )
        {
            char c = peek();
            if (isspace(c))
            {
                advance();
                continue;
            }
            if (c == '/' && pos + 1 < size)
            {
                if (source[pos + 1] == '/')
                {   // single line
                    while (!isAtEnd() && peek() != '\n') advance();
                    continue;
                }
                if (source[pos + 1] == '*')
                {   // multi-line
                    pos += 2;
                    while (!isAtEnd() && !(peek() == '*' && source[pos + 1] == '/'))
                        advance();
                    pos += 2;
                    continue;
                }
            }
            break;
        }
    }

    const Token& Tokenizer::setToken(TokenType type, const std::string& lexeme)
    {
        curr_token.type = type;
        curr_token.column = column;
        curr_token.line = line;
        curr_token.lexeme = lexeme;
        return curr_token;
    }

    const Token& Tokenizer::symbol()
    {
        char c = advance();
        switch (c)
        {
            case '#': return preprocessor();
            case '(': return setToken( TokenType::LParen,   "(" );
            case ')': return setToken( TokenType::RParen,   ")" );
            case '{': return setToken( TokenType::LBrace,   "{" );
            case '}': return setToken( TokenType::RBrace,   "}" );
            case '[': return setToken( TokenType::LBracket, "[" );
            case ']': return setToken( TokenType::RBracket, "]" );
            case ',': return setToken( TokenType::Comma,    "," );
            case ';': return setToken( TokenType::Semicolon,";" );
            case ':': return setToken( TokenType::Colon,    ":" );
            case '=': return setToken( TokenType::Equal,    "=" );
            case '.': return setToken( TokenType::Dot,      "." );
            case '+': return setToken( TokenType::Operator, "+" );
            case '-': return setToken( TokenType::Operator, "-" );
            case '*': return setToken( TokenType::Operator, "*" );
            case '/': return setToken( TokenType::Operator, "/" );
            case 0:   return setToken( TokenType::EndOfFile,"" );
            default:  return setToken( TokenType::Unknown,  std::string(1, c) );
        }
    }

    const Token& Tokenizer::preprocessor()
    {
        std::string directive;
        directive += advance();
        while (!isAtEnd() && peek() != '\n')
        {
            directive += advance();
        }

        // Consume the newline (if any)
        if (!isAtEnd() && peek() == '\n')
            advance();

        token_queue.push(setToken( TokenType::NewLine, "\n" ));
        return setToken( TokenType::Preprocessor, directive );
    }

    const Token& Tokenizer::number()
    {
        size_t start = pos;
        while ( isdigit( peek() ) ) advance();
        std::string lex((pos - start), 0);
        strncpy(lex.data(), source + start, pos - start);
        return setToken( TokenType::Number, lex );
    }

    const Token& Tokenizer::identifier()
    {
        size_t start = pos;
        while ( isalnum( peek() ) || peek() == '_' ) advance();

        std::string lex((pos - start), 0);
        strncpy(lex.data(), source + start, pos - start);

        // Keyword mapping
        static const std::unordered_map< std::string, TokenType > keywords =
        {
            { "in",                     TokenType::Qualifier },
            { "out",                    TokenType::Qualifier },
            { "const",                  TokenType::Qualifier },
            { "shared",                 TokenType::Qualifier },
            { "buffer",                 TokenType::Qualifier },
            { "uniform",                TokenType::Qualifier },
            { "return",                 TokenType::Qualifier },
            { "void",                   TokenType::Type },
            { "float",                  TokenType::Type },
            { "int",                    TokenType::Type },
            { "uint",                   TokenType::Type },
            { "vec2",                   TokenType::Type },
            { "vec3",                   TokenType::Type },
            { "vec4",                   TokenType::Type },
            { "mat2",                   TokenType::Type },
            { "mat3",                   TokenType::Type },
            { "mat4",                   TokenType::Type },
            { "bool",                   TokenType::Type },
            { "double",                 TokenType::Type },
            { "dvec2",                  TokenType::Type },
            { "dvec3",                  TokenType::Type },
            { "dvec4",                  TokenType::Type },
            { "struct",                 TokenType::Type },
            { "push_block_layout",      TokenType::PushBlockLayout },
            { "vertex_layout",          TokenType::VertexLayout },
            { "set_layout",             TokenType::IndexedSetLayouts },
            { "in_layout",              TokenType::InLayout },
            { "out_layout",             TokenType::OutLayout },
            { "layout",                 TokenType::Layout },
            { "location",               TokenType::Location },
            { "binding",                TokenType::Binding },
            { "set",                    TokenType::Set },
            { "input_rate",             TokenType::InputRate },
            { "sampler",                TokenType::Sampler },
            { "sampled_image",          TokenType::SampledImage },
            { "storage_image",          TokenType::StorageImage },
            { "combined_image_sampler", TokenType::CombinedImageSampler },
            { "uniform_buffer",         TokenType::UniformBuffer },
            { "uniform_texel_buffer",   TokenType::UniformTexelBuffer },
            { "uniform_buffer_dynamic", TokenType::UniformBufferDynamic },
            { "storage_buffer",         TokenType::StorageBuffer },
            { "storage_texel_buffer",   TokenType::StorageTexelBuffer },
            { "storage_buffer_dynamic", TokenType::StorageBufferDynamic },
            { "input_attachment",       TokenType::InputAttachment },
        };

        auto itr = keywords.find( lex );
        if ( itr != keywords.end() )
        {
            return setToken( itr->second, lex );
        }

        return setToken( TokenType::Identifier, lex );
    }

    
    std::string toString(TokenType type)
    {
        switch (type) {
            // General
            case TokenType::EndOfFile:      return "EndOfFile";
            case TokenType::Identifier:     return "Identifier";
            case TokenType::Number:         return "Number";
            case TokenType::StringLiteral:  return "StringLiteral";

            // Symbols
            case TokenType::LParen:         return "LParen";
            case TokenType::RParen:         return "RParen";
            case TokenType::LBrace:         return "LBrace";
            case TokenType::RBrace:         return "RBrace";
            case TokenType::LBracket:       return "LBracket";
            case TokenType::RBracket:       return "RBracket";
            case TokenType::Comma:          return "Comma";
            case TokenType::Semicolon:      return "Semicolon";
            case TokenType::Colon:          return "Colon";
            case TokenType::Equal:          return "Equal";
            case TokenType::Dot:            return "Dot";
            case TokenType::HashTag:        return "HashTag";

            // Keywords
            case TokenType::VertexLayout:   return "VertexLayout";
            case TokenType::IndexedSetLayouts:      return "IndexedSetLayouts";
            case TokenType::Qualifier:      return "Qualifier";
            case TokenType::InLayout:       return "InLayout";
            case TokenType::OutLayout:      return "OutLayout";
            case TokenType::Location:       return "Location";
            case TokenType::Binding:        return "Binding";
            case TokenType::InputRate:      return "InputRate";

            // Types
            case TokenType::Type:           return "Type";

            case TokenType::Sampler:                return "Sampler";
            case TokenType::SampledImage:           return "SampledImage";
            case TokenType::StorageImage:           return "StorageImage";
            case TokenType::UniformBuffer:          return "UniformBuffer";
            case TokenType::UniformTexelBuffer:     return "UniformTexelBuffer";
            case TokenType::UniformBufferDynamic:   return "UniformBufferDynamic";
            case TokenType::StorageBuffer:          return "StorageBuffer";
            case TokenType::StorageTexelBuffer:     return "StorageTexelBuffer";
            case TokenType::StorageBufferDynamic:   return "StorageBufferDynamic";
            case TokenType::CombinedImageSampler:   return "CombinedImageSampler";
            case TokenType::InputAttachment:        return "InputAttachment";
            case TokenType::PushBlockLayout:        return "PushBlockLayout";

            // Misc
            case TokenType::Unknown:        return "Unknown";

            // Handle unassigned values gracefully
            default:                        return "Unknown";
        }
    }

}
