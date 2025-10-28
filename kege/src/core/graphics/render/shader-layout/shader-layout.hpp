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
#include "shader-constant-binding-point.hpp"
#include "shader-set-binding-point-layout.hpp"

namespace kege{

    /**
     * @brief Represents the overall layout of a shader, including push constants and set layouts.
     */
    class ShaderLayout : public kege::RefCounter
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
         * @return Pointer to the ShaderSetBindingPoint if found, nullptr otherwise.
         */
        const ShaderSetBindingPoint* get( const std::string& name )const;

        /**
         * @brief Retrieves a set binding point by name.
         * @param name The name of the set binding point.
         * @return Pointer to the ShaderSetBindingPoint if found, nullptr otherwise.
         */
        ShaderSetBindingPoint* get( const std::string& name );

        /**
         * @brief Retrieves a set binding point by index.
         * @param binding_index The index of the set binding point.
         * @return Pointer to the ShaderSetBindingPoint.
         */
        const ShaderSetBindingPoint* getBinding( int set_index, int binding_index )const;

        /**
         * @brief Retrieves a set binding point by index.
         * @param binding_index The index of the set binding point.
         * @return Pointer to the ShaderSetBindingPoint.
         */
        ShaderSetBindingPoint* getBinding( int set_index, int binding_index );

        /**
         * @brief Retrieves a set binding point by index.
         * @param set_index The index of the set binding point.
         * @return Pointer to the ShaderSetBindingPoint.
         */
        const ShaderSetBindingPointLayout* getSet( int set_index )const;

        /**
         * @brief Retrieves a set binding point by index.
         * @param set_index The index of the set binding point.
         * @return Pointer to the ShaderSetBindingPoint.
         */
        ShaderSetBindingPointLayout* getSet( int set_index );

        //PipelineBindPoint getPipelineBindPoint()const;
        ShaderLayout
        (
            const std::string& name,
            std::vector< ref::ShaderSetBindingPointLayout >& shader_set_binding_layouts,
            std::vector< Ref< ShaderConstantBindingPoint > >& push_constant_blocks
        );

        virtual ~ShaderLayout();

    protected:
        
        enum LookUpType{ PUSH_CONSTANT, LAYOUT };

        struct LookUpEntry
        {
            LookUpType type;
            size_t index;
        };

        std::vector< Ref< ShaderSetBindingPointLayout > > shader_set_binding_point_layouts;
        std::unordered_map< std::string, int > layout_lookup_map;

        std::vector< Ref< ShaderConstantBindingPoint > > shader_constant_binding_points;

        std::unordered_map< std::string, LookUpEntry > quick_lookup_map;
        std::vector< ShaderBindingBlock* > shader_binding_blocks;

        std::string name;

        friend ShaderData;
    };

    struct ShaderLayoutDesc
    {
        std::map< int, std::vector< Ref< ShaderSetBindingPoint > > > shader_set_binding_points;
        std::vector< Ref< ShaderConstantBindingPoint > > push_constant_blocks;
        std::string name;
    };

}
#endif /* shader_layout_hpp */
