//
//  vk-buffer.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/14/25.
//

#include "vk-device.hpp"
#include "vk-manager.hpp"
#include "vk-instance.hpp"
#include "vk-buffer.hpp"

namespace kege::vk{

    VkResult Buffer::create( const kege::BufferDesc& desc )
    {
        _frames = desc.frames;
        _memory_usage = desc.memory_usage;
        _buffer_usages = desc.usage;
        _size = desc.size;

        for( int frame = 0; frame < desc.frames; ++frame )
        {
            VkResult result = _device->_manager.createBuffer
            (
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | convertBufferUsageFlags( desc.usage ),
                convertMemoryPropertyFlags( desc.memory_usage ),
                desc.size, nullptr, &_sources[ frame ].handle, &_sources[ frame ].memory
            );

            if ( result != VK_SUCCESS )
            {
                clear();
                return result;
            }

            /**
             * For debug purposes, assign a name to buffer handle if a name id present.
             */
            if ( _device->_instance->isValidationEnabled() && desc.name )
            {
                _device->_manager.debugSetObjectName
                (
                    (uint64_t) _sources[ frame ].handle,
                    VK_OBJECT_TYPE_BUFFER,
                    desc.name
                );
            }

            /**
             * For buffer that are shared between the CPU & GPU. The data can be copied directly
             * to the buffer.
             */
            if ( desc.memory_usage == MemoryUsage::CpuToGpu )
            {
                copyFrom( desc.data, desc.size, 0 );
            }

            /**
             * For buffer that only exist on the GPU. You can not copy data to them directly.
             * Thus you must create a seperate shared buffer between the CPU & GPU that contains
             * the data, then use a command buffer to transfer the data to the designated buffer.
             */
            else if ( desc.memory_usage == MemoryUsage::GpuOnly )
            {
                /**
                 * create a seperate shared buffer between the CPU & GPU that contains the data.
                 */
                vk::Buffer::Source source;
                VkResult result = _device->_manager.createBuffer
                (
                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                    desc.size, desc.data, &source.handle, &source.memory
                );
                if ( result != VK_SUCCESS )
                {
                    _device->_manager.destroyBuffer( source.handle );
                    _device->_manager.destroyMemory( source.memory );
                    clear();
                    return result;
                }

                /**
                 * transfer the data to the designated buffer.
                 */
                VkBufferCopy copy_region{};
                copy_region.srcOffset = 0; // Optional
                copy_region.dstOffset = 0; // Optional
                copy_region.size = desc.size;
                VkCommandBuffer command_buffer = _device->_queue_manager->beginTransferQueueCommandBuffer();
                vkCmdCopyBuffer( command_buffer, source.handle, _sources[ frame ].handle, 1, &copy_region );
                _device->_queue_manager->endTransferQueueCommandBuffer( command_buffer );
                _device->_manager.destroyBuffer( source.handle );
                _device->_manager.destroyMemory( source.memory );
            }
        }
        return VK_SUCCESS;
    }


    void Buffer::copyFromBuffer
    (
        uint64_t offset,
        uint64_t size,
        const ref::Buffer& src,
        uint64_t src_offset
    )
    {
        const vk::Buffer* other = src->vk();
        int frame = getFrameIndex();

        VkBufferCopy copy_region{};
        copy_region.srcOffset = src_offset;
        copy_region.dstOffset = offset;
        copy_region.size = size;

        VkCommandBuffer command_buffer = _device->_queue_manager->beginTransferQueueCommandBuffer();
        vkCmdCopyBuffer( command_buffer, _sources[ frame ].handle, other->getSource( frame ).handle, 1, &copy_region );
        _device->_queue_manager->endTransferQueueCommandBuffer( command_buffer );
    }

    void Buffer::copyToBuffer
    (
        uint64_t offset,
        uint64_t size,
        ref::Buffer& dst,
        uint64_t dst_offset
    )
    {
        const vk::Buffer* other = dst->vk();
        int frame = getFrameIndex();

        VkBufferCopy copy_region{};
        copy_region.srcOffset = offset;
        copy_region.dstOffset = dst_offset;
        copy_region.size = size;

        VkCommandBuffer command_buffer = _device->_queue_manager->beginTransferQueueCommandBuffer();
        vkCmdCopyBuffer( command_buffer, other->getSource( frame ).handle, _sources[ frame ].handle, 1, &copy_region );
        _device->_queue_manager->endTransferQueueCommandBuffer( command_buffer );

    }

    // Copy data to/from buffer
    void Buffer::copyFrom(const void* data, uint64_t size, uint64_t offset)
    {
        if ( _memory_usage == kege::MemoryUsage::CpuToGpu )
        {
            if ( data != nullptr && size != 0 && _frames )
            {
                int frame = _device->getFrameIndex() % _frames;
                vkMapMemory( _device->handle(), _sources[ frame ].memory, 0, size, 0, &_sources[ frame ].mapped_ptr );
                size = min( size, _size );
                memcpy( _sources[ frame ].mapped_ptr, data, size );
                vkUnmapMemory( _device->handle(), _sources[ frame ].memory );
                _sources[ frame ].mapped_ptr = nullptr;
            }
        }
        else
        {
            kege::Log::error << "can not map buffer, MemoryUsage must be set to CpuToGpu.";
        }
    }
    
    void Buffer::copyTo(void* data, uint64_t size, uint64_t offset)const
    {
        if ( data != nullptr && size != 0 && _frames )
        {
            void* mapped_ptr = nullptr;
            int frame = getFrameIndex();
            vkMapMemory( _device->handle(), _sources[ frame ].memory, 0, size, 0, &mapped_ptr );
            memcpy( data, mapped_ptr, size );
            vkUnmapMemory( _device->handle(), _sources[ frame ].memory );
        }
    }

    // Map buffer memory for CPU access (if cpuAccessible is true)
    const void* Buffer::map(uint64_t offset, uint64_t size)const
    {
        int frame = getFrameIndex();
        const Buffer::Source& source = _sources[ frame ];
        void* mapped_ptr = nullptr;

        // Only map if memory allows it and not already mapped
        if( source.mapped_ptr == nullptr && _memory_usage == MemoryUsage::GpuToCpu )
        {
            if ( size == 0 )
            {
                size = _size - offset;
            }

            VkResult result = vkMapMemory( _device->handle(), source.memory, offset, size, 0, &mapped_ptr );
            if (result == VK_SUCCESS)
            {
                // Return pointer adjusted by offset
                return static_cast< uint8_t* >( mapped_ptr ) + offset;
            }
            else
            {
                switch ( result )
                {
                    case VK_ERROR_OUT_OF_HOST_MEMORY:
                        kege::Log::error << "VK_ERROR_OUT_OF_HOST_MEMORY in mapBuffer()"<<Log::nl;
                        break;
                    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                        kege::Log::error << "VK_ERROR_OUT_OF_DEVICE_MEMORY in mapBuffer()"<<Log::nl;
                        break;
                    case VK_ERROR_MEMORY_MAP_FAILED:
                        kege::Log::error << "VK_ERROR_MEMORY_MAP_FAILED in mapBuffer()"<<Log::nl;
                        break;
                    default:
                        kege::Log::error << "unablde to map device memory in mapBuffer()"<<Log::nl;
                        break;
                }
                return nullptr;
            }
        }
        else if ( source.mapped_ptr != nullptr )
        {
            // Already mapped, return adjusted pointer
            return static_cast< uint8_t* >( source.mapped_ptr ) + offset;
        }
        return nullptr; // Handle not found
    }

    void* Buffer::map(uint64_t offset, uint64_t size)
    {
        int frame = getFrameIndex();
        Buffer::Source& source = _sources[ frame ];
        // Only map if memory allows it and not already mapped
        if( source.mapped_ptr == nullptr && _memory_usage == MemoryUsage::CpuToGpu )
        {
            if ( size == 0 )
            {
                size = _size - offset;
            }

            VkResult result = vkMapMemory( _device->handle(), source.memory, offset, size, 0, &source.mapped_ptr );
            if (result == VK_SUCCESS)
            {
                // Return pointer adjusted by offset
                return static_cast< uint8_t* >( source.mapped_ptr ) + offset;
            }
            else
            {
                switch ( result )
                {
                    case VK_ERROR_OUT_OF_HOST_MEMORY:
                        kege::Log::error << "VK_ERROR_OUT_OF_HOST_MEMORY in mapBuffer()"<<Log::nl;
                        break;
                    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                        kege::Log::error << "VK_ERROR_OUT_OF_DEVICE_MEMORY in mapBuffer()"<<Log::nl;
                        break;
                    case VK_ERROR_MEMORY_MAP_FAILED:
                        kege::Log::error << "VK_ERROR_MEMORY_MAP_FAILED in mapBuffer()"<<Log::nl;
                        break;
                    default:
                        kege::Log::error << "unablde to map device memory in mapBuffer()"<<Log::nl;
                        break;
                }
                return nullptr;
            }
        }
        else if ( source.mapped_ptr != nullptr )
        {
            // Already mapped, return adjusted pointer
            return static_cast< uint8_t* >( source.mapped_ptr ) + offset;
        }
        return nullptr; // Handle not found
    }

    void Buffer::unmap()
    {
        int frame = getFrameIndex();
        vkUnmapMemory( _device->handle(), _sources[ frame ].memory );
    }

    bool Buffer::resize( uint64_t size )
    {
        for( int frame = 0; frame < _frames; ++frame )
        {
           vk::Buffer::Source source;
           VkResult result = _device->_manager.createBuffer
           (
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | convertBufferUsageFlags( _buffer_usages ),
                convertMemoryPropertyFlags( _memory_usage ),
                size, nullptr, &source.handle, &source.memory
           );

           if ( result != VK_SUCCESS )
           {
               clear();
               return false;
           }

            _device->copyBuffer( source, _sources[ frame ], (_size < size ) ? _size : size );
            _device->_manager.destroyBuffer( _sources[ frame ].handle );
            _device->_manager.destroyMemory( _sources[ frame ].memory );
            _sources[ frame ] = source;

        }
        _size = size;
        return true;
    }

    int Buffer::getFrameIndex()const
    {
        return _device->getFrameIndex() % _frames;
    }

    const void* Buffer::data()const
    {
        int frame = getFrameIndex();
        return _sources[ frame ].memory;
    }

    void* Buffer::data()
    {
        int frame = getFrameIndex();
        return _sources[ frame ].memory;
    }

    bool Buffer::empty()const
    {
        return _frames ? true : _size == 0;
    }

    void Buffer::clear()
    {
        if ( _device )
        {
            _device->destroyBuffer( this );
            _frames = 0;
            _size = 0;
            _device = nullptr;
        }
    }

    int Buffer::frames()const
    {
        return _frames;
    }

    Buffer::Buffer( vk::Device* device, const kege::BufferDesc& desc )
    :   _device( device )
    {
        create( desc );
    }

    Buffer::~Buffer()
    {
        clear();
    }

}
