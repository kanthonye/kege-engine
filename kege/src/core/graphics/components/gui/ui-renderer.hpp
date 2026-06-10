//
//  ui-renderer.hpp
//  editor
//
//  Created by Kenneth Esdaile on 5/6/26.
//

#ifndef ui_renderer_hpp
#define ui_renderer_hpp

#include "../font/font.hpp"
#include "../mesh/mesh.hpp"
#include "../../render/renderer.hpp"
#include "../../../io/virtual-directory.hpp"
#include "../../../resource/asset-manager.hpp"
#include "../../../utils/frames.hpp"

#include "ui-layout.hpp"

namespace kege::ui{

    class Renderer
    {
    public:

        kege::ui::Extent rendererText
        (
            const kege::vec2& start,
            float width,
            float font_size,
            uint32_t color,
            bool wrap_around,
            const char* text,
            const kege::ui::Rect& clip_rect
        );

        void renderWidget
        (
            const kege::ui::Layout& layout,
            const kege::ui::Widget* widget,
            kege::ui::Rect clip_rect
        );

        void render( const ui::Layout& layout );

        void draw( const kege::ui::DrawInstance& instance );

        const kege::ui::DrawBatch& getGuiComponent() const;

        void onWindowResize(const kege::Extent2D& extent);

        const kege::ref::Font& getFont() const;

        bool initialize
        (
            kege::Renderer* renderer,
            kege::AssetManager* assets,
            kege::ECS* ecs
        );

        void shutdown();
        void begin();
        void end();

        Renderer();

    private:

        bool createRenderableResources();
        bool loadPipeline();
        bool loadFont();

    public:

        enum : uint32_t { MAX_DRAW_COUNT = 30000 };
        kege::ref::Buffer _ui_instance_buffers[ MAX_FRAMES_IN_FLIGHT ];
        //kege::ui::DrawCommand* _ui_draw_command_buffer;

        //kege::ui::GuiDrawBatch _gui_draw_batch;

        kege::GraphicsDevice* _graphics;
        kege::AssetManager* _assets;
        kege::ECS* _ecs;

        kege::ref::Font _font;
    };

}

#endif /* ui_renderer_hpp */
