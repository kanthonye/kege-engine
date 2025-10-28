//
//  shader-binding-resrc.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/18/25.
//

#ifndef shader_binding_resrc_hpp
#define shader_binding_resrc_hpp

#include "graphics.hpp"

namespace kege{

    /**
     * @brief Abstract base class for shader resource bindings.
     */
    struct ShaderBindingElement : public RefCounter
    {
        virtual void emplaceBuffers( const std::vector< BufferInfo >& i ) {}

        virtual const BufferInfo* buffer( int i = 0 ) const { return nullptr; }
        virtual BufferInfo* buffer( int i = 0 ) { return nullptr; }

        virtual const ImageInfo* image( int i = 0 ) const { return nullptr; }
        virtual ImageInfo* image( int i = 0 ) { return nullptr; }

        virtual int count() const { return 0; }

        virtual bool isBufferView() const { return false; }
        virtual bool isBuffer() const { return false; }
        virtual bool isImage() const { return false; }

        virtual ~ShaderBindingElement(){}
    };

    /**
     * @brief Represents a binding of multiple buffers.
     */
    struct ShaderBindingBuffers : public ShaderBindingElement
    {
        const BufferInfo* buffer( int i ) const { return &buffers[i]; }
        BufferInfo* buffer( int i ) { return &buffers[i]; }
        int count() const { return (int) buffers.size(); }
        bool isBuffer() const { return true; }

        ShaderBindingBuffers( const BufferBindings& info )
        :   buffers( info )
        {}
        std::vector< BufferInfo > buffers;
    };

    /**
     * @brief Represents a binding of multiple images.
     */
    struct ShaderBindingImages : public ShaderBindingElement
    {
        virtual const ImageInfo* image( int i ) const { return &images[i]; }
        virtual ImageInfo* image( int i ) { return &images[i]; }
        int count() const { return (int) images.size(); }
        bool isImage() const { return true; }

        ShaderBindingImages( const ImageBindings& info )
        :   images( info )
        {}
        std::vector< ImageInfo > images;
    };

    /**
     * @brief Represents a binding of a single buffer.
     */
    struct BufferBinding : public ShaderBindingElement
    {
        const BufferInfo* buffer( int i ) const { return &info; }
        BufferInfo* buffer( int i ) { return &info; }
        int count() const { return 1; }
        bool isBuffer() const { return true; }
        BufferBinding(const BufferInfo& i): info(i) {}
        BufferInfo info;
    };

    /**
     * @brief Represents a binding of a single image.
     */
    struct ImageBinding : public ShaderBindingElement
    {
        virtual const ImageInfo* image( int i = 0 ) const { return &info; }
        virtual ImageInfo* image( int i = 0 ) { return &info; }
        int count() const { return 1; }
        bool isImage() const { return true; }
        ImageBinding(const ImageInfo& i): info(i) {}
        ImageInfo info;
    };

}
#endif /* shader_binding_resrc_hpp */
