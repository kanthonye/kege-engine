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
     * @brief Represents the overall layout of a shader, including push constants and set layouts.
     */
    class ShaderLayout : public kege::RefCounter
    {
    public:

        const kege::LookUpEntry* getLookUpEntry( const std::string& name )const;

        /**
         * @brief Retrieves a set binding point by name.
         * @param name The name of the set binding point.
         * @return Pointer to the ShaderBindingPointSet if found, nullptr otherwise.
         */
        const kege::PushBlock* getPushBlock( const std::string& name )const;
        const kege::PushBlock* getPushBlock( int index )const;

        /**
         * @brief Retrieves a set binding point by name.
         * @param name The name of the set binding point.
         * @return Pointer to the ShaderBindingPointSet if found, nullptr otherwise.
         */
        kege::SetLayout* getPushBlock( const std::string& name );

        /**
         * @brief Retrieves a set binding point by name.
         * @param name The name of the set binding point.
         * @return Pointer to the ShaderBindingPointSet if found, nullptr otherwise.
         */
        const ref::SetLayout getSetLayout( const std::string& name )const;

        /**
         * @brief Retrieves a set binding point by name.
         * @param name The name of the set binding point.
         * @return Pointer to the ShaderBindingPointSet if found, nullptr otherwise.
         */
        ref::SetLayout getSetLayout( const std::string& name );

        /**
         * @brief Retrieves a set binding point by index.
         * @param set_index The index of the set binding point.
         * @return Pointer to the ShaderBindingPointSet.
         */
        const ref::SetLayout& getSetLayout( int set_index )const;

        /**
         * @brief Retrieves a set binding point by index.
         * @param set_index The index of the set binding point.
         * @return Pointer to the ShaderBindingPointSet.
         */
        ref::SetLayout getSetLayout( int set_index );

        int32_t getSetIndex( const kege::SetLayout* layout )const;
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

        std::unordered_map< const kege::SetLayout*, int32_t > _set_index_mapper;

        std::unordered_map< std::string, LookUpEntry > _quick_lookup;
        kege::IndexedSetLayouts _indexed_set_layouts;
        kege::PushBlockLayout _push_block_layout;
        std::string _name;
    };
}
namespace kege::ref{
    typedef kege::Ref< kege::ShaderLayout > ShaderLayout;
}
namespace kege::cref{
    typedef kege::Ref< const kege::ShaderLayout > ShaderLayout;
}
#endif /* shader_layout_hpp */
