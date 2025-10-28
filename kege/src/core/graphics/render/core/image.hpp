//
//  image.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/22/25.
//

#ifndef kege_image_hpp
#define kege_image_hpp

#include "graphics-core.hpp"

namespace kege{

    /**
     * @brief Enumeration of texture dimensionalities and types.
     *
     * Defines the fundamental structure of texture resources.
     */
    enum class ImageType
    {
        Type1D,        ///< 1-dimensional texture
        Type1DArray,    ///< Array of 1D textures
        Type2D,         ///< 2-dimensional texture
        Type2DArray,    ///< Array of 2D textures
        TypeCube,       ///< Cube map texture (6 faces)
        TypeCubeArray,  ///< Array of cube map textures
        Type3D          ///< 3-dimensional (volumetric) texture
    };

    enum struct ImageUsage: uint32_t
    {
        Undefined       = 0,
        TransferSrc     = 1 << 1,
        TransferDst     = 1 << 2,
        Sampled         = 1 << 3,
        Storage         = 1 << 4,
        Color           = 1 << 5,
        DepthStencil    = 1 << 6,
        Transient       = 1 << 7,
        Input           = 1 << 8,
        HostTransfer    = 1 << 9,
        Present         = 1 << 10,
    };

    inline constexpr ImageUsage operator | (const ImageUsage& a, const ImageUsage& b)
    {
        return static_cast< ImageUsage >(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }
    inline constexpr ImageUsage operator&(const ImageUsage& a, const ImageUsage& b)
    {
        return static_cast< ImageUsage >(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }
    inline constexpr ImageUsage& operator |=( ImageUsage a, const ImageUsage& b)
    {
        return a = a | b;
    }
    inline constexpr ImageUsage& operator &=( ImageUsage a, const ImageUsage& b)
    {
        return a = a & b;
    }
    inline constexpr bool checkFlag( ImageUsage a, const ImageUsage& b)
    {
        return (static_cast< uint32_t >( a ) & static_cast< uint32_t >( b )) != 0;
    }

//    /**
//     * @brief Describes the properties and configuration of a texture resource.
//     */
//    struct Image::Desc
//    {
//        ImageType type = ImageType::Type2D;             ///< Dimensionality and type of texture
//        uint32_t width = 1;                                 ///< Base width in texels
//        uint32_t height = 1;                                ///< Base height in texels
//        uint32_t depth = 1;                                 ///< Depth for 3D textures, array layers for others
//        uint32_t mip_levels = 1;                            ///< Number of mipmap levels
//        uint32_t layers = 1;
//        Format format = Format::undefined;                  ///< Pixel format and data type
//        SampleCount sample_count = SampleCount::Count1;     ///< MSAA sample count
//        ImageUsage usage = ImageUsage::Undefined;  ///< Allowed usages
//        MemoryUsage memory_usage = MemoryUsage::GpuOnly;    ///< Memory placement strategy
//        std::string name = "";                        ///< Debug label (visible in tools like RenderDoc)
//
//        const void* data =  nullptr;
//
//    };

//
//    /**
//     * @brief Commonly used texture usage combinations
//     */
//    namespace TextureUsage
//    {
//        constexpr ImageUsage Default =
//            ImageUsage::Sampled |
//            ImageUsage::CopySrc |
//            ImageUsage::CopyDst;
//
//        constexpr ImageUsage RenderTarget =
//            ImageUsage::Color |
//            ImageUsage::Sampled |
//            ImageUsage::CopySrc;
//
//        constexpr ImageUsage DepthStencil =
//            ImageUsage::DepthStencil |
//            ImageUsage::Sampled;
//
//        constexpr ImageUsage Storage =
//            ImageUsage::Storage |
//            ImageUsage::Sampled |
//            ImageUsage::CopySrc |
//            ImageUsage::CopyDst;
//    }
}


namespace kege{

    // Abstract Image class
    class Image : public kege::RefCounter {
    public:

        struct Desc
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
            std::string debug_name;
            //std::optional< ClearValue > optimizedClearValue;
        };

    public:


        void operator()( const kege::Image::Desc& desc )
        {
            _extent = desc.extent;
            _format = desc.format;
            _array_layers = desc.array_layers;
            _mip_levels = desc.mip_levels;
            _layout = desc.initial_layout;
            _image_usages = desc.usage;
            _memory_usage = desc.memory_usage;
            _samples = desc.samples;
            _type = desc.type;
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
        Extent3D getExtent() const { return _extent; }
        ImageType getType() const { return _type; }
        Format getFormat() const { return _format; }

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

        ImageUsage getImageUsage() const { return _image_usages; }
        MemoryUsage getMemoryUsage() const { return _memory_usage; }
        uint64_t size() const { return _size; };

        ImageLayout getLayout()const{ return _layout; }
        ImageUsage getImageUsages()const{ return _image_usages; }
        uint64_t getSize()const{ return _size; }

        uint32_t getMipLevels()const{ return _mip_levels; }
        uint32_t getArrayLayers()const{ return _array_layers; }
        SampleCount getSampleCount()const{ return _samples; }


        void setLayout( ImageLayout& layout ){ _layout = layout; }

        virtual const vk::Image* vk() const { return nullptr; }
        virtual vk::Image* vk() { return nullptr; }

        virtual ~Image() = default;

    protected:

        Image( const kege::Image::Desc& desc )
        :   _extent( desc.extent )
        ,   _format( desc.format )
        ,   _array_layers( desc.array_layers )
        ,   _mip_levels( desc.mip_levels )
        ,   _layout( desc.initial_layout )
        ,   _image_usages( desc.usage )
        ,   _memory_usage( desc.memory_usage )
        ,   _type( desc.type )
        ,   _samples( desc.samples )
        ,   _size( 0 )
        {}

    protected:

        ImageLayout _layout;
        MemoryUsage _memory_usage;
        ImageUsage _image_usages;
        uint64_t _size;

        ImageType _type;
        Format _format;
        Extent3D _extent;
        uint32_t _mip_levels;
        uint32_t _array_layers;
        SampleCount _samples;

        friend kege::ref::Image;
    };



    class Sampler : public kege::RefCounter
    {
    public:

        virtual const vk::Sampler* vk() const { return nullptr; }
        virtual vk::Sampler* vk() { return nullptr; }
        virtual ~Sampler() = default;

    protected:

        Sampler() = default;
    };

}
#endif /* kege_image_hpp */
