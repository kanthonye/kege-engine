//
//  lexer.hpp
//  xml
//
//  Created by Kenneth Esdaile on 11/21/24.
//

#ifndef lexer_hpp
#define lexer_hpp

#include <stdlib.h>
#include <vector>
#include <string>

namespace kege
{

#define LEXER_TOKENS \
\
LEXER_TOKEN( ERROR ), \
LEXER_TOKEN( END_STREAM ), \
LEXER_TOKEN( END ), \
LEXER_TOKEN( START ), \
\
LEXER_TOKEN( WHITE_SPACE ), \
LEXER_TOKEN( NEWLINE ), \
\
LEXER_TOKEN( PLUS ), \
LEXER_TOKEN( MINUS ), \
LEXER_TOKEN( EQUAL ), \
LEXER_TOKEN( PERCENT_SIGN ), \
LEXER_TOKEN( AMPERSAND ), \
LEXER_TOKEN( LESS_THAN ), \
LEXER_TOKEN( GREATER_THAN ), \
LEXER_TOKEN( L_SQUARE_BRACKET ), \
LEXER_TOKEN( R_SQUARE_BRACKET ), \
LEXER_TOKEN( L_CURLY_BRACKET ), \
LEXER_TOKEN( R_CURLY_BRACKET ), \
LEXER_TOKEN( L_PARENTHESIS ), \
LEXER_TOKEN( R_PARENTHESIS ), \
LEXER_TOKEN( SEMICOLON ), \
LEXER_TOKEN( COLON ), \
LEXER_TOKEN( COMMA ), \
LEXER_TOKEN( DOUBLE_QUOTE ), \
LEXER_TOKEN( SINGLE_QUOTE ), \
LEXER_TOKEN( PERIOD ), \
LEXER_TOKEN( NUMBER_SIGN ), \
LEXER_TOKEN( AT_SIGN ), \
LEXER_TOKEN( DOLLAR ), \
LEXER_TOKEN( ASTERISK ), \
LEXER_TOKEN( SLASH ), \
LEXER_TOKEN( BACK_SLASH ), \
LEXER_TOKEN( UNDER_SCORE ), \
LEXER_TOKEN( CARET ), \
LEXER_TOKEN( QUESTION_MARK ), \
LEXER_TOKEN( VERTICAL_BAR ), \
LEXER_TOKEN( GRAVE_ACCENT), \
LEXER_TOKEN( EQUIVALENCY_SIGN ), \
LEXER_TOKEN( EXCLAMATION_MARK ), \
\
LEXER_TOKEN( IDENTIFIER ), \
LEXER_TOKEN( INTEGER ), \
LEXER_TOKEN( FLOAT ), \
LEXER_TOKEN( FLOAT_E ), \
LEXER_TOKEN( FLOAT_EE ), \
LEXER_TOKEN( STRING ), \
\
LEXER_TOKEN( COMMENT ), \
LEXER_TOKEN( MAX_COUNT )


    class Lexer
    {
    public:

        static bool charIsWhiteSpace( char c );
        static bool charIsNumeric( char c );
        static bool charIsId( char c );

        void setBufferSource( uint64_t size, const char* bufr );
        uint64_t getRemaindingCharCount()const;
        uint64_t getReadCharCount()const;

        virtual uint32_t getToken() = 0;

        const char* getLexeme();
        void operator +=(int n);
        void operator -=(int n);
        void clear();

        virtual ~Lexer();
        Lexer();

    protected:

        uint64_t _buffer_size;
        const char* _buffer;
        const char* _cstr;
        std::string _lexeme;
        std::string _error;
    };

}
#endif /* lexer_hpp */
