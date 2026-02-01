//
//  kege-string.hpp
//  CacheSystem
//
//  Created by Kenneth Esdaile on 3/12/23.
//

#ifndef kege_string_hpp
#define kege_string_hpp

#include <assert.h>
#include <cstdint>
#include <cstddef>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <iostream>

#ifdef KEGE_DEBUG
    #define ASSERT(cond, msg) \
        do { \
            if (!(cond)) { \
                fprintf(stderr, "ASSERTION FAILED: %s\nFile: %s:%d\n", msg, __FILE__, __LINE__); \
                std::abort(); \
            } \
        } while(0)
#else
    #define ASSERT(cond, msg) do {} while(0)
#endif

namespace kege{

    class string
    {
    public:

        friend std::ostream& operator <<(std::ostream& os, const kege::string& str);

        friend  bool operator <(const string& a, const string& b);
        
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
        string& operator +=( int chr );

        string operator +( const string& str )const;
        string operator +( const char* str )const;
        string operator +( int chr )const;

        string& operator =( const string& str );
        string& operator =( const char* str );

        const char& operator *()const;
        char& operator *();

        //explicit operator std::string()const;
        operator bool()const;

        string& copyFrom( uint64_t offset, uint64_t size, const char* str );
        string& copyTo( uint64_t offset, uint64_t size, char* str );

        string& copyFrom( uint64_t offset, const string& str );
        string& copyTo( uint64_t offset, string& str );

        bool match( uint64_t offset, const char* str )const;

        void insert(size_t pos, size_t count, char ch);
        void insert(size_t pos, const char* str);
        void erase(size_t pos, size_t count);
        bool find( char c )const;

        string parseFileName()const;
        string parseFilePath()const;
        string parseFileExt()const;
        string parseName()const;

        string lowercase()const;
        string uppercase()const;

        void resize( uint64_t size );
        int64_t length()const;

        const char* c_str()const;
        char* str();

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

    struct StringHash // FNV-1a 64-bit (Simple, Fast, Excellent for Strings)
    {
        static constexpr uint64_t offset = 14695981039346656037ULL;
        static constexpr uint64_t prime = 1099511628211ULL;

        std::size_t operator()( const kege::string& s ) const noexcept
        {
            uint64_t hash = offset;
            for (size_t i = 0; i < s.length(); ++i)
            {
                hash ^= static_cast<uint64_t>(static_cast<unsigned char>(s[i]));
                hash *= prime;
            }
            return static_cast<std::size_t>(hash);
        }
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
    template<> struct hash< kege::string > : kege::StringHash {};
}

#endif /* kege_string_hpp */
