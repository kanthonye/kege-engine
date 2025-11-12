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
#include "../mesh/mesh.hpp"
#include "../render/graph/render-queue.hpp"
#include "../render/pipeline/shader-pipeline.hpp"
#include "../../io/virtual-directory.hpp"
#include "../../resource/asset-manager.hpp"
#include "../../utils/frames.hpp"

namespace kege::ui{

    class Viewer : public kege::RefCounter
    {
    public:

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

        void draw( const ui::Widget& content, const ui::Rect& clip_rect );

        /**
         */
        void draw( ui::Layout& layout, int pid, const ui::Rect& clip_rect );

        void linearize( ui::Layout& layout, int pid, int zindex, std::vector< std::pair< int, ui::Widget* > >& nodes, int& count );
        void insertionSort(std::vector< std::pair< int, ui::Widget* > >& arr);

        void drawsort( ui::Layout& layout, int pid );

        void collectVisibleWidgets( RenderExecutor* manager, ui::Layout& layout );

        /**
         */
        void begin();
        void end();

        /**
         * Sets the current font for rendering text.
         *
         * @param font The font to set.
         */
        void setFont(const ref::Font& font);

        /**
         * Retrieves the current font.
         *
         * @return The current font.
         */
        const ref::Font& getFont() const;

        /**
         * Initializes the Core object.
         *
         * @return true if initialization is successful, false otherwise.
         */
        bool initialize( kege::Graphics* graphics, kege::AssetManager* asset_manager, ref::Font font );

        ref::Image getDefaultTexture();

        void setViewportImage( const kege::ImageBindInfo& info, int frame );
        void setThemeImage( const kege::ImageBindInfo& info, int frame );
        void setFontImage( const kege::ImageBindInfo& info, int frame );

        /**
         * Uninitializes the Core object, releasing any allocated resources.
         */
        void shutdown();

        void flush();
        
        ~Viewer();
        Viewer();

    private:

        kege::BufferBindInfo createBuffer();

        std::vector< kege::ui::DrawElem > _drawbuffer;
        ref::Font _font; // The current font used for rendering text.

        ref::ShaderPipeline _shader_pipeline;

        kege::AssetManager* _asset_manager;
        kege::Graphics* _graphics;

        PushConstantBlock _push_constant;
        Extent2D _fbo_size;

        kege::ref::ShaderData _shader_data;
        kege::ref::Mesh _meshs[2];

        kege::BufferBindings _buffer_bindings[2];

        const int _max_render_instances;
        uint64_t _pipeline;

        uint32_t _curr_buffer_index;
        uint32_t _draw_count;
    };

}

#endif /* ui_viewer_hpp */
