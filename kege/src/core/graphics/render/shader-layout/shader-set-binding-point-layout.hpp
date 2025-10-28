//
//  shader-set-binding-layout.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/18/25.
//

#ifndef shader_set_binding_layout_hpp
#define shader_set_binding_layout_hpp

#include "shader-set-binding-point.hpp"

namespace kege {
    
    class ShaderResrcBindingSet;

    /**
     * @brief Represents a layout of shader set binding points.
     */
    class ShaderSetBindingPointLayout : public kege::RefCounter
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
        const ShaderSetBindingPoint* getBinding( int binding_index )const;

        /**
         * @brief Retrieves a set binding point by index.
         * @param binding_index The index of the set binding point.
         * @return Pointer to the ShaderSetBindingPoint.
         */
        ShaderSetBindingPoint* getBinding( int binding_index );

        size_t count()const;
        
        virtual ref::ShaderSet allocateShaderBindingSet( const ShaderResrcBindingSet& binding_points ) = 0;
        virtual ~ShaderSetBindingPointLayout();

    protected:
        
        std::vector< Ref< ShaderSetBindingPoint > > binding_points;
        std::unordered_map< std::string, size_t > quick_lookup_map;
        friend ShaderLayout;
        friend ShaderData;
    };

}
#endif /* shader_set_binding_layout_hpp */
