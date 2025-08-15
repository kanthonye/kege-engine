//
//  shader-resource-manager.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/22/25.
//

#ifndef shader_resource_manager_hpp
#define shader_resource_manager_hpp

#include "../../graphics/core/graphics-core.hpp"

namespace kege{

    class Graphics;
    class ShaderResourceManager;

    /**
     * @brief Represents a single shader resource binding (either buffer or image).
     *
     * This union-based structure can hold either a buffer or image handle along with
     * type information, and provides conversion operators for easy assignment.
     */
    struct ShaderResrcBinding
    {
        enum Type { INVALID, BUFFER, IMAGE, TEXEL_BUFFER } type;

        union
        {
            std::vector< kege::BufferInfo > buffer_info;
            std::vector< kege::ImageInfo > image_info;
            std::vector< kege::BufferViewHandle > texel_buffer_info;
        };

        kege::DescriptorType descriptor_type;
        uint32_t binding;

        inline friend bool operator!=( const ShaderResrcBinding& info, kege::ImageHandle handle );
        inline friend bool operator!=( kege::ImageHandle handle, const ShaderResrcBinding& info );
        inline friend bool operator==( const ShaderResrcBinding& info, kege::ImageHandle handle );
        inline friend bool operator==( kege::ImageHandle handle, const ShaderResrcBinding& info );
        inline friend bool operator!=( const ShaderResrcBinding& info, kege::BufferHandle handle );
        inline friend bool operator!=( kege::BufferHandle handle, const ShaderResrcBinding& info );
        inline friend bool operator==( const ShaderResrcBinding& info, kege::BufferHandle handle );
        inline friend bool operator==( kege::BufferHandle handle, const ShaderResrcBinding& info );

        uint32_t count()const
        {
            switch ( type )
            {
                case BUFFER: return static_cast< uint32_t >( buffer_info.size() );
                case IMAGE: return static_cast< uint32_t >( image_info.size() );
                case TEXEL_BUFFER: return static_cast< uint32_t >( texel_buffer_info.size() );
                case INVALID: break;
            }
            return 0;
        }


        ShaderResrcBinding& operator =( const ShaderResrcBinding& info )
        {
            type = info.type;
            switch ( info.type )
            {
                case BUFFER: buffer_info = info.buffer_info; break;
                case IMAGE: image_info = info.image_info; break;
                case TEXEL_BUFFER: texel_buffer_info = info.texel_buffer_info; break;
                case INVALID: break;
            }
            return *this;
        }

        ShaderResrcBinding( const std::vector< kege::BufferViewHandle >& info )
        :   texel_buffer_info( info )
        ,   type( TEXEL_BUFFER )
        {}

        ShaderResrcBinding( const std::vector< kege::BufferInfo >& info )
        :   buffer_info( info )
        ,   type( BUFFER )
        {}

        ShaderResrcBinding( const std::vector< kege::ImageInfo >& info )
        :   image_info( info )
        ,   type( IMAGE )
        {}

        ShaderResrcBinding( const ShaderResrcBinding& info )
        :   type( info.type )
        {
            this->operator =( info );
        }


        ShaderResrcBinding()
        :   type( INVALID )
        {
        }

        ~ShaderResrcBinding()
        {
            switch (type)
            {
                case BUFFER: buffer_info.~vector(); break;
                case IMAGE: image_info.~vector(); break;
                case TEXEL_BUFFER: texel_buffer_info.~vector(); break;
                case INVALID: break;
            }
        }
    };

    inline bool operator!=( const kege::ShaderResrcBinding& info, kege::ImageHandle handle )
    {
        return ( info.type == kege::ShaderResrcBinding::IMAGE )
        ? info.image_info[0].image.id != handle.id
        : false;
    }
    inline bool operator!=( kege::ImageHandle handle, const kege::ShaderResrcBinding& info )
    {
        return ( info.type == kege::ShaderResrcBinding::IMAGE )
        ? info.image_info[0].image.id != handle.id
        : false;
    }
    inline bool operator==( const kege::ShaderResrcBinding& info, kege::ImageHandle handle )
    {
        return ( info.type == kege::ShaderResrcBinding::IMAGE )
        ? info.image_info[0].image.id != handle.id
        : false;
    }
    inline bool operator==( kege::ImageHandle handle, const kege::ShaderResrcBinding& info )
    {
        return ( info.type == kege::ShaderResrcBinding::IMAGE )
        ? info.image_info[0].image.id != handle.id
        : false;
    }
    inline bool operator!=( const kege::ShaderResrcBinding& info, kege::BufferHandle handle )
    {
        return ( info.type == kege::ShaderResrcBinding::BUFFER )
        ? info.buffer_info[0].buffer.id != handle.id
        : false;
    }
    inline bool operator!=( kege::BufferHandle handle, const kege::ShaderResrcBinding& info )
    {
        return ( info.type == kege::ShaderResrcBinding::BUFFER )
        ? info.buffer_info[0].buffer.id != handle.id
        : false;
    }
    inline bool operator==( const kege::ShaderResrcBinding& info, kege::BufferHandle handle )
    {
        return ( info.type == kege::ShaderResrcBinding::BUFFER )
        ? info.buffer_info[0].buffer.id != handle.id
        : false;
    }
    inline bool operator==( kege::BufferHandle handle, const kege::ShaderResrcBinding& info )
    {
        return ( info.type == kege::ShaderResrcBinding::BUFFER )
        ? info.buffer_info[0].buffer.id != handle.id
        : false;
    }

    /**
     * @brief Information structure for updating existing shader resources.
     *
     * Specifies how to update an existing shader resource binding.
     */
    struct ShaderResrcUpdateInfo
    {
        int32_t index;
        ShaderResrcBinding infos; ///< New binding value
    };




   struct ShaderResrcSet
   {
       std::vector< kege::ShaderResrcBinding > bindings;
       kege::DescriptorSetHandle set;
   };


    /**
     * @brief Represents a set of shader resource bindings with descriptor set information.
     *
     * This structure maintains a collection of bindings along with their descriptor set
     * and linking information for resource management.
     */
    struct ShaderResrcSetBindings
    {
        std::vector< ShaderResrcSet > shader_resource_sets;
        int32_t prev = -1; ///< Previous index in linked list (-1 for none)
        int32_t next = -1; ///< Next index in linked list (-1 for none)
    };

    /**
     * @brief Handle class for accessing shader resources.
     *
     * Provides controlled access to shader resource bindings through the resource manager.
     * Acts as a smart handle that can be used to access the underlying resource data.
     */
    class ShaderResrc
    {
    public:

        const ShaderResrcSet* get( int frame_index )const;
        
        /**
         * @brief Dereference operator for accessing bindings.
         * @return Pointer to the associated ShaderResrcSetBindings.
         */
        const ShaderResrcSet* operator->() const;

        /**
         * @brief Boolean conversion operator.
         * @return True if the resource is valid, false otherwise.
         */
        operator bool() const;

        /**
         * @brief Constructs a resource handle tied to a specific manager and index.
         * @param mngr The resource manager that owns this resource.
         * @param index The index of the resource in the manager.
         */
        ShaderResrc(ShaderResourceManager* mngr, uint32_t index);

        /**
         * @brief Default constructor creates an invalid/null resource.
         */
        ShaderResrc();

    private:

        friend ShaderResourceManager;
        ShaderResourceManager* _mngr; ///< Pointer to the owning resource manager
        uint32_t _index;           ///< Index of this resource in the manager
    };

    /**
     * @brief Information structure for allocating new shader resources.
     *
     * Contains all necessary information to create a new shader resource binding.
     */
    struct ShaderResrcAllocInfo
    {
        int quantity;
        std::vector< kege::DescriptorSetLayoutBinding > bindings;
    };

   typedef std::vector< ShaderResrcUpdateInfo > ShaderResrcUpdateInfos;

    /**
     * @brief Manages the allocation, retention, and binding of shader resources.
     *
     * This class provides a centralized management system for shader resources (textures, buffers, etc.),
     * handling their allocation, lifetime management, and descriptor set binding operations.
     * It maintains internal linked lists for tracking allocated and retained resources.
     */
    class ShaderResourceManager
    {
    public:
        /**
         * @brief Allocates new shader resources.
         * @param info List of allocation information for new resources.
         * @return The allocated shader resource descriptor.
         */
        ShaderResrc allocate( const ShaderResrcAllocInfo& info );

        /**
         * @brief Retains a shader resource to prevent its release.
         * @param name The name of the resource to retain.
         * @param resrc The resource to retain into this object.
         * @return True if retention was successful, false otherwise.
         */
        bool retain( const std::string& name, ShaderResrc& resrc );

        /**
         * @brief Releases a resource declaration.
         * @param name The name of the resource to undeclare.
         * @return True if the resource was successfully undeclared, false otherwise.
         */
        bool undeclear( const std::string& name );

        /**
         * @brief Updates existing shader resources.
         * @param infos List of update information for resources.
         */
        bool update( int frame_index, ShaderResrcSet* resource, const std::vector< kege::ShaderResrcUpdateInfo >& infos );
        bool update( ShaderResrc& resource, const std::vector< ShaderResrcUpdateInfos >& infos );
        ///void update( const kege::WriteDescriptorSet& writes );

        /**
         * @brief Retrieves a shader resource by name.
         * @param name The name of the resource to retrieve.
         * @return The shader resource descriptor.
         */
        ShaderResrc get( const std::string& name );

        /**
         * @brief Resets the resource manager, freeing all allocated resources.
         *
         * After reset(), all previously allocated resources become invalid.
         * The freed list becomes the new allocation list.
         */
        void reset();

//        /**
//         * @brief Binds an object to a specific index in a resource.
//         * @param resource The resource to modify.
//         * @param index The binding index within the resource.
//         * @param binding The binding object to set.
//         */
//        void setBindingObjectAt(const ShaderResrc& resource, int index, const ShaderResrcBinding& binding);

        /**
         * @brief Generate a new ShaderResrcSetBindings.
         * @return The index of the ShaderResrcSetBindings.
         */
        uint32_t generate();

        /**
         * @brief Splices linked lists for resource management.
         *
         * This is an internal operation used to manage the linked list of resources.
         *
         * @param lhead Pointer to the local head pointer.
         * @param ltail Pointer to the local tail pointer.
         * @param index The insertion index.
         * @param head Pointer to the source head pointer.
         * @param tail Pointer to the source tail pointer.
         */
        void splice( int32_t* lhead, int32_t* ltail, int32_t index, int32_t* head, int32_t* tail );

        void initalize( kege::Graphics* graphics );
        void shutdown();

        ~ShaderResourceManager();
        ShaderResourceManager();

    public:

        /// @name Internal State
        /// @{
        std::vector< ShaderResrcSetBindings > _shader_resources; ///< All resource set bindings
        std::unordered_map< std::string, uint32_t > _name_index_map;      ///< Mapping from names to indices
        kege::Graphics* _graphics;                                        ///< Associated graphics context

        /**
         * @brief Head of the list of retained ShaderResrcSetBindings.
         *
         * Retained resources are those that have been explicitly kept alive.
         */
        int32_t _retained_head;

        /**
         * @brief Tail of the list of retained ShaderResrcSetBindings.
         */
        int32_t _retained_tail;

        /**
         * @brief Head of the allocated ShaderResrcSetBindings linked list.
         *
         * This tracks all currently allocated resources.
         */
        int32_t _alloc_head;

        /**
         * @brief Tail of the allocated ShaderResrcSetBindings linked list.
         */
        int32_t _alloc_tail;

        /**
         * @brief Head of freed resources list.
         *
         * When reset() is called, _freed_head becomes _alloc_head.
         */
        int32_t _freed_head;

        /**
         * @brief Tail of freed resources list.
         *
         * When reset() is called, _freed_tail becomes _alloc_tail.
         */
        int32_t _freed_tail;
        /// @}
    };

}
#endif /* shader_resource_manager_hpp */
