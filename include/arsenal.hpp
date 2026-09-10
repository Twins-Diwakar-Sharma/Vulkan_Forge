#ifndef H_4RS3NAL_H
#define H_4RS3NAL_H

#include "scribe.hpp"
#include <vector>
#include <vulkan/vulkan.hpp>
#include <iostream>
#include "vk_mem_alloc.h"
#include <GLFW/glfw3.h>

namespace forge
{
  namespace arsenal
  {
    static constexpr uint8_t success = 0;
    static constexpr uint8_t failure = 1;

    extern uint32_t vulkanApiVersion;
    extern VkInstance instance;
    extern VkSurfaceKHR surface;
    extern VkPhysicalDevice physicalDevice;
    extern uint32_t graphicsQueueFamilyIndex;
    extern VkDevice device;
    extern VkQueue graphicsQueue;
    extern VmaAllocator vmaAllocator;

    // Methods public
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);
    
    void evoke(const std::string* appName, GLFWwindow* pGLFWwindow, uint32_t width, uint32_t height);
    void eradicate();

    // private methods just to list, donot make them in header
    /*
    uint8_t createVulkanInstance(const std::string& appName);
    uint8_t createWindowSurface();
    void selectPhysicalDevice();
    uint8_t findGraphicsQueue();
    uint8_t createLogicalDevice();
    uint8_t createVMA();
    */


  } // end arsenal
} // end forge

#endif
