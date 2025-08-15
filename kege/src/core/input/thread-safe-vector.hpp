//
//  thread-safe-vector.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 1/12/25.
//

#ifndef thread_safe_vector_hpp
#define thread_safe_vector_hpp

#include <mutex>
#include <vector>

namespace kege
{
    template < typename T > class ThreadSafeVector {
    public:

        // Constructor
        ThreadSafeVector();

        std::vector< T >* operator ->();

        // Add an element to the vector
        void swapOut( std::vector< T >& vec );

        // Add an element to the vector
        void push(const T& element);

        // Reset the count to 0
        void reset();

        // Get the size of the vector
        size_t size() const;

        // Get the current count
        int getCount() const;

        // Access an element by index (thread-safe)
        T getElement(size_t index) const;

    private:

        mutable std::mutex _mutex; // Mutex for thread safety
        std::vector< T > _vec;       // The underlying vector
        int _count;                // Tracks the number of elements
    };

    template < typename T > std::vector< T >* ThreadSafeVector< T >::operator ->()
    {
        return &_vec;
    }

    template < typename T > void ThreadSafeVector< T >::swapOut( std::vector< T >& vec )
    {
        std::lock_guard<std::mutex> lock( _mutex );
        vec.swap( _vec );
    }

    // Constructor
    template < typename T > ThreadSafeVector< T >::ThreadSafeVector() : _count(0) {}

    // Add an element to the vector
    template < typename T > void ThreadSafeVector< T >::push(const T& element)
    {
        std::lock_guard<std::mutex> lock( _mutex );
        _vec.push_back( element );
    }

    // Reset the count to 0
    template < typename T > void ThreadSafeVector< T >::reset()
    {
        std::lock_guard<std::mutex> lock( _mutex );
        _count = 0;
    }

    // Get the size of the vector
    template < typename T > size_t ThreadSafeVector< T >::size() const
    {
        std::lock_guard<std::mutex> lock( _mutex );
        return _vec.size();
    }

    // Get the current count
    template < typename T > int ThreadSafeVector< T >::getCount() const
    {
        std::lock_guard<std::mutex> lock( _mutex );
        return _count;
    }

    // Access an element by index (thread-safe)
    template < typename T > T ThreadSafeVector< T >::getElement(size_t index) const
    {
        std::lock_guard<std::mutex> lock( _mutex );
        if ( index < _vec.size() ) {
            return _vec[index];
        }
        throw std::out_of_range("Index out of range");
    }

}
#endif /* thread_safe_vector_hpp */
