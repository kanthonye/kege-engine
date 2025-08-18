//
//  glfw-window.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/27/25.
//

#ifndef glfw_window_hpp
#define glfw_window_hpp

#define GLFW_INCLUDE_VULKAN


//#include <vk_mem_alloc.h>  // Include Vulkan Memory Allocator (VMA)


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wnullability-completeness"
#include <GLFW/glfw3.h>
#pragma clang diagnostic pop

#include "../../graphics/core/graphics-window.hpp" // your abstract interface

namespace kege{

    class GlfwWindow : public GraphicsWindow
    {
    public:

        GraphicsSurface createSurface( GraphicsInstance* instance ) override;
        std::vector< const char* > getRequiredInstanceExtensions() override;
        bool create(const WindowCreateInfo& info) override;
        void destroy() override;

        void setTitle(const std::string& title) override;
        void setSize(uint32_t width, uint32_t height) override;
        void setPosition(int x, int y) override;
        void setResizable(bool resizable) override;
        void setDecorated(bool decorated) override;
        void setFullscreen(bool fullscreen) override;
        void setVSync(bool enabled) override;

        uint32_t getWidth() const override;
        uint32_t getHeight() const override;
        //ivec2 getPosition() const override;
        bool isVisible() const override;
        bool isResizable() const override;
        bool isFullscreen() const override;
        bool isVSyncEnabled() const override;

        Extent2D getFramebufferSize() const override;
        vec2 getContentScale() const override;

        bool shouldClose() const override;
        void pollEvents() override;

        void requestAttention() override;
        void minimize() override;
        void maximize() override;
        void restore() override;
        void show() override;
        void hide() override;

        ~GlfwWindow() override;
        GlfwWindow();

    private:

        friend void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        friend void cursorPositionCallback( GLFWwindow* window, double xpos, double ypos );
        friend void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        friend void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    private:

        GLFWwindow* _window = nullptr;
        WindowCreateInfo _create_info;
        Extent2D _framebuffer_extent;
        bool _is_fullscreen = false;
        bool _vsync_enabled = false;
    };

}
#endif /* glfw_window_hpp */
