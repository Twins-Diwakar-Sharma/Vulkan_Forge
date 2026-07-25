#include "Khidki.hpp"

Khidki::Khidki(int w, int h, std::string name): width(w), height(h), windowName(name)
{
  initWindow();
}

Khidki::~Khidki()
{
  glfwDestroyWindow(window);
  glfwTerminate();
}


void Khidki::initWindow()
{
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // WHY??

  window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);

}

bool Khidki::shouldClose()
{
  return glfwWindowShouldClose(window);
}
