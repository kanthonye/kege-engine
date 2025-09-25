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

        bool update( const std::vector<int32_t>& set_indices = {} );

        /**
         * @brief Enables access to this shader resource handles
         * @return A collection of resource handle associated with this shader resource.
         */
        const std::vector< int >* operator ->()const;

        /**
         * @brief Enables access to each uniform set associated with this shader resource
         * @return A reference to the uniform set at the given index
         */
        const UniformSet& operator[](int i) const;

        /**
         * @brief Enables access to each uniform set associated with this shader resource
         * @return A reference to the uniform set at the given index
         */
        UniformSet& operator[](int i);

        /**
         * @brief Boolean conversion operator.
         * @return True if the resource is valid, false otherwise.
         */
        operator bool() const;

        /**
         * @brief Copy assignment operator
         */
        ShaderResource& operator =( const ShaderResource& other );

        /**
         * @brief Move assignment operator
         */
        ShaderResource& operator =( ShaderResource&& other) noexcept;

        /**
         * @brief Copy constructor
         */
        ShaderResource( const ShaderResource& other );

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
        ShaderResource( kege::ShaderResourceManager* mngr, uint32_t index );

    private:

        friend kege::ShaderResourceManager;

        /** @brief Pointer to the resource manager that owns this resource. */
        kege::ShaderResourceManager* _mngr;

        /** @brief Index of the resource within the manager. */
        uint32_t _index;
    };

}


namespace kege{

    class ShaderResrc : public RefCounter
    {
    public:

        const UniformResourceSet& operator[]( int set_index )const;
        UniformResourceSet& operator[]( int set_index );

        const ShaderBindings& getShaderBindings()const;

        void update( const std::vector< int >& sets = {} );
        void updateSet( int set_index );
        void release();

        ShaderResrc( Graphics* graphics, const UniformDescriptorSets& descriptors );
        ShaderResrc( const UniformResourceLayout& layout );
        ShaderResrc();

        ~ShaderResrc();

    private:

        UniformResourceLayout layout;
        ShaderBindings bindings;
    };

}
#endif /* shader_resource_binding_hpp */
