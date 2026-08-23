#ifndef H_KHIDKI_H
#define H_KHIDKI_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

class Khidki
{
  private:
    GLFWwindow* window;
    void initWindow();
    const uint32_t width;
    const uint32_t height;
    std::string windowName;
  public:
    Khidki(uint32_t w, uint32_t h, std::string name);
    ~Khidki();
    Khidki(const Khidki &) = delete;
    Khidki& operator=(const Khidki &) = delete;

    bool shouldClose();
    GLFWwindow* getWindowPointer();
};

#endif
