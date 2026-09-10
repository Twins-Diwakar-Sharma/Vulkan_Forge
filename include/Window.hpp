#ifndef H_W1ND0W_H
#define H_W1ND0W_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace forge
{
  class Window
  {
    private:
      GLFWwindow* pGLFWwindow;
      void initWindow();
      const uint32_t width;
      const uint32_t height;
      std::string windowName;
    public:
      Window(uint32_t w, uint32_t h, std::string name);
      ~Window();
      Window(const Window &) = delete;
      Window& operator=(const Window &) = delete;

      bool shouldClose();
      GLFWwindow* getWindowPointer();
      void handleKey();
  };

}

#endif
