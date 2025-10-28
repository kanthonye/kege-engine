//
//  tokenizer.hpp
//  logr
//
//  Created by Kenneth Esdaile on 10/19/25.
//

#ifndef tokenizer_hpp
#define tokenizer_hpp

#include <string>
#include <vector>
#include <queue>
#include <unordered_map>

namespace kege::glsl
{
    enum class TokenType
    {
        // General
        EndOfFile,
        Identifier,
        Number,
        StringLiteral,
        NewLine,

        // Symbols
        LParen,     // (
        RParen,     // )
        LBrace,     // {
        RBrace,     // }
        LBracket,   // [
        RBracket,   // ]
        Comma,      // ,
        Semicolon,  // ;
        Colon,      // :
        Equal,      // =
        Dot,        // .
        HashTag,    // #

        Operator,
        Preprocessor,

        // Qualifier
        Qualifier, // in, out, const, shared, buffer, uniform,

        // Keywords
        VertexLayout,   // vertex_layout
        SetLayout,      // set_layout
        InLayout,       // in
        OutLayout,      // out
        Layout,         // layout
        Location,
        Binding,
        Set,
        InputRate,

        Sampler,
        SampledImage,
        StorageImage,
        UniformBuffer,
        UniformTexelBuffer,
        UniformBufferDynamic,
        StorageBuffer,
        StorageTexelBuffer,
        StorageBufferDynamic,
        CombinedImageSampler,
        InputAttachment,
        PushBlockLayout,

        // Types (can be parsed dynamically too)
        Type,           // vec3, mat4, float, int, etc.

        // Misc
        Unknown
    };

    struct Token
    {
        TokenType type;
        std::string lexeme;
        int line;
        int column;
    };

    class Tokenizer
    {
    public:

        Tokenizer(size_t size, const char* src);

        const Token& getCurrToken()const;

        const Token& getNextToken();

        std::vector< Token > tokenize();
        
    private:

        const Token& preprocessor();
        
        bool isAtEnd() const;

        char peek() const;

        char advance();

        void skipWhitespaceAndComments();

        const Token& setToken(TokenType type, const std::string& lexeme);

        const Token& symbol();

        const Token& number();

        const Token& identifier();
        
    private:

        std::queue<Token> token_queue;
        const char* source;
        Token curr_token;
        size_t size;
        size_t pos;
        int column;
        int line;
    };

    // Function to convert TokenType to string
    std::string toString(TokenType type);
    
}

#endif /* tokenizer_hpp */
