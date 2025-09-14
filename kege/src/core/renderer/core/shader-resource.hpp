//
//  shader-resource.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/9/25.
//

#ifndef shader_resource_hpp
#define shader_resource_hpp

#include "graphics-core.hpp"
#include "uniform-resource.hpp"

namespace kege{

    class ShaderResourceManager;

    /**
     * @brief Handle class for accessing shader resources.
     *
     * Provides controlled access to shader resource bindings through the resource manager.
     * Acts as a smart handle that can be used to access the underlying resource data.
     */
    class ShaderResource // ShaderResourceLayout
    {
    public:

        /**
         * @brief Internal structure representing a shader resource object.
         *
         * This structure encapsulates the details of a shader resource, including its
         * associated descriptor sets for each frame in flight, as well as linked list
         * pointers for managing allocation and retention.
         */
        struct Node
        {
            std::vector< kege::Uniform > bindings;

            // Handle to the descriptor set.
            int32_t handle;

            /**
             * @brief Previous index in linked list (-1 for none)
             */
            int32_t prev = -1;

            /**
             * @brief Next index in linked list (-1 for none)
             */
            int32_t next = -1;

            int32_t duplicates = 0;
        };

    public:

        const kege::Uniform& operator[](int i) const;
        kege::Uniform& operator[](int i);
        int32_t operator*() const;

        const UniformLayouts& set(int i)const;

        /**
         * @brief Boolean conversion operator.
         * @return True if the resource is valid, false otherwise.
         */
        operator bool() const;

        /**
         * @brief Move assignment operator
         */
        ShaderResource& operator =( ShaderResource&& other) noexcept;

        /**
         * @brief Copy assignment operator
         */
        ShaderResource& operator =( const ShaderResource& other );

        /**
         * @brief Copy constructor
         */
        ShaderResource( const ShaderResource& m );

        /**
         * @brief Move constructor
         */
        ShaderResource( ShaderResource&& other )noexcept;

        /**
         * @brief Default constructor creates an invalid/null resource.
         */
        ShaderResource();

        /**
         * @brief deconstructor
         */
        ~ShaderResource();

    private:

        /**
         * @brief Constructs a resource handle tied to a specific manager and index.
         * @param mngr The resource manager that owns this resource.
         * @param index The index of the resource in the manager.
         */
        ShaderResource( ShaderResourceManager* mngr, uint32_t index );

    private:

        friend ShaderResourceManager;

        /** @brief Pointer to the resource manager that owns this resource. */
        ShaderResourceManager* _mngr;

        /** @brief Index of the resource within the manager. */
        uint32_t _index;
    };


    struct MakeShaderResources
    {
        std::vector< UniformBindingElements > binding_elements;
        UniformLayoutDescription descriptors;
        ShaderResource * resources;
        int quantity;
    };



    struct CreateBufferInfo
    {
        BufferDesc desc;
        size_t offset;
        size_t range;
    };
    struct CreateBufferBindingPair
    {
        std::vector< CreateBufferInfo > create_infos;
        int binding;
    };
    typedef std::vector< CreateBufferBindingPair > CreateBufferBindingSet;

    struct CreateImageInfo
    {
        ImageDesc desc;
        SamplerHandle sampler;
    };
    struct CreateImageBindingPair
    {
        std::vector< CreateImageInfo > create_infos;
        int binding;
    };
    typedef std::vector< CreateImageBindingPair > CreateImageBindingSet;


    struct CreateShaderResources
    {
        CreateBufferBindingSet binding_buffer_sets;
        CreateImageBindingSet binding_image_sets;
        UniformLayoutDescription descriptors;
        ShaderResource * resources;
        int quantity;
    };

//    inline bool operator==(const kege::ShaderResource& a, const kege::ShaderResource& b){ return a.id == b.id; }
//    inline bool operator!=(const kege::ShaderResource& a, const kege::ShaderResource& b){ return a.id != b.id; }
//    inline bool operator <(const kege::ShaderResource& a, const kege::ShaderResource& b){ return a.id  < b.id; }

}

#endif /* shader_resource_binding_hpp */
