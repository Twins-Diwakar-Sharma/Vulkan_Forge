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
    Pipeline pipeline;

    // Methods
    void update();
    void input();
    void render();

    uint64_t frameIndex = 0;
    uint64_t nextSignalValue = vulkancontext_MaxFramesInFlight;
    bool requireSwapchainRecreate = false;
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
