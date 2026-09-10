#include "Window.hpp"

namespace forge
{

  Window::Window(uint32_t w, uint32_t h, std::string name): width(w), height(h), windowName(name)
  {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // WHY??

    pGLFWwindow = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
  }

  Window::~Window()
  {
    if(pGLFWwindow)
      glfwDestroyWindow(pGLFWwindow);
    glfwTerminate();
  }

  bool Window::shouldClose()
  {
    return glfwWindowShouldClose(pGLFWwindow);
  }

  GLFWwindow* Window::getWindowPointer()
  {
    return pGLFWwindow;
  }

  void Window::handleKey()
  {
    if(glfwGetKey(pGLFWwindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
      glfwSetWindowShouldClose(pGLFWwindow, true);
    }
  }

} // end forge
