//
//  array.hpp
//  kege
//
//  Created by Kenneth Esdaile on 12/6/24.
//

#ifndef kege_arr_hpp
#define kege_arr_hpp

#include <stddef.h>
#include <initializer_list>
#include <algorithm>
#include <stdexcept>
#include "../memory/ref.hpp"

namespace kege{

    /**
     * Dynamic array implementation
     */
    template <typename T> class array
    {
    private:

        void reset()
        {
            _data = nullptr;
            _capacity = 0;
        }

    public:

        class iterator
        {
        public:
            // Iterator traits for STL compatibility
            using pointer = T*;
            using reference = T&;

            // Constructor
            iterator(pointer ptr = nullptr) : _ptr(ptr) {}

            // Postfix increment
            iterator operator++(int) { iterator tmp = *this; ++_ptr; return tmp; }

            // Prefix increment
            iterator& operator++() { ++_ptr; return *this; }

            // Postfix decrement
            iterator operator--(int) { iterator tmp = *this; --_ptr; return tmp; }

            // Prefix decrement
            iterator& operator--() { --_ptr; return *this; }

            // Dereference operators
            reference operator*() const { return *_ptr; }
            pointer operator->() const { return _ptr; }

            // Arithmetic operators
            iterator operator+(size_t n) const { return iterator(_ptr + n); }
            iterator operator-(size_t n) const { return iterator(_ptr - n); }
            size_t operator-(const iterator& other) const { return _ptr - other._ptr; }

            // Compound assignment
            iterator& operator+=(size_t n) { _ptr += n; return *this; }
            iterator& operator-=(size_t n) { _ptr -= n; return *this; }

            // Subscript operator
            reference operator[](size_t n) const { return _ptr[n]; }

            // Comparison operators
            bool operator==(const iterator& other) const { return _ptr == other._ptr; }
            bool operator!=(const iterator& other) const { return _ptr != other._ptr; }
            bool operator <(const iterator& other) const { return _ptr < other._ptr; }
            bool operator >(const iterator& other) const { return _ptr > other._ptr; }
            bool operator<=(const iterator& other) const { return _ptr <= other._ptr; }
            bool operator>=(const iterator& other) const { return _ptr >= other._ptr; }

        private:

            pointer _ptr;
        };

    public:

        //const_iterator begin() { return iterator(_data); }
        //const_iterator end() { return iterator(_data + _capacity); }

        iterator begin() { return iterator(_data); }
        iterator end() { return iterator(_data + _capacity); }

        const T& operator[](size_t index) const
        {
            return _data[index];
        }

        T& operator[](size_t index)
        {
            return _data[index];
        }

        const T& at(size_t index) const
        {
            if (index >= _capacity) throw std::out_of_range("Index out of bounds");
            return _data[index];
        }

        T& at(size_t index)
        {
            if (index >= _capacity) throw std::out_of_range("Index out of bounds");
            return _data[index];
        }

        void clear()
        {
            if( _data != nullptr )
            {
                delete[] _data;
                reset();
            }
        }

        // Size accessor
        size_t size() const { return _capacity; }

        const T* data() const { return _data; }
        T* data() { return _data; }

        void resize( size_t size )
        {
            if ( size == 0 )
            {
                clear();
                return;
            }

            T* data = new T[ size ];
            if ( _data != nullptr )
            {
                size_t min_capacity = (size < _capacity)? size : _capacity;
                std::copy(_data, _data + min_capacity, data);
                delete[] _data;
            }
            _data = data;
            _capacity = size;
        }

        bool empty()const
        {
            return _capacity == 0;
        }

        void replace( array& other )
        {
            _data = other._data;
            _capacity = other._capacity;
            other.reset();
        }

        void swap( array& other )
        {
            T* old = _data;
            _data = other._data;
            other._data = old;

            size_t oldsize = _capacity;
            _capacity = other._capacity;
            other._capacity = oldsize;
        }

        // Assignment operators
        array& operator=(const array& other)
        {
            if (this != &other)
            {
                if( _data != nullptr ) delete[] _data;
                if ( other._data != nullptr )
                {
                    _capacity = other._capacity;
                    _data = new T[_capacity];
                    std::copy(other._data, other._data + _capacity, _data);
                }
            }
            return *this;
        }

        array& operator=( array&& other ) noexcept
        {
            if (this != &other)
            {
                if( _data != nullptr ) delete[] _data;

                _data = other._data;
                _capacity = other._capacity;
                other.reset();
            }
            return *this;
        }

        // Constructor from initializer_list
        array( const std::initializer_list<T>& init )
        :   _capacity(init.size())
        {
            _data = new T[_capacity];
            std::copy(init.begin(), init.end(), _data);
        }

        // Copy constructor
        array( const array& other )
        :   _capacity( other._capacity )
        {
            _data = new T[_capacity];
            std::copy(other._data, other._data + _capacity, _data);
        }

        // Move constructor
        array( array&& other ) noexcept
        :   _data(other._data)
        ,   _capacity(other._capacity)
        {
            other.reset();
        }

        // default constructor
        array( size_t size )
        :   _data( nullptr )
        ,   _capacity( 0 )
        {
            resize( size );
        }

        // default constructor
        array()
        :   _data( nullptr )
        ,   _capacity( 0 )
        {
            //clear();
        }

        // Destructor
        ~array()
        {
            clear();
        }

    private:

        T* _data;
        size_t _capacity;
    };

}




namespace kege{

    /**
     * Dynamic array (vector) implementation
     */
    template <typename T> class vector : public kege::RefCounter
    {
    public:

        class iterator
        {
        public:
            // Iterator traits for STL compatibility
            using pointer = T*;
            using reference = T&;

            // Constructor
            iterator(pointer ptr = nullptr) : _ptr(ptr) {}

            // Postfix increment
            iterator operator++(int) { iterator tmp = *this; ++_ptr; return tmp; }

            // Prefix increment
            iterator& operator++() { ++_ptr; return *this; }

            // Postfix decrement
            iterator operator--(int) { iterator tmp = *this; --_ptr; return tmp; }

            // Prefix decrement
            iterator& operator--() { --_ptr; return *this; }

            // Dereference operators
            reference operator*() const { return *_ptr; }
            pointer operator->() const { return _ptr; }

            // Arithmetic operators
            iterator operator+(size_t n) const { return iterator(_ptr + n); }
            iterator operator-(size_t n) const { return iterator(_ptr - n); }
            size_t operator-(const iterator& other) const { return _ptr - other._ptr; }

            // Compound assignment
            iterator& operator+=(size_t n) { _ptr += n; return *this; }
            iterator& operator-=(size_t n) { _ptr -= n; return *this; }

            // Subscript operator
            reference operator[](size_t n) const { return _ptr[n]; }

            // Comparison operators
            bool operator==(const iterator& other) const { return _ptr == other._ptr; }
            bool operator!=(const iterator& other) const { return _ptr != other._ptr; }
            bool operator <(const iterator& other) const { return _ptr < other._ptr; }
            bool operator >(const iterator& other) const { return _ptr > other._ptr; }
            bool operator<=(const iterator& other) const { return _ptr <= other._ptr; }
            bool operator>=(const iterator& other) const { return _ptr >= other._ptr; }

        private:

            pointer _ptr;
        };

        class const_iterator
        {
        public:
            // Iterator traits for STL compatibility
            using pointer = const T*;
            using reference = const T&;

            // Constructor
            const_iterator(pointer ptr = nullptr) : _ptr(ptr) {}

            // Postfix increment
            const_iterator operator++(int) { iterator tmp = *this; ++_ptr; return tmp; }

            // Prefix increment
            const_iterator& operator++() { ++_ptr; return *this; }

            // Postfix decrement
            const_iterator operator--(int) { iterator tmp = *this; --_ptr; return tmp; }

            // Prefix decrement
            const_iterator& operator--() { --_ptr; return *this; }

            // Dereference operators
            reference operator*() const { return *_ptr; }
            pointer operator->() const { return _ptr; }

            // Arithmetic operators
            const_iterator operator+(size_t n) const { return const_iterator(_ptr + n); }
            const_iterator operator-(size_t n) const { return const_iterator(_ptr - n); }
            size_t operator-(const const_iterator& other) const { return _ptr - other._ptr; }

            // Compound assignment
            const_iterator& operator+=(size_t n) { _ptr += n; return *this; }
            const_iterator& operator-=(size_t n) { _ptr -= n; return *this; }

            // Subscript operator
            reference operator[](size_t n) const { return _ptr[n]; }

            // Comparison operators
            bool operator==(const const_iterator& other) const { return _ptr == other._ptr; }
            bool operator!=(const const_iterator& other) const { return _ptr != other._ptr; }
            bool operator <(const const_iterator& other) const { return _ptr < other._ptr; }
            bool operator >(const const_iterator& other) const { return _ptr > other._ptr; }
            bool operator<=(const const_iterator& other) const { return _ptr <= other._ptr; }
            bool operator>=(const const_iterator& other) const { return _ptr >= other._ptr; }

        private:

            pointer _ptr;
        };

    public:

        /**
         * Begin and end const_iterator
         */
        const_iterator cbegin() { return const_iterator( _arr.data() ); }
        const_iterator cend() { return const_iterator( _arr.data() + _count ); }

        /**
         * Begin and end iterators
         */
        iterator begin() { return iterator( _arr.data() ); }
        iterator end() { return iterator( _arr.data() + _count ); }

        /**
         * Add element to the end of the vector
         */
        void push_back( T t )
        {
            if ( _count >= _arr.size() ) _arr.resize((_count == 0)? 16: 2 * _count );
            _arr[ _count ] = t;
            _count += 1;
        }

        /**
         * Erase element at position pos
         */
        void erase(iterator pos)
        {
            if (pos == end() || empty()) return;

            // Calculate the index
            size_t index = pos - begin();

            // Move elements after the erased position one slot forward
            if (index < _count - 1) {
                std::move(_arr.data() + index + 1, _arr.data() + _count, _arr.data() + index);
            }

            // Decrease the count
            _count--;
        }

        /**
         * Access operators
         */
        const T& operator[](size_t index) const { return _arr[index]; }
        T& operator[](size_t index) { return _arr[index]; }

        /**
         * at() with bounds checking
         */
        const T& at(size_t index) const { return _arr[index]; }
        T& at(size_t index) { return _arr[index]; }

        /**
         * Reduce the size of the array if less than half is used
         * @return true if the array was swinked, false otherwise
         */
        bool swinked()
        {
            size_t swink_size = (_arr.size() - 1) * 0.5;
            if (_count <= swink_size)
            {
                _arr.resize( swink_size );
                return true;
            }
            return false;
        }

        /**
         * Clear the vector
         */
        void clear()
        {
            _arr.clear();
            _count = 0;
        }

        /**
         * Size accessor
         */
        size_t size() const { return _count; }

        /**
         * Data accessor
         */
        const T* data() const { return _arr.data(); }
        T* data() { return _arr.data(); }

        /**
         * Reserve space in the array
         */
        void reserve( size_t size ) { _arr.resize( size ); }

        /**
         * Resize the vector
         */
        void resize( size_t size )
        {
            _arr.resize( size );
            _count = _arr.size();
        }

        /**
         * Check if the vector is empty
         */
        bool empty() const { return _count == 0; }

        /**
         * Replace the contents of the vector with another
         */
        void replace( vector& other )
        {
            _arr.replace( other._arr );
            _count = other._count;
        }

        /**
         * Swap the contents of the vector with another
         */
        void swap( vector& other )
        {
            _arr.swap( other );
            _count = other._count;

            size_t old_count = other._count;
            other._count = _count;
            _count = old_count;
        }

        /**
         * Assignment operators
         */
        vector& operator=(const vector& other)
        {
            _arr.operator=( other );
            _count = other._count;
            return *this;
        }

        /**
         * Move assignment operator
         */
        vector& operator=( vector&& other ) noexcept
        {
            _arr.operator=( other );
            _count = other._count;
            return *this;
        }

        /**
         * Constructor from initializer_list
         */
        vector( const std::initializer_list<T>& init )
        :   _arr( init )
        ,   _count( init.size() )
        {
        }

        /**
         * Copy constructor
         */
        vector( const vector& other )
        :   _arr( other._arr )
        ,   _count( other._count )
        {
        }

        /**
         * Move constructor
         */
        vector( vector&& other ) noexcept
        :   _arr( other._arr )
        ,   _count( other._count )
        {
            other._arr.reset();
            other._count = 0;
        }

        /**
         * construct vector with given size
         */
        vector( size_t size )
        :   _arr( size )
        ,   _count( 0 )
        {
            resize( size );
        }

        /**
         * default constructor
         */
        vector()
        :   _count( 0 )
        {}

        /**
         * Destructor
         */
        ~vector()
        {
            clear();
        }

    private:

        array< T > _arr;
        size_t _count;
    };

}
#endif /* kege_arr_hpp */
