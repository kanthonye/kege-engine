//
//  render-pass-render-functions.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 4/24/26.
//

#ifndef render_pass_render_functions_hpp
#define render_pass_render_functions_hpp

#include "../../render/graph/render-pass-desc.hpp"

namespace kege{

    struct TextureManager
    {
        std::map<kege::Image*, uint> image_to_slot;
        kege::ImageBindings image_bindings;

        kege::ref::ShaderResourceBinder resource_binder;

        void bind(Image* img);
        void evict(Image* img);
        uint getSlot(Image* img);
    };

    struct MeshManager
    {
        std::map<kege::Image*, uint> image_to_slot;
        kege::ImageBindings image_bindings;

        void bind(Image* img);
        void evict(Image* img);
        uint getSlot(Image* img);
    };

    struct MaterialManager
    {
        std::map<kege::Image*, uint> image_to_slot;
        kege::ImageBindings image_bindings;

        kege::ref::ShaderResourceBinder resource_binder;

        void bind(Image* img);
        void evict(Image* img);
        uint getSlot(Image* img);
    };


    void renderDeferredStaticGeometry(kege::RenderPassContext& context);
    void forwardRenderingStaticMesh(kege::RenderPassContext& context);
}
#endif /* render_pass_render_functions_hpp */
