#ifndef lexer_hpp
#define lexer_hpp

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "glsl-tokens.hpp"
#include "glsl-token-table.hpp"
#include "glsl-preprocessor.hpp"

namespace kege{namespace glsl{

    class Lexer
    {
    public:

        void begin( const std::vector< char >* buffer );

        const char* getPrevLexeme()const;
        const char* getLexeme();

        Token getNextToken();
        
        int getCurrLine();
        void revert();

        void clear();
        void print();


        Lexer( const std::vector< char >* buffer );
        virtual ~Lexer();
        Lexer();

    protected:

        HashTable< uint32_t >* token_table;
        std::vector< char > writebufr;

        const char* prev_lexeme;
        char* lexeme;

        const char* prev_position;
        const char* curr_position;

        std::string _error;
        int line_count;
    };

}}
#endif
