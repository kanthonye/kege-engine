//
//  lexer.cpp
//  xml
//
//  Created by Kenneth Esdaile on 11/21/24.
//

#include "lexer.hpp"

namespace kege
{
    bool Lexer::charIsNumeric( char c )
    {
        return ( c >= '0' && '9' >= c );
    }

    bool Lexer::charIsWhiteSpace( char c )
    {
        return c == ' ' || c == '\t';
    }

    bool Lexer::charIsId( char c )
    {
        return
        ( c >= 'A' && 'Z' >= c ) ||
        ( c >= 'a' && 'z' >= c ) ||
        ( c >= '0' && '9' >= c ) ||
        c == '_' || c == '-' || c == '/' || c == '.'
        ;
    }

    void Lexer::setBufferSource( uint64_t size, const char* bufr )
    {
        _buffer_size = size;
        _buffer = bufr;
        _cstr = bufr;
    }

    uint64_t Lexer::getRemaindingCharCount()const
    {
        return (_buffer == nullptr ) ? 0 : &_buffer[ _buffer_size - 1 ] - _cstr;
    }

    uint64_t Lexer::getReadCharCount()const
    {
        return (_buffer == nullptr ) ? 0 : _cstr - &_buffer[ 0 ];
    }

    const char* Lexer::getLexeme()
    {
        return _lexeme.data();
    }

    void Lexer::operator +=(int n)
    {
        _cstr += n;
    }
    void Lexer::operator -=(int n)
    {
        _cstr -= n;
    }

    void Lexer::clear()
    {
        _lexeme.clear();
    }

    Lexer:: ~Lexer()
    {
        clear();
    }

    Lexer::Lexer()
    :   _buffer( nullptr )
    ,   _cstr( nullptr )
    {
        _lexeme.resize( 128 );
        _lexeme[0] = 0;
    }



}
