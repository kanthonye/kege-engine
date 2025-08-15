//
//  graphics-window.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/27/25.
//

#ifndef graphics_window_hpp
#define graphics_window_hpp

#include "../../graphics/core/graphics-core.hpp"

namespace kege{

    class GraphicsInstance;

    // WindowCreateInfo structure
    struct WindowCreateInfo
    {
        std::string title = "Untitled Window";
        uint32_t width = 1280;
        uint32_t height = 720;
        bool visible = true;
        bool resizable = true;
        bool fullscreen = false;
        bool maximized = false;
        bool decorated = true;
        bool vsync = false;    // VSync on or off
    };


    class WindowListener;


    // Abstract GraphicsWindow interface
    class GraphicsWindow : public RefCounter
    {
    public:

        // Surface creation (for Vulkan, DirectX, etc.)
        virtual GraphicsSurface createSurface( GraphicsInstance* instance ) = 0;
        virtual std::vector< const char* > getRequiredInstanceExtensions() = 0;

        // Window lifecycle
        virtual bool create(const WindowCreateInfo& info) = 0;
        virtual void destroy() = 0;

        // Framebuffer / DPI
        virtual Extent2D getFramebufferSize() const = 0;
        virtual vec2 getContentScale() const = 0;

        // Frame lifecycle
        virtual bool shouldClose() const = 0;
        virtual void pollEvents() = 0;

        // Window state control
        virtual void show() = 0;
        virtual void hide() = 0;
        virtual void minimize() = 0;
        virtual void maximize() = 0;
        virtual void restore() = 0; // un-minimize or un-maximize
        virtual void requestAttention() = 0; // flash the window on taskbar (optional)

        // Window properties
        virtual void setTitle(const std::string& title) = 0;
        virtual void setSize(uint32_t width, uint32_t height) = 0;
        virtual void setPosition(int x, int y) = 0;
        virtual void setResizable(bool resizable) = 0;
        virtual void setDecorated(bool decorated) = 0; // Borderless toggle
        virtual void setFullscreen(bool fullscreen) = 0;
        virtual void setVSync(bool enabled) = 0;

        // Window queries
        virtual uint32_t getWidth() const = 0;
        virtual uint32_t getHeight() const = 0;
        //virtual ivec2 getPosition() const = 0;
        virtual bool isVisible() const = 0;
        virtual bool isResizable() const = 0;
        virtual bool isFullscreen() const = 0;
        virtual bool isVSyncEnabled() const = 0;

        void onKeyboard( int key, int scancode, int action, int mods );
        void onMouseButton( int button, int action, int mods );
        void onCursorPosition( double xpos, double ypos );
        void onScroll( double xoffset, double yoffset );

        void removeListener( WindowListener* listener );
        void addListener( WindowListener* listener );

        virtual ~GraphicsWindow() = default;

    protected:

        std::vector< WindowListener* > _listeners;
    };



    class WindowListener
    {
    public:

        virtual void onKeyboard( int key, int scancode, int action, int mods ){}
        virtual void onMouseButton( int button, int action, int mods ){}
        virtual void onCursorPosition( double xpos, double ypos ){}
        virtual void onScroll( double xoffset, double yoffset ){}
        virtual ~WindowListener(){}
    };

}

namespace kege{

    struct WindowResized
    {
    public:

        static void callback(int width, int height);
        static int getHeight();
        static int getWidth();

    private:

        static int _width;
        static int _height;
    };

    struct FramebufferResized
    {
    public:

        static void callback(int width, int height);
        static int getHeight();
        static int getWidth();

    private:

        static int _width;
        static int _height;
    };
}
#endif /* graphics_window_hpp */
