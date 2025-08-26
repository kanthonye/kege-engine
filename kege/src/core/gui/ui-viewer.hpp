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

        kege::vec2 drawText
        (
            const kege::vec2& start,
            float width,
            float font_size,
            const ui::Color& color,
            bool wrap_around,
            const char* text,
            const ui::Rect& clip_rect
        );

        void draw( const ui::Content& content, const ui::Rect& clip_rect );

        /**
         */
        void draw( ui::Layout& layout, int pid, const ui::Rect& clip_rect );

        void linearize( ui::Layout& layout, int pid, int zindex, std::vector< std::pair< int, ui::Content* > >& nodes, int& count );
        void insertionSort(std::vector< std::pair< int, ui::Content* > >& arr);

        void drawsort( ui::Layout& layout, int pid );

        /**
         */
        void begin( kege::CommandEncoder* command );
        void end();

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

        void flush();
        
        Viewer();

    private:

        std::vector< kege::ui::DrawElem > _drawbuffer;
        uint32_t _count;
        kege::Font _font; // The current font used for rendering text.

        const int _max_render_instances;

        BufferHandle _gpu_draw_buffer[ kege::MAX_FRAMES_IN_FLIGHT ];
        DescriptorSetHandle _shader_resource_draw_buffer[ kege::MAX_FRAMES_IN_FLIGHT ];
        DescriptorSetHandle _shader_resource_font;
        DescriptorSetHandle _descriptor_ui_texture;
        kege::PipelineHandle _pipeline;

        kege::ImageHandle _default_ui_texture;

        kege::CommandEncoder* _encoder;
        Graphics* _graphics;

        PushConstant _push_constant;
        Extent2D _fbo_size;
    };

}

#endif /* ui_viewer_hpp */
