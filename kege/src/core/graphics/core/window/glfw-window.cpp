//
//  glfw-window.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/27/25.
//

//#include "../vulkan/vk-instance.hpp"
//#include <vulkan/vulkan.h>
#include "glfw-window.hpp"

namespace kege{

    void keyCallback( GLFWwindow* window, int key, int scancode, int action, int mods )
    {
        kege::Communication::broadcast<const KeyboardEvent&>({window, key, scancode, action, mods});
    }

    void cursorPositionCallback( GLFWwindow* window, double xpos, double ypos )
    {
        kege::Communication::broadcast<const PointerEvent&>({window, xpos, ypos});
    }

    void mouseButtonCallback( GLFWwindow* window, int button, int action, int mods )
    {
        kege::Communication::broadcast<const ButtonEvent&>({window, button, action, mods});
    }

    void scrollCallback( GLFWwindow* window, double xoffset, double yoffset )
    {
        kege::Communication::broadcast<const ScrollEvent&>({window, xoffset, yoffset});
    }

    void windowSizeCallback(GLFWwindow* window, int width, int height)
    {
        kege::Communication::broadcast<const WindowSizeEvent&>({window, width, height});
    }

    void framebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        kege::Communication::broadcast<const WindowFrameBufferSizeEvent&>({window, width, height});
    }
    void windowContentScaleCallback(GLFWwindow* window, float xscale, float yscale)
    {
        kege::Communication::broadcast<const WindowContentScaleEvent&>({window, xscale, yscale});
    }
    void windowPosCallback(GLFWwindow* window, int xpos, int ypos)
    {
        kege::Communication::broadcast<const WindowPositionEvent&>({window, xpos, ypos});
    }
    void windowMaximizeCallback(GLFWwindow* window, int maximized)
    {
        kege::Communication::broadcast<const WindowMaximizeEvent&>({window, maximized});
    }
    void windowFocusCallback(GLFWwindow* window, int focused)
    {
        kege::Communication::broadcast<const WindowFocusEvent&>({window, focused});
    }
    void windowRefreshCallback(GLFWwindow* window)
    {
        kege::Communication::broadcast<const WindowRefreshEvent&>({window});
    }

    std::vector< const char* > GlfwWindow::getRequiredInstanceExtensions()
    {
        uint32_t count = 0;
        const char** extensions = glfwGetRequiredInstanceExtensions( &count );
        return std::vector<const char*>( extensions, extensions + count );
    }

    bool GlfwWindow::create(const WindowCreateInfo& info)
    {
        _create_info = info;

        glfwWindowHint(GLFW_RESIZABLE, info.resizable ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_DECORATED, info.decorated ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_VISIBLE, info.visible ? GLFW_TRUE : GLFW_FALSE);

        _window = glfwCreateWindow(info.width, info.height, info.title.c_str(), nullptr, nullptr);
        if (!_window)
        {
            //throw std::runtime_error("Failed to create GLFW window!");
            return false;
        }

        if (info.fullscreen)
        {
            setFullscreen(true);
        }

        setVSync( info.vsync );

        //float xscale, yscale;
        //glfwGetWindowContentScale(_window, &xscale, &yscale);
        //glfwGetWindowContentScale(_window, &xscale, &yscale);

        glfwSetKeyCallback( _window, keyCallback );
        glfwSetCursorPosCallback( _window, cursorPositionCallback );
        glfwSetMouseButtonCallback( _window, mouseButtonCallback );
        glfwSetScrollCallback( _window, scrollCallback );
        glfwSetWindowSizeCallback(_window, windowSizeCallback);
        glfwSetFramebufferSizeCallback(_window, framebufferSizeCallback);
        glfwSetWindowFocusCallback(_window, windowFocusCallback);
        glfwSetWindowRefreshCallback(_window, windowRefreshCallback);
        glfwSetWindowPosCallback(_window, windowPosCallback);
        glfwSetWindowMaximizeCallback(_window, windowMaximizeCallback);

        return true;
    }

    void GlfwWindow::destroy()
    {
        if (_window)
        {
            glfwDestroyWindow(_window);
            _window = nullptr;
        }
    }

    void GlfwWindow::setTitle(const std::string& title)
    {
        glfwSetWindowTitle( _window, title.data() );
    }

    void GlfwWindow::setSize(uint32_t width, uint32_t height)
    {
        glfwSetWindowSize( _window, width, height );
    }

    void GlfwWindow::setPosition(int x, int y)
    {
        glfwSetWindowPos( _window, x, y );
    }

    void GlfwWindow::setResizable(bool resizable)
    {
        _create_info.resizable = resizable;
        glfwSetWindowAttrib(_window, GLFW_RESIZABLE, resizable);
    }

    void GlfwWindow::setDecorated(bool decorated)
    {
    }

    void GlfwWindow::setFullscreen(bool fullscreen)
    {}

    void GlfwWindow::setVSync(bool enabled)
    {
    }

    Extent2D GlfwWindow::getSize() const
    {
        int w,h;
        glfwGetWindowSize( _window, &w, &h );
        return Extent2D{ uint32_t(w), uint32_t(h) };
    }

    bool GlfwWindow::isVisible() const
    {
        return _create_info.visible;
    }

    bool GlfwWindow::isResizable() const
    {
        return _create_info.resizable;
    }

    bool GlfwWindow::isFullscreen() const
    {
        return _is_fullscreen;
    }

    bool GlfwWindow::isVSyncEnabled() const
    {
        return _vsync_enabled;
    }


    Extent2D GlfwWindow::getFramebufferSize() const
    {
        int w,h;
        glfwGetFramebufferSize( _window, &w, &h );
        return Extent2D{ uint32_t(w), uint32_t(h) };
    }

    vec2 GlfwWindow::getContentScale() const
    {
        vec2 scale;
        glfwGetWindowContentScale( _window, &scale.x, &scale.y );
        return scale;
    }

    bool GlfwWindow::shouldClose() const
    {
        return glfwWindowShouldClose( _window ) != 0;
    }

    void GlfwWindow::pollEvents()
    {
        glfwPollEvents();
    }

    void GlfwWindow::requestAttention()
    {}

    void GlfwWindow::minimize()
    {
    }

    void GlfwWindow::maximize()
    {}

    void GlfwWindow::restore()
    {
        glfwRestoreWindow( _window );
    }

    void GlfwWindow::show()
    {
        glfwShowWindow( _window );
    }

    void GlfwWindow::hide()
    {
        glfwHideWindow( _window );
    }

    void GlfwWindow::terminate()
    {
        glfwTerminate();
    }

    bool GlfwWindow::init()
    {
        if ( !glfwInit() )
        {
            kege::Log::error << "Failed to initialize GLFW!" <<kege::Log::nl;
            return false;
        }
        return true;
    }

    GlfwWindow::GlfwWindow()
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // No OpenGL context
    }

    GlfwWindow::~GlfwWindow()
    {
        destroy();
    }



}

