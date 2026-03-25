//
//  image.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/22/25.
//

#ifndef kege_image_hpp
#define kege_image_hpp

#include "../pipeline/depth-state.hpp"
#include "graphics-core.hpp"

namespace kege{

    struct ClearRect
    {
        Rect2D      rect;
        uint32_t    base_array_layer;
        uint32_t    layer_count;
    };

    enum struct ImageAspectFlag
    {
        None = 0x00000000, // No aspect selected (invalid in most operations)
        Color   = 0x00000001,
        Depth   = 0x00000002,
        Stencil = 0x00000004,
        DepthStencil = Depth | Stencil
    };

    struct ClearAttachment
    {
        ImageAspectFlag aspect_mask;
        uint32_t        attachment_index;
        ClearValue      clear_value;
    };

    /**
     * @brief Structure describing a region for texture copy operations.
     *
     * Defines the subresource layers and 3D offsets for both the source and
     * destination textures, along with the 3D extent of the region to be copied.
     */
    struct ImageCopyRegion
    {
        SubresourceLayers src_subresource; // Subresource layers for the source texture.
        SubresourceLayers dst_subresource; // Subresource layers for the destination texture.

        Offset3D src_offset; // 3D offset for the source texture.
        Offset3D dst_offset; // 3D offset for the destination texture.

        Extent3D extent; // 3D extent of the copy region.
    };

    /**
     * @brief Structure defining a subresource range for texture operations (e.g., clearing).
     *
     * This structure specifies the mip levels and array layers affected by an operation.
     * It's used in functions like `clearColorTexture` and `clearDepthStencilTexture`.
     */
    struct ImageSubresourceRange
    {
        ImageAspectFlag aspect_mask; ///< Aspect mask indicating which parts of the image are affected (e.g., color, depth, stencil).
        uint32_t base_mip_level;     ///< First mipmap level to affect.
        uint32_t level_count;        ///< Number of mipmap levels to affect.
        uint32_t base_array_layer;   ///< First array layer to affect.
        uint32_t layer_count;        ///< Number of array layers to affect.
    };

    /**
     * @brief Enumeration of image layouts.
     *
     * Describes the possible layouts an image can be in, which affects how
     * the image data is organized in memory and how it can be accessed.
     */
    enum class ImageLayout
    {
        Undefined,          ///< Initial undefined layout
        General,            ///< General-purpose layout
        Color,              ///< Optimal for color attachment access
        Depth,              ///< Optimal for depth attachment access
        DepthRead,
        Stencil,
        StencilRead,
        DepthStencil,       ///< Optimal for depth/stencil attachment access
        DepthStencilRead,   ///< Optimal for depth/stencil attachment access
        DepthRead_Stencil,
        Depth_StencilRead,
        ShaderRead,         ///< Optimal for shader read-only access
        TransferSrc,        ///< Optimal for transfer source operations
        TransferDst,        ///< Optimal for transfer destination operations
        PreInitialized,
        Present,            ///< Optimal for presentation engine access
        HostRead,           ///< Layout suitable for CPU reading
        HostWrite,          ///< Layout suitable for CPU writing
        // Add more as needed...
    };

    /**
     * @brief Describes an image memory barrier.
     *
     * Contains all the information needed to transition an image resource
     * between different states, including layout, access patterns, and
     * pipeline stages.
     */
    struct ImageMemoryBarrier
    {
        std::string resource_name;  ///< Name of the image in the RenderGraph

        /// @name Transition Metadata
        /// @{
        ImageLayout old_layout;    ///< Layout before the barrier
        ImageLayout new_layout;    ///< Layout after the barrier
        AccessFlags src_access;     ///< Access type before the barrier
        AccessFlags dst_access;     ///< Access type after the barrier
        PipelineStageFlag src_stage;   ///< Pipeline stage before the barrier
        PipelineStageFlag dst_stage;   ///< Pipeline stage after the barrier
        /// @}

        QueueType src_queue = QueueType::Graphics; ///< Source queue family
        QueueType dst_queue = QueueType::Graphics; ///< Destination queue family

        ImageSubresourceRange subresource_range; ///< Affected subresource range
        ref::Image image;
    };

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

    /**
     * @brief Enumeration of supported multisample anti-aliasing (MSAA) sample counts.
     *
     * Used for render targets and textures that support multisampling.
     */
    enum class SampleCount
    {
        Count1 = 1,    ///< No multisampling (1 sample per pixel)
        Count2 = 2,    ///< 2x MSAA
        Count4 = 4,    ///< 4x MSAA (common default)
        Count8 = 8,    ///< 8x MSAA
        Count16 = 16,  ///< 16x MSAA (high quality)
        Count32 = 32,  ///< 32x MSAA (very high quality)
        Count64 = 64   ///< 64x MSAA (extremely high quality)
    };

    
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

}
#endif /* kege_image_hpp */
