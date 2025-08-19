//
//  ui-viewer.hpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#ifndef ui_viewer_hpp
#define ui_viewer_hpp

#include "font.hpp"
#include "ui-layout.hpp"
#include "ui-font-loader.hpp"
#include "ui-font-creator.hpp"

namespace kege::ui{

    class Viewer
    {
    public:

        struct PushConstant
        {
            kege::mat44 projection;
            kege::vec4 resolution;
        };

        /**
         */
        void view( kege::CommandEncoder* encoder, ui::Layout& layout, Node& node, const Rect2D& clip_rect );

        /**
         */
        void view( kege::CommandEncoder* command, ui::Layout& layout );

        /**
         * Sets the current font for rendering text.
         *
         * @param font The font to set.
         */
        void setFont(const kege::Font& font);

        /**
         * Retrieves the current font.
         *
         * @return The current font.
         */
        const kege::Font& getFont() const;

        /**
         * Initializes the Core object.
         *
         * @return true if initialization is successful, false otherwise.
         */
        bool initialize( Graphics* graphics, kege::PipelineHandle pipeline, kege::Font font );

        /**
         * Uninitializes the Core object, releasing any allocated resources.
         */
        void setUiImages( SamplerHandle sampler, std::vector< kege::ImageHandle > images );

        /**
         * Uninitializes the Core object, releasing any allocated resources.
         */
        void shutdow();

        void flush( kege::CommandEncoder* encoder );
        
        Viewer();

    private:

        std::vector< kege::ui::DrawElem > _drawbuffer;
        uint32_t _count;
        kege::Font _font; // The current font used for rendering text.

        const int _max_render_instances;

        BufferHandle _gpu_draw_buffer[ kege::MAX_FRAMES_IN_FLIGHT ];
        DescriptorSetHandle _shader_resource_draw_buffer[ kege::MAX_FRAMES_IN_FLIGHT ];
        DescriptorSetHandle _shader_resource_font;
        DescriptorSetHandle _ui_texture;
        kege::PipelineHandle _pipeline;
        Graphics* _graphics;

        PushConstant _push_constant;
        Extent2D _fbo_size;
    };

}

#endif /* ui_viewer_hpp */
