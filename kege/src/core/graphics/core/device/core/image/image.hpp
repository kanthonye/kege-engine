//
//  image.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/22/25.
//

#ifndef kege_image_hpp
#define kege_image_hpp

#include "../common/graphics-common.h"

namespace kege
{
    class Image;
}

namespace kege::vk
{
    class Image;
}

namespace kege::ref
{
    typedef kege::Ref< kege::Image > Image;
}

namespace kege::cref
{
    typedef kege::Ref< const kege::Image > Image;
}

namespace kege{

    struct ImageDesc
    {
        // Core properties
        ImageType type = ImageType::Type2D;
        Format format = Format::rgba_u8_norm;
        Extent3D extent;
        uint32_t mip_levels = 1;
        uint32_t array_layers = 1;
        SampleCount samples = SampleCount::Count1;

        // Usage and properties
        ImageUsage usage = ImageUsage::Color | ImageUsage::Sampled | ImageUsage::TransferSrc;

        //ImageCreateFlags flags;
        MemoryUsage memory_usage = MemoryUsage::GpuOnly;

        // Initial layout and data
        ImageLayout initial_layout = ImageLayout::Undefined;
        //std::optional< ImageData > initial_data;

        const void* data =  nullptr;

        // Advanced features
        std::string name;
        //std::optional< ClearValue > optimizedClearValue;
    };

    // Abstract Image class
    class Image : public kege::RefCounter {
    public:


        void operator()( const kege::ImageDesc& desc )
        {
            _desc = desc;
        }
//        struct ViewCreateInfo
//        {
//            ImageViewType viewType;
//            Format format;
//            ComponentMapping components;
//            ImageSubresourceRange subresourceRange;
//            ImageViewUsageFlags usage;
//        };


        // Core functionality
//        virtual void transitionLayout(CommandBuffer* cmd, ImageLayout oldLayout,
//                                     ImageLayout newLayout,
//                                     const ImageSubresourceRange& range = {}) = 0;

        uint64_t getMemorySize() const { return _size; }
        Extent3D getExtent() const { return _desc.extent; }
        ImageType getType() const { return _desc.type; }
        Format getFormat() const { return _desc.format; }

//        // Copy between buffers
//        virtual void copyFromBuffer
//        (
//            uint64_t offset,
//            uint64_t size,
//            const ref::Buffer& src,
//            uint64_t src_offset
//        )= 0;
//
//        virtual void copyToBuffer
//        (
//            uint64_t offset,
//            uint64_t size,
//            ref::Buffer& dst,
//            uint64_t dst_offset
//        )= 0;
//
//        // Copy data to/from buffer
//        virtual void copyFrom(const void* data, uint64_t size, uint64_t offset = 0) = 0;
//        virtual void copyTo(void* data, uint64_t size, uint64_t offset = 0)const = 0;
//
//        // Map buffer memory for CPU access (if cpuAccessible is true)
//        virtual const void* map(uint64_t offset = 0, uint64_t size = 0)const = 0;
//        virtual void* map(uint64_t offset = 0, uint64_t size = 0) = 0;
//        virtual void unmap() = 0;
//
//        virtual const void* data()const;
//        virtual void* data();
//
//        virtual bool resize( uint64_t size ) = 0;
//        virtual bool empty()const = 0;
//        virtual void clear() = 0;
//
//        virtual int frames()const = 0;
        // Get buffer properties

        ImageUsage getImageUsage() const { return _desc.usage; }
        MemoryUsage getMemoryUsage() const { return _desc.memory_usage; }
        uint64_t size() const { return _size; };

        ImageLayout getLayout()const{ return _desc.initial_layout; }
        uint64_t getSize()const{ return _size; }

        uint32_t getMipLevels()const{ return _desc.mip_levels; }
        uint32_t getArrayLayers()const{ return _desc.array_layers; }
        SampleCount getSampleCount()const{ return _desc.samples; }


        void setLayout( ImageLayout& layout ){ _desc.initial_layout = layout; }

        virtual const vk::Image* vk() const { return nullptr; }
        virtual vk::Image* vk() { return nullptr; }

        virtual ~Image() = default;

    protected:

        Image( const kege::ImageDesc& desc )
        :   _desc( desc )
        ,   _size( 0 )
        {}

    protected:

        ImageDesc _desc;
        uint64_t _size;

        friend kege::ref::Image;
    };

}
#endif /* kege_image_hpp */
