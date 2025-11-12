//
//  array.hpp
//  kege
//
//  Created by Kenneth Esdaile on 12/6/24.
//

#ifndef kege_array_hpp
#define kege_array_hpp

#include <stddef.h>
#include <initializer_list>
#include <algorithm>
#include <stdexcept>
#include "../memory/ref.hpp"

namespace kege{

    template <typename T> class array : public kege::RefCounter
    {
    public:

        const T& operator[](size_t index) const
        {
            if (index >= _size) throw std::out_of_range("Index out of bounds");
            return _data[index];
        }

        T& operator[](size_t index)
        {
            if (index >= _size) throw std::out_of_range("Index out of bounds");
            return _data[index];
        }

        const T& at(size_t index) const
        {
            if (index >= _size) throw std::out_of_range("Index out of bounds");
            return _data[index];
        }

        T& at(size_t index)
        {
            if (index >= _size) throw std::out_of_range("Index out of bounds");
            return _data[index];
        }

        void clear()
        {
            if( _data != nullptr )
            {
                delete[] _data;
                _size = 0;
            }
        }

        // Size accessor
        size_t size() const { return _size; }

        // Iterators
        const T* begin() const { return _data; }
        T* begin() { return _data; }

        const T* end() const { return _data + _size; }
        T* end() { return _data + _size; }

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
                size_t min_size = (size < _size)? size : _size;
                std::copy(_data, _data + min_size, data);
                delete[] _data;
            }
            _data = data;
            _size = size;
        }

        // Assignment operators
        array& operator=(const array& other)
        {
            if (this != &other)
            {
                delete[] _data;
                _size = other._size;
                _data = new T[_size];
                std::copy(other._data, other._data + _size, _data);
            }
            return *this;
        }

        array& operator=( array&& other ) noexcept
        {
            if (this != &other)
            {
                delete[] _data;
                _data = other._data;
                _size = other._size;
                other._data = nullptr;
                other._size = 0;
            }
            return *this;
        }

        // Constructor from initializer_list
        array( const std::initializer_list<T>& init )
        :   _size(init.size())
        {
            _data = new T[_size];
            std::copy(init.begin(), init.end(), _data);
        }

        // Copy constructor
        array( const array& other )
        :   _size( other._size )
        {
            _data = new T[_size];
            std::copy(other._data, other._data + _size, _data);
        }

        // Move constructor
        array( array&& other ) noexcept
        :   _data(other._data), _size(other._size)
        {
            other._data = nullptr;
            other._size = 0;
        }

        // default constructor
        array( size_t size )
        :   _data( nullptr ), _size( 0 )
        {
            resize( size );
        }

        // default constructor
        array()
        :   _data( nullptr ), _size( 0 )
        {
            clear();
        }

        // Destructor
        ~array()
        {
            clear();
        }

    private:

        T* _data;
        size_t _size;
    };

}
#endif /* kege_array_hpp */
