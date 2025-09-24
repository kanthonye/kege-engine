//
//  shader-resource-manager.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/22/25.
//

#ifndef shader_resource_manager_hpp
#define shader_resource_manager_hpp

#include "shader-resource.hpp"

namespace kege{

    class Graphics;

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
         * @brief Internal structure representing a shader resource object.
         *
         * This structure encapsulates the details of a shader resource, including its
         * associated descriptor sets for each frame in flight, as well as linked list
         * pointers for managing allocation and retention.
         */
        struct UniformSetsObject
        {
            UniformSets resource_sets;

            /**
             * @brief Previous index in linked list (-1 for none)
             */
            std::vector< int32_t > uniform_set_handles;

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

        bool update( const std::vector< int32_t >& set_indices, const ShaderResource& resources );

        ShaderResource create( const UniformSetsDesc& desc );
        ShaderResource create( const UniformSetDesc& desc );
        void free( const ShaderResource& resource );

        /**
         * @brief Initializes the shader resource manager.
         * @param device Pointer to the graphics system for resource operations.
         */
        void initalize( kege::GraphicsDevice* device );

        /**
         * @brief Shuts down the shader resource manager, freeing all resources.
         */
        void shutdown();

        ~ShaderResourceManager();
        ShaderResourceManager();

    private:

        /**
         * @brief Increments the reference count for a shader resource.
         * @param handler Pointer to the shader resource whose reference count to increment.
         */ 
        void incrementReference( ShaderResource* handler );

        /**
         * @brief Generate a new ShaderResrcSetBindings.
         */
        int generate();

    private:

        // Storage for all shader resource set layouts.
        std::vector< UniformSetsObject > _shader_resources;

        // Pointer to the graphics system for resource operations.
        kege::GraphicsDevice* _device;

        int _shader_resource_counter;

        /**
         * @brief Previous index in linked list (-1 for none)
         */
        int32_t _head;

        /**
         * @brief Next index in linked list (-1 for none)
         */
        int32_t _tail;
        
        friend ShaderResource;
    };

}
#endif /* shader_resource_manager_hpp */
