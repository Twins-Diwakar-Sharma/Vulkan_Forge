#ifndef H_3NGIN3_H
#define H_3NGIN3_H

#include <string>
#include "Khidki.hpp"
#include <vulkan/vulkan.hpp>
#include <iostream>
#include <stdexcept>

class Engine
{
  private:
    // Members
    const std::string appName = "VULKAN";
    Khidki khidki{WIDTH, HEIGHT, appName};
    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    int initializationStatus=200;

    // Methods
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);


    bool createVulkanInstance();
    VkPhysicalDevice getPhysicalDevice();


  public:
    // Members
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = WIDTH/16 * 9;

    // Methods
    void run();
    Engine();
    ~Engine();


};

#endif
