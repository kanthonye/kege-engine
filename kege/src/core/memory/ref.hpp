//
//  Ref.hpp
//  KEGE
//
//  Created by Kenneth Esdaile on 1/15/23.
//

#ifndef KEGE_REF
#define KEGE_REF

#include <stdlib.h>

namespace kege
{

    class RefCounter
    {
    public:

        inline int  referenceRefCounter()const { return _ref_counter; }
        inline void decrementRefRefCounter()const { _ref_counter--; }
        inline void incrementRefRefCounter()const { _ref_counter++; }
        
    protected:

        RefCounter(): _ref_counter( 0 ) {}
        virtual ~RefCounter(){}

    private:

        mutable int _ref_counter;
    };
    
}


namespace kege{

    template< typename T > class Ref
    {
    public:

        inline friend bool operator ==( const Ref& a, const Ref& b )
        {
            return a._ref == b._ref;
        }
        inline friend bool operator !=( const Ref& a, const Ref& b )
        {
            return a._ref != b._ref;
        }
        
        inline friend bool operator ==( const T* a, const Ref& b )
        {
            return a == b._ref;
        }
        inline friend bool operator !=( const Ref& a, const T* b )
        {
            return a._ref != b;
        }
        
        inline Ref& operator =( const Ref& a )
        {
            if ( a._ref == _ref ) return *this;
            return initialize( a._ref );
        }

        inline Ref& operator =( Ref&& a )
        {
            if ( _ref != nullptr )
                clear();

            _ref = a._ref;
            a._ref = nullptr;

            return *this;
        }
         
        inline Ref& operator =( T* a )
        {
            return initialize( a );
        }

        inline const T* operator ->()const
        {
            return _ref;
        }
        inline T* operator ->()
        {
            return _ref;
        }
        
        inline const T* ref()const
        {
            return _ref;
        }
        inline T* ref()
        {
            return _ref;
        }
        
        inline const T& operator *()const
        {
            return *_ref;
        }
        inline T& operator *()
        {
            return *_ref;
        }

        inline operator bool()const
        {
            return _ref != nullptr;
        }

        virtual void clear()
        {
            if( _ref != nullptr )
            {
                _ref->decrementRefRefCounter();
                if( _ref->referenceRefCounter() <= 0 )
                {
                    delete _ref;
                }
                _ref = nullptr;
            }
        }

        Ref( const Ref<T>& r )
        :   _ref( nullptr )
        {
            initialize( r._ref );
        }

        Ref( Ref<T>&& r )
        :   _ref( r._ref )
        {
            r._ref = nullptr;
        }

        Ref( T* r )
        :   _ref( nullptr )
        {
            initialize( r );
        }
        Ref()
        :   _ref( nullptr )
        {}
        
        virtual ~Ref()
        {
            clear();
        }
        
    protected:

        Ref& initialize( T* a )
        {
            if ( _ref != nullptr )
                clear();
            _ref = a;

            if ( a != nullptr )
                _ref->RefCounter::incrementRefRefCounter();

            return *this;
        }

        T* _ref;
    };

}

#endif /* KEGE_REF */
