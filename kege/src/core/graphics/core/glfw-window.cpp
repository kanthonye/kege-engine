//
//  glfw-window.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/27/25.
//

#include "../devices/vulkan/vulkan-instance.hpp"
#include "../../graphics/core/glfw-window.hpp"

namespace kege{

    static GlfwWindow* global_window_ptr;


    void keyCallback( GLFWwindow* window, int key, int scancode, int action, int mods )
    {
        if ( global_window_ptr == nullptr ) return;
        global_window_ptr->onKeyboard( key, scancode, action, mods );
    }

    void cursorPositionCallback( GLFWwindow* window, double xpos, double ypos )
    {
        if ( global_window_ptr == nullptr ) return;
        global_window_ptr->onCursorPosition( xpos, ypos );
    }

    void mouseButtonCallback( GLFWwindow* window, int button, int action, int mods )
    {
        if ( global_window_ptr == nullptr ) return;
        global_window_ptr->onMouseButton( button, action, mods );
    }

    void scrollCallback( GLFWwindow* window, double xoffset, double yoffset )
    {
        if ( global_window_ptr == nullptr ) return;
        global_window_ptr->onScroll( xoffset, yoffset );
    }

    GraphicsSurface GlfwWindow::createSurface( GraphicsInstance* instance )
    {
        switch ( instance->getGraphicsAPI() )
        {
            case GraphicsAPI::Vulkan:
            {
                VkSurfaceKHR surface;
                vk::Instance* vk_instance = static_cast< vk::Instance* >( instance );
                glfwCreateWindowSurface( vk_instance->getHandle(), _window, nullptr, &surface );
                return surface;
            }

            default: break;
        }
        return nullptr;
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

        glfwSetKeyCallback( _window, keyCallback );
        glfwSetCursorPosCallback( _window, cursorPositionCallback );
        glfwSetMouseButtonCallback( _window, mouseButtonCallback );
        glfwSetScrollCallback( _window, scrollCallback );

        global_window_ptr = this;
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
    }

    void GlfwWindow::setDecorated(bool decorated)
    {
    }

    void GlfwWindow::setFullscreen(bool fullscreen)
    {}

    void GlfwWindow::setVSync(bool enabled)
    {
    }

    uint32_t GlfwWindow::getWidth() const
    {
        return _create_info.width;
    }

    uint32_t GlfwWindow::getHeight() const
    {
        return _create_info.height;
    }

//    ivec2 GlfwWindow::getPosition() const
//    {
//        return {_create_info.pos_x,_create_info.pos_y};
//    }

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
        int width; int height;
        glfwGetFramebufferSize( _window, &width, &height );
        return Extent2D{uint32_t(width), uint32_t(height)};
    }

    vec2 GlfwWindow::getContentScale() const
    {
        float xscale,yscale;
        glfwGetWindowContentScale( _window, &xscale, &yscale );
        return {xscale, yscale};
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

    GlfwWindow::GlfwWindow()
    {
        if ( !glfwInit() )
        {
            throw std::runtime_error( "Failed to initialize GLFW!" );
        }
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // No OpenGL context
    }

    GlfwWindow::~GlfwWindow()
    {
        destroy();
        glfwTerminate();
    }



}

