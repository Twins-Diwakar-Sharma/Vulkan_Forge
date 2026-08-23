#ifndef H_3NGIN3_H
#define H_3NGIN3_H

#include <string>
#include "Khidki.hpp"
#include <iostream>
#include <stdexcept>
#include "VulkanContext.hpp"
#include "Pipeline.hpp"


class Engine
{
  private:
    // Members
    const std::string appName = "VULKAN";
    Khidki khidki{WIDTH, HEIGHT, appName};
    //VulkanContext vkContext{appName, khidki.getWindowPointer(), WIDTH, HEIGHT};
    Pipeline pipeline;
  public:
    // Members
    static constexpr uint32_t WIDTH = 800;
    static constexpr uint32_t HEIGHT = WIDTH/16 * 9;

    // Methods
    void run();
    Engine();
    ~Engine();


};

#endif
