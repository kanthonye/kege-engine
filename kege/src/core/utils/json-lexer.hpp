//
//  json-glsl-lexer.hpp
//  kege
//
//  Created by Kenneth Esdaile on 6/15/24.
//

#ifndef json_glsl_lexer_hpp
#define json_glsl_lexer_hpp

#include <string>
#include <vector>

namespace kege{
    class JsonLexer
    {
    public:

        enum Token
        {
            ERROR = -4,
            END_STREAM,
            END,
            START,
            INTEGER,
            FLOAT,
            FLOAT_E,
            FLOAT_EX,
            STRING,
            COLON,
            COMMA,
            OPEN_CURLY_BRACKET,
            CLOSE_CURLY_BRACKET,
            OPEN_SQUARE_BRACKET,
            CLOSE_SQUARE_BRACKET,
            MAX_TOKEN_COUNT,
        };

    public:

        static const char* toString( int token );
        static bool load( std::vector< char >& file_bufr, const char* filename );
        const char* getLexeme();
        Token getNextToken();
        int getCurrLine();
        void clear();
        bool eos()const;

        JsonLexer(const char* curr_char);
        virtual ~JsonLexer();
        JsonLexer();

    protected:

        std::string lexeme;
        const char* _curr_char;
        std::string _error;
        int _line_count;
    };

}

#endif /* json_glsl_lexer_hpp */
