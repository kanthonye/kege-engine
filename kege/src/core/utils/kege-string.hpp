//
//  kege-string.hpp
//  CacheSystem
//
//  Created by Kenneth Esdaile on 3/12/23.
//

#ifndef kege_string_hpp
#define kege_string_hpp
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
namespace kege{

    class string
    {
    public:

        friend std::ostream& operator <<(std::ostream& os, const kege::string& str);

        //friend string operator +(const string& a, const string& b);
        //friend string operator +(const string& a, const char* b);
        friend string operator +(const char* a, const string& b);
        //friend string operator +(const string& a, int8_t b);
        friend string operator +(int8_t a, const string& b);

        friend bool operator ==(const string& a, const string& b);

        const char& operator [](uint64_t i)const;
        char& operator [](uint64_t i);

        string& operator +=( const string& str );
        string& operator +=( const char* str );
        string& operator +=( const char chr );
        
        string operator +( const string& str )const;
        string operator +( const char* str )const;
        string operator +( int chr )const;

        string& operator =( const string& str );
        string& operator =( const char* str );

        const char& operator *()const;
        char& operator *();

        operator bool()const;
        
        void copyFrom( uint64_t size, const char* str );
        bool cmp( const char* str )const;
        
//        string read( const char* flags )const;
//        int64_t position(const char* s)const;
//        string seekfwd(const char* s)const;
//        string seekbwd(const char* s)const;
//
//        string skipfwd(const char* s)const;
//        string skipbwd(const char* s)const;

        string parse_fname()const;
        string parse_fpath()const;
        string parse_name()const;

        string lowercase()const;
        string uppercase()const;

        void resize( uint64_t size );
        int64_t length()const;

        const char* c_str()const;
        char* str();

//        static std::size_t hash(const char* str);
//        std::size_t hash()const;

        double  toFloat()const;
        int64_t toInt64()const;
        int32_t toInt32()const;

        bool empty()const;
        void clear();


        string( char* str, uint64_t length );
        string( const char* str );

        // copy constructor
        string( const string& str );

        // move constructor
        string( string&& str );

        // default constructor
        string();

        // de-constructor
        ~string();

    private:

        uint64_t _length;
        char* _str;
    };


    kege::string dtostr(double n, int space);
    kege::string ftostr(float n, int space);
    kege::string itostr(int64_t n);
    kege::string itostr(int32_t n);

    const char* strcpy( char* buf, int size, const char* brks, const char* str );
    const char* strskp( const char* str, const char* skips );
    bool match( const char* s1, const char* s2 );

    bool ischar(char c, const char* s);
}


namespace std
{
    template <> struct hash< kege::string >
    {
        std::size_t operator()(const kege::string& str) const
        {
            if ( str.empty() ) return 0;
            return std::hash<std::string>()( str.c_str() );
        }
    };
}
#endif /* kege_string_hpp */
