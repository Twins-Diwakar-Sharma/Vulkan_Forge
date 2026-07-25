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
    const int width;
    const int height;
    std::string windowName;
  public:
    Khidki(int w, int h, std::string name);
    ~Khidki();
    Khidki(const Khidki &) = delete;
    Khidki& operator=(const Khidki &) = delete;

    bool shouldClose();
};

#endif
