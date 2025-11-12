//
//  shader-layout.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/18/25.
//

#ifndef shader_layout_hpp
#define shader_layout_hpp

#include <memory>
#include <vector>
#include <string>
#include <cstdint>
#include <map>
#include <unordered_map>
#include "set-layout.hpp"

namespace kege{

    struct LookUpEntry
    {
        enum Type{ PUSH_CONSTANT, LAYOUT };
        LookUpEntry::Type type;
        uint32_t index;
    };

    /**
     * @brief Associates a set index with a layout binding.
     *
     * This structure pairs a set index with a layout binding description. It is ment
     * to be used as a return value from a ShaderLayout object, allowing the use to
     * have the set index and the layout binding which is the binding spot info.
     *
     * @see kege::LayoutBinding
     * @see kege::ShaderLayout
     */
    struct LayoutBindingDesc
    {
        uint32_t index;
        kege::LayoutBinding binding;
    };


    /**
     * @brief Represents the overall layout of a shader, including push constants and set layouts.
     */
    class ShaderLayout : public kege::RefCounter
    {
    public:

        const kege::LookUpEntry* getLookUpEntry( const std::string& name )const;

        /**
         * @brief Retrieves a set binding point by name.
         * @param name The name of the set binding point.
         * @return Pointer to the LayoutBinding if found, nullptr otherwise.
         */
        const kege::ShaderStructBlock* getStructBlock( const std::string& name )const;

        /**
         * @brief Retrieves a set binding point by name.
         * @param name The name of the set binding point.
         * @return Pointer to the LayoutBinding if found, nullptr otherwise.
         */
        const kege::LayoutBindingDesc* getLayoutBindingDesc( const std::string& name )const;

        /**
         * @brief Retrieves a set binding point by name.
         * @param name The name of the set binding point.
         * @return Pointer to the PushBlock if found, nullptr otherwise.
         */
        const kege::PushBlock* getPushBlock( const std::string& name )const;

        /**
         * @brief Retrieves a set binding point by name.
         * @param index The index of the push block.
         * @return Pointer to the PushBlock if found, nullptr otherwise.
         */
        const kege::PushBlock* getPushBlock( int index )const;

        /**
         * @brief Retrieves a set binding point by index.
         * @param set_index The index of the set binding point.
         * @return Pointer to the ShaderBindingPointSet.
         */
        const kege::IndexedSetLayout& getSetLayout( int set_index )const;

        /**
         * @brief Get the set layout associated with the given binding name.
         * @param binding_name The name of the set binding point.
         * @return Pointer to the SetLayout.
         */
        kege::IndexedSetLayout getSetLayout( const std::string& binding_name );

        /**
         * @brief Retrieves a set binding point by index.
         * @param set_index The index of the set binding point.
         * @return Pointer to the SetLayout.
         */
        kege::IndexedSetLayout getSetLayout( int set_index );

        int32_t getSetIndex( const kege::SetLayout* layout )const;

        kege::IndexedSet allocateSet( const std::string& set_name );
        kege::IndexedSet allocateSet( int set_index );

        int getSetLayoutBindSignature()const;

        virtual const vk::ShaderLayout* vk()const = 0;
        virtual vk::ShaderLayout* vk() = 0;

        ShaderLayout
        (
            const std::string& name,
            const kege::IndexedSetLayouts& set_layouts,
            const kege::PushBlockLayout& push_block_layout
        );

        virtual ~ShaderLayout();

    protected:

        struct Entry
        {
            int set_binding_index;
            int set_layout_index;
            int push_block_index;
        };
        std::unordered_map< std::string, Entry > _quick_lookup;


        std::unordered_map< const kege::SetLayout*, int32_t > _set_layout_index_lookup;
        kege::IndexedSetLayouts _indexed_set_layouts;

        std::vector< LayoutBindingDesc > _set_bindings;
        kege::PushBlockLayout _push_block_layout;

        std::string _name;

        int _set_layout_bind_signature;
    };
}
namespace kege::ref{
    typedef kege::Ref< kege::ShaderLayout > ShaderLayout;
}
namespace kege::cref{
    typedef kege::Ref< const kege::ShaderLayout > ShaderLayout;
}
#endif /* shader_layout_hpp */
