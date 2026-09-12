#define GLFW_INCLUDE_VULKAN
#include "Window.hpp"

namespace forge
{

Window::Window(uint32_t WIDTH, uint32_t HEIGHT, std::string appName)
{
    if (!glfwInit()) 
    {
        throw std::runtime_error("forge::Window: Failed to initialize GLFW");
    }

    // 2. Tell GLFW NOT to create an OpenGL context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // 3. Optional: Disable window resizing (or leave it out/set to GLFW_TRUE if wanted)
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // 4. Create the actual window
   pGLFWwindow = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Window", nullptr, nullptr);
    if (!pGLFWwindow) 
    {
        glfwTerminate();
        throw std::runtime_error("forge::Window: Failed to create GLFW window");
    }
}

Window::~Window()
{
  glfwDestroyWindow(pGLFWwindow);
  glfwTerminate();
}


GLFWwindow* Window::getWindowPointer()
{
  return pGLFWwindow;
}

bool Window::shouldClose()
{
  return glfwWindowShouldClose(pGLFWwindow);
}

void Window::handleKey()
{
  if(glfwGetKey(pGLFWwindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(pGLFWwindow, true);
  }
}


}
