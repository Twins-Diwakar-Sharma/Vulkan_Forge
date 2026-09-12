#ifndef H_F0rGe_W1nd0w_H
#define H_F0rGe_W1nd0w_H

#include "scribe.hpp"
#include <GLFW/glfw3.h>
#include <string>

namespace forge
{
  class Window
  {
    private:
      GLFWwindow* pGLFWwindow;
    public:
      Window(uint32_t WIDTH, uint32_t HEIGHT, std::string appName);
      ~Window();
      
      GLFWwindow* getWindowPointer();
      bool shouldClose(); 
      void handleKey();
  };
}

#endif
