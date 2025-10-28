//
//  shader-binding-point.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/18/25.
//

#ifndef shader_binding_point_hpp
#define shader_binding_point_hpp

#include "shader-binding-element.hpp"

namespace kege{

    /**
     * @brief Represents a binding point in a shader for various resource types.
     */
    struct ShaderBindingResrc : public kege::RefCounter
    {
        enum Type { UNDEFINED, BUFFER, IMAGE, SAMPLER, BUFFER_VIEW };

        /**
         * @brief Retrieves the buffer info at the specified index.
         * @param i Index of the buffer to retrieve.
         * @return Pointer to the BufferInfo.
         */
        const BufferInfo* buffer( int i = 0 ) const { return resource->buffer( i ); }

        /**
         * @brief Retrieves the buffer info at the specified index.
         * @param i Index of the buffer to retrieve.
         * @return Pointer to the BufferInfo.
         */
        BufferInfo* buffer( int i = 0 ) { return resource->buffer( i ); }

        /**
         * @brief Retrieves the image info at the specified index.
         * @param i Index of the image to retrieve.
         * @return Pointer to the ImageInfo.
         */
        const ImageInfo* image( int i = 0 ) const { return resource->image( i ); }

        /**
         * @brief Retrieves the image info at the specified index.
         * @param i Index of the image to retrieve.
         * @return Pointer to the ImageInfo.
         */
        ImageInfo* image( int i = 0 ) { return resource->image( i ); }

        /**
         * @brief Retrieves the count of resources bound.
         * @return Number of resources.
         */
        int count() const { return resource->count(); }

        /**
         * @brief Checks if the binding point is a buffer view.
         * @return True if it is a buffer view, false otherwise.
         */
        bool isBufferView() const { return resource->isBufferView(); }

        /**
         * @brief Checks if the binding point is a buffer.
         * @return True if it is a buffer, false otherwise.
         */
        bool isBuffer() const { return resource->isBuffer(); }

        /**
         * @brief Checks if the binding point is an image.
         * @return True if it is an image, false otherwise.
         */
        bool isImage() const { return resource->isImage(); }

        /**
         * @brief Assignment operator for ShaderBindingResrc.
         * @param other The other ShaderBindingResrc to assign from.
         * @return Reference to this ShaderBindingResrc.
         */
        ShaderBindingResrc& operator = ( const ShaderBindingResrc& other )
        {
            type = other.type;
            resource = other.resource;
            return *this;
        }

        /**
         * @brief Move assignment operator for ShaderBindingResrc.
         * @param other The other ShaderBindingResrc to move from.
         * @return Reference to this ShaderBindingResrc.
         */
        ShaderBindingResrc& operator = ( ShaderBindingResrc&& other )
        {
            type = other.type;
            resource = other.resource;
            other.type = UNDEFINED;
            other.resource = {};
            return *this;
        }

        /**
         * @brief Assignment operator for buffer view bindings.
         * @param info The buffer view bindings to assign.
         * @return Reference to this ShaderBindingResrc.
         */
        ShaderBindingResrc& operator = ( const BufferViewBindings& info )
        {
            return *this;
        }

        /**
         * @brief Assignment operator for buffer bindings.
         * @param info The buffer bindings to assign.
         * @return Reference to this ShaderBindingResrc.
         */
        ShaderBindingResrc& operator = ( const BufferBindings& info )
        {
            resource.clear();
            resource = new ShaderBindingBuffers( info );
            return *this;
        }

        /**
         * @brief Assignment operator for image bindings.
         * @param info The image bindings to assign.
         * @return Reference to this ShaderBindingResrc.
         */
        ShaderBindingResrc& operator = ( const ImageBindings& info )
        {
            resource.clear();
            resource = new ShaderBindingImages( info );
            return *this;
        }

        /**
         * @brief Assignment operator for a single buffer info.
         * @param info The buffer info to assign.
         * @return Reference to this ShaderBindingResrc.
         */
        ShaderBindingResrc& operator = ( const BufferInfo& info )
        {
            resource.clear();
            resource = new BufferBinding( info );
            return *this;
        }

        /**
         * @brief Assignment operator for a single image info.
         * @param info The image info to assign.
         * @return Reference to this ShaderBindingResrc.
         */
        ShaderBindingResrc& operator = ( const ImageInfo& info )
        {
            resource.clear();
            resource = new ImageBinding( info );
            return *this;
        }

        /**
         * @brief Constructor for buffer view bindings.
         * @param binding The buffer view bindings.
         */
        explicit ShaderBindingResrc( const BufferViewBindings& binding )
        :   type( BUFFER_VIEW )
        {}

        /**
         * @brief Constructor for buffer bindings.
         * @param info The buffer bindings.
         */
        explicit ShaderBindingResrc( const BufferBindings& info )
        :   type( BUFFER )
        ,   resource( new ShaderBindingBuffers( info ) )
        {}

        /**
         * @brief Constructor for image bindings.
         * @param info The image bindings.
         */
        explicit ShaderBindingResrc( const ImageBindings& info )
        :   type( IMAGE )
        ,   resource( new ShaderBindingImages( info ) )
        {}

        /**
         * @brief Constructor for a single buffer info.
         * @param info The buffer info.
         */
        explicit ShaderBindingResrc( const BufferInfo& info )
        :   type( BUFFER )
        ,   resource( new BufferBinding( info ) )
        {}

        /**
         * @brief Constructor for a single image info.
         * @param info The image info.
         */
        explicit ShaderBindingResrc( const ImageInfo& info )
        :   type( IMAGE )
        ,   resource( new ImageBinding( info ) )
        {}

        /**
         * @brief Copy constructor for ShaderBindingResrc.
         * @param other The other ShaderBindingResrc to copy from.
         */
        explicit ShaderBindingResrc( const ShaderBindingResrc& other )
        :   type( other.type )
        ,   resource( other.resource )
        {}

        /**
         * @brief Move constructor for ShaderBindingResrc.
         * @param other The other ShaderBindingResrc to move from.
         */
        explicit ShaderBindingResrc( ShaderBindingResrc&& other )
        :   type( other.type )
        ,   resource( other.resource )
        {
            other.type = UNDEFINED;
            other.resource = {};
        }

        ~ShaderBindingResrc(){}

        ShaderBindingResrc()
        :   type( UNDEFINED )
        {}

        ShaderBindingResrc::Type type;
        Ref< ShaderBindingElement > resource;
    };

}
#endif /* shader_binding_point_hpp */
