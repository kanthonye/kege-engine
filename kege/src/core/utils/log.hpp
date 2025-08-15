//
//  log.hpp
//  vm
//
//  Created by Kenneth Esdaile on 6/13/25.
//

#ifndef log_hpp
#define log_hpp

#include <cstdarg>
#include <string>

#include <iostream>
#include <vector>

namespace kege{

    class Log {
    private:

        struct NL{};

    public:

        static NL nl;

        typedef std::vector< std::string > Entry;

        Log& operator <<( const std::string& val )
        {
            push( val );
            return *this;
        }
        Log& operator <<( const char* val )
        {
            push({ val });
            return *this;
        }
        Log& operator <<( int32_t val )
        {
            push( std::to_string( val ) );
            return *this;
        }
        Log& operator <<( uint32_t val )
        {
            push( std::to_string( val ) );
            return *this;
        }
        Log& operator <<( int64_t val )
        {
            push( std::to_string( val ) );
            return *this;
        }
        Log& operator <<( size_t val )
        {
            push( std::to_string( val ) );
            return *this;
        }
        Log& operator <<( uint64_t val )
        {
            push( std::to_string( val ) );
            return *this;
        }
        Log& operator <<( char val )
        {
            push( std::to_string( val ) );
            return *this;
        }
        Log& operator <<( uint8_t val )
        {
            push( std::to_string( val ) );
            return *this;
        }

        Log& operator <<( const Log::NL& nl )
        {
            push( "\n" );
            logs.push_back( new Entry );
            return *this;
        }

        Log& operator <<=( const std::string& b )
        {
            logs.back()->push_back( b );
            logs.back()->push_back( " " );
            return *this;
        }

        bool empty()const
        {
            return logs.empty();
        }

        void flush( FILE* file );

        Log( const std::string& name, const std::string& color );
        ~Log();
        Log();

        static Log warning;
        static Log debug;
        static Log error;
        static Log info;

    private:

        Entry* push( const std::string& msg )
        {
            if ( logs.empty() )
            {
                logs.push_back( new Entry );
                std::cout <<"\n" << name <<": ";
            }
            logs.back()->push_back( msg );
            std::cout << msg;
            return logs.back();
        }

    private:

        std::vector< Entry* > logs;
        std::string color;
        std::string name;
        int entry_index;
    };
}
#define KEGE_LOG_ERROR kege::Log::error   << "( "<<__FILE__ <<" ) -> " << __FUNCTION__ << "() -> "
#define KEGE_LOG_WARN  kege::Log::warning << "( "<<__FILE__ <<" ) -> " << __FUNCTION__ << "() -> "
#define KEGE_LOG_INFO  kege::Log::info
#define KEGE_LOG_DEBUG kege::Log::debug

#endif /* log_hpp */
