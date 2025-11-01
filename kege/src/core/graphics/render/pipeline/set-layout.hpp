//
//  set-layout.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/30/25.
//

#ifndef set_layout_hpp
#define set_layout_hpp

#include "shader-set.hpp"

namespace kege{

    /**
     * @brief Abstract base class for shader binding sets.
     */
    class SetLayout : public kege::RefCounter
    {
    public:

        /**
         * @brief Retrieves a field from a buffer block by name.
         * @param name The name of the buffer block.
         * @param field The name of the field within the buffer block.
         * @return Pointer to the ShaderStructField if found, nullptr otherwise.
         */
        const ShaderStructField* getField( const std::string& name, const std::string& field )const;

        /**
         * @brief Retrieves a field from a buffer block by name.
         * @param name The name of the buffer block.
         * @param field The name of the field within the buffer block.
         * @return Pointer to the ShaderStructField if found, nullptr otherwise.
         */
        ShaderStructField* getField( const std::string& name, const std::string& field );

        /**
         * @brief Retrieves a set binding point by name.
         * @param name The name of the set binding point.
         * @return Pointer to the ShaderBindingPointSet if found, nullptr otherwise.
         */
        const kege::BindingInfo* getBinding( const std::string& name )const;

        /**
         * @brief Retrieves a set binding point by name.
         * @param name The name of the set binding point.
         * @return Pointer to the ShaderBindingPointSet if found, nullptr otherwise.
         */
        kege::BindingInfo* getBinding( const std::string& name );

        /**
         * @brief Retrieves a set binding point by index.
         * @param binding_index The index of the set binding point.
         * @return Pointer to the ShaderBindingPointSet.
         */
        const kege::BindingInfo* getBinding( int binding_index )const;

        /**
         * @brief Retrieves a set binding point by index.
         * @param binding_index The index of the set binding point.
         * @return Pointer to the ShaderBindingPointSet.
         */
        kege::BindingInfo* getBinding( int binding_index );

        /**
         * @brief Retrieves the name of this object.
         * @return String name
         */
        const std::string& getName()const;

        int totalBindingCount()const;
        int totalBufferCount()const;
        int totalImageCount()const;

        /**
         * @brief Retrieves the number of bindings location for this layout.
         * @return The number of binding points
         */
        size_t count()const;

        virtual const vk::SetLayout* vk() const { return nullptr; }
        virtual vk::SetLayout* vk() { return nullptr; }

        virtual ref::ShaderSet allocateSet() = 0;

        virtual ~SetLayout();

    protected:

        SetLayout( const kege::SetBindings& bindings );

    protected:

        std::unordered_map< std::string, uint32_t > _binding_set_lookup;
        kege::SetBindings _bindings;
        std::string _name;
        int _total_buffers;
        int _total_images;
        friend ShaderLayout;
        friend ShaderData;
    };
}

namespace kege::ref{
    typedef kege::Ref< kege::SetLayout > SetLayout;
}

namespace kege{

    struct IndexedSetLayout
    {
        /**
         * @var index
         * @brief Descriptor set index within the pipeline layout.
         */
        uint32_t index;

        /**
         * @var set
         * @brief Descriptor set configuration assigned to this index.
         */
        ref::SetLayout set;
    };

   using IndexedSetLayouts = std::vector< IndexedSetLayout >;
}

#endif /* set_layout_hpp */
