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

    VkResult Buffer::create( vk::Device* device, const kege::BufferDesc& desc )
    {
        _device = device;
        _memory_usage = desc.memory_usage;
        _buffer_usages = desc.usage;
        _size = desc.size;

        VkResult result = _device->_manager.createBuffer
        (
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | convertBufferUsageFlags( desc.usage ),
            convertMemoryPropertyFlags( desc.memory_usage ),
            desc.size, nullptr, &_handle, &_memory
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
                (uint64_t) _handle,
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
            VkResult result = _device->_manager.createBuffer
            (
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                desc.size, desc.data, &_handle, &_memory
            );
            if ( result != VK_SUCCESS )
            {
                _device->_manager.destroyBuffer( _handle );
                _device->_manager.destroyMemory( _memory );
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
            vkCmdCopyBuffer( command_buffer, _handle, _handle, 1, &copy_region );
            _device->_queue_manager->endTransferQueueCommandBuffer( command_buffer );
            _device->_manager.destroyBuffer( _handle );
            _device->_manager.destroyMemory( _memory );
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

        VkBufferCopy copy_region{};
        copy_region.srcOffset = src_offset;
        copy_region.dstOffset = offset;
        copy_region.size = size;

        VkCommandBuffer command_buffer = _device->_queue_manager->beginTransferQueueCommandBuffer();
        vkCmdCopyBuffer( command_buffer, _handle, other->_handle, 1, &copy_region );
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

        VkBufferCopy copy_region{};
        copy_region.srcOffset = offset;
        copy_region.dstOffset = dst_offset;
        copy_region.size = size;

        VkCommandBuffer command_buffer = _device->_queue_manager->beginTransferQueueCommandBuffer();
        vkCmdCopyBuffer( command_buffer, other->_handle, _handle, 1, &copy_region );
        _device->_queue_manager->endTransferQueueCommandBuffer( command_buffer );

    }

    // Copy data to/from buffer
    void Buffer::copyFrom(const void* data, uint64_t size, uint64_t offset)
    {
        if ( _memory_usage == kege::MemoryUsage::CpuToGpu )
        {
            if ( data != nullptr && size != 0 )
            {
                vkMapMemory( _device->handle(), _memory, 0, size, 0, &_mapped_ptr );
                size = min( size, _size );
                memcpy( _mapped_ptr, data, size );
                vkUnmapMemory( _device->handle(), _memory );
                _mapped_ptr = nullptr;
            }
        }
        else
        {
            kege::Log::error << "can not map buffer, MemoryUsage must be set to CpuToGpu.";
        }
    }
    
    void Buffer::copyTo(void* data, uint64_t size, uint64_t offset)const
    {
        if ( data != nullptr && size != 0 )
        {
            void* mapped_ptr = nullptr;
            vkMapMemory( _device->handle(), _memory, 0, size, 0, &mapped_ptr );
            memcpy( data, mapped_ptr, size );
            vkUnmapMemory( _device->handle(), _memory );
        }
    }

    // Map buffer memory for CPU access (if cpuAccessible is true)
    const void* Buffer::map(uint64_t offset, uint64_t size)const
    {
        void* mapped_ptr = nullptr;

        // Only map if memory allows it and not already mapped
        if( _mapped_ptr == nullptr && _memory_usage == MemoryUsage::GpuToCpu )
        {
            if ( size == 0 )
            {
                size = _size - offset;
            }

            VkResult result = vkMapMemory( _device->handle(), _memory, offset, size, 0, &mapped_ptr );
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
        else if ( _mapped_ptr != nullptr )
        {
            // Already mapped, return adjusted pointer
            return static_cast< uint8_t* >( _mapped_ptr ) + offset;
        }
        return nullptr; // Handle not found
    }

    void* Buffer::map(uint64_t offset, uint64_t size)
    {
        // Only map if memory allows it and not already mapped
        if( _mapped_ptr == nullptr && _memory_usage == MemoryUsage::CpuToGpu )
        {
            if ( size == 0 )
            {
                size = _size - offset;
            }

            VkResult result = vkMapMemory( _device->handle(), _memory, offset, size, 0, &_mapped_ptr );
            if (result == VK_SUCCESS)
            {
                // Return pointer adjusted by offset
                return static_cast< uint8_t* >( _mapped_ptr ) + offset;
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
        else if ( _mapped_ptr != nullptr )
        {
            // Already mapped, return adjusted pointer
            return static_cast< uint8_t* >( _mapped_ptr ) + offset;
        }
        return nullptr; // Handle not found
    }

    void Buffer::unmap()
    {
        vkUnmapMemory( _device->handle(), _memory );
        _mapped_ptr = nullptr;
    }

    bool Buffer::resize( uint64_t size )
    {
        BufferDesc desc;
        desc.size = size;
        desc.data = nullptr;
        desc.usage = BufferUsages::None;
        desc.memory_usage = MemoryUsage::GpuOnly;

        vk::Buffer source(_device, desc);

//        VkResult result = _device->_manager.createBuffer
//        (
//            VK_BUFFER_USAGE_TRANSFER_DST_BIT | convertBufferUsageFlags( _buffer_usages ),
//            convertMemoryPropertyFlags( _memory_usage ),
//            size, nullptr, &_handle, &_memory
//        );
//        if ( result != VK_SUCCESS )
//        {
//            clear();
//            return false;
//        }

        _device->copyBuffer( source, *this, (_size < size ) ? _size : size );
        _device->_manager.destroyBuffer( _handle );
        _device->_manager.destroyMemory( _memory );
        _handle = source._handle;
        _memory = source._memory;

        _size = size;
        return true;
    }

    const void* Buffer::data()const
    {
        return _mapped_ptr;
    }

    void* Buffer::data()
    {
        return _mapped_ptr;
    }

    bool Buffer::empty()const
    {
        return _size == 0;
    }

    void Buffer::clear()
    {
        if ( _device )
        {
            _device->destroyBuffer( this );
            _size = 0;
            _device = nullptr;
        }
    }

    const VkBuffer& Buffer::getHandle() const
    {
        return _handle;
    }
    const VkDeviceMemory& Buffer::getMemoryHandle() const
    {
        return _memory;
    }
    Buffer::Buffer( vk::Device* device, const kege::BufferDesc& desc )
    :   _device( device )
    ,   _handle( VK_NULL_HANDLE )
    ,   _memory( VK_NULL_HANDLE )
    ,   _mapped_ptr( nullptr )
    {
        create( device, desc );
    }

    Buffer::Buffer()
    :   _device( nullptr )
    ,   _handle( VK_NULL_HANDLE )
    ,   _memory( VK_NULL_HANDLE )
    ,   _mapped_ptr( nullptr )
    {
    }

    Buffer::~Buffer()
    {
        clear();
    }

}
