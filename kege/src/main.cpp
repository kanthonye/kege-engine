//
//  main.cpp
//  ui
//
//  Created by Kenneth Esdaile on 3/5/25.
//

#include "../src/editor/editor.hpp"

void test();

int main(int argc, const char * argv[])
{
    kege::Editor editor;
    return editor.run();
}

//namespace kege{
//
//    template< typename T > class Mem
//    {
//    public:
//
//        inline friend bool operator ==( const Mem& a, const Mem& b )
//        {
//            return a._ref == b._ref;
//        }
//
//        inline friend bool operator !=( const Mem& a, const Mem& b )
//        {
//            return a._ref != b._ref;
//        }
//
//        inline friend bool operator ==( const T* a, const Mem& b )
//        {
//            return a == b._ref;
//        }
//
//        inline friend bool operator !=( const Mem& a, const T* b )
//        {
//            return a._ref != b;
//        }
//
//        inline Mem& operator =( const Mem& a )
//        {
//            if ( a._ref == _ref ) return *this;
//            return initialize( a._ref );
//        }
//
//        inline Mem& operator =( Mem&& a )
//        {
//            if ( _ref != nullptr )
//                clear();
//
//            _ref = a._ref;
//            a._ref = nullptr;
//
//            return *this;
//        }
//
//        inline Mem& operator =( T* a )
//        {
//            return initialize( a );
//        }
//
//        inline const T* operator ->()const
//        {
//            return _ref;
//        }
//        inline T* operator ->()
//        {
//            return _ref;
//        }
//
//        inline const T* ref()const
//        {
//            return _ref;
//        }
//
//        inline T* ref()
//        {
//            return _ref;
//        }
//
//        inline const T& operator *()const
//        {
//            return *_ref;
//        }
//        
//        inline T& operator *()
//        {
//            return *_ref;
//        }
//
//        inline operator bool()const
//        {
//            return _ref != nullptr;
//        }
//
//        virtual void clear()
//        {
//            if( _ref != nullptr )
//            {
//                _ref->decrementMemMemCounter();
//                if( _ref->referenceMemCounter() <= 0 )
//                {
//                    delete _ref;
//                }
//                _ref = nullptr;
//            }
//        }
//
//        Mem( const Mem<T>& r )
//        :   _ref( nullptr )
//        {
//            initialize( r._ref );
//        }
//
//        Mem( Mem<T>&& r )
//        :   _ref( r._ref )
//        {
//            r._ref = nullptr;
//        }
//
//        Mem( T* r )
//        :   _ref( nullptr )
//        {
//            initialize( r );
//        }
//        Mem()
//        :   _ref( nullptr )
//        {}
//
//        virtual ~Mem()
//        {
//            clear();
//        }
//
//    protected:
//
//        Mem& initialize( T* a )
//        {
//            if ( _ref != nullptr )
//                clear();
//            _ref = a;
//
//            if ( a != nullptr )
//                _ref->MemCounter::incrementMemMemCounter();
//
//            return *this;
//        }
//
//        T* _ref;
//    };
//
//}
