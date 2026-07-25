#ifndef H_3NGIN3_H
#define H_3NGIN3_H

#include <string>
#include "Khidki.hpp"
#include <vulkan/vulkan.hpp>

class Engine
{
  private:
    const std::string appName = "VULKAN";
    Khidki khidki{WIDTH, HEIGHT, appName};
    VkInstance instance;

  public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = WIDTH/16 * 9;
    void run();
    Engine();
    ~Engine();


};

#endif
