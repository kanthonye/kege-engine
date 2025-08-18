//
//  core.hpp
//  kege
//
//  Created by Kenneth Esdaile on 12/6/24.
//

#ifndef framework_hpp
#define framework_hpp

#include <stddef.h>
//#include "../device/core/types.hpp"
//#include "../device/core/image.hpp"
//#include "../device/core/buffer.hpp"
//#include "../device/core/framebuffer.hpp"
//#include "../device/core/command-buffer.hpp"
//#include "../device/core/shader-resource.hpp"
//#include "../device/core/shader-pipeline.hpp"
//#include "../device/core/device.hpp"
//#include "../device/core/render-window.hpp"

#include <initializer_list>
#include <algorithm>
#include <stdexcept>

namespace kege{

    template <typename T> class array
    {
    public:

        // Constructor from initializer_list
        array( std::initializer_list<T> init )
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

        // Access operators
        T& operator[](size_t index)
        {
            if (index >= _size) throw std::out_of_range("Index out of bounds");
            return _data[index];
        }

        const T& operator[](size_t index) const
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
        T* begin() { return _data; }
        T* end() { return _data + _size; }
        const T* begin() const { return _data; }
        const T* end() const { return _data + _size; }

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

    private:

        T* _data;
        size_t _size;
    };

}
#endif /* framework_hpp */
