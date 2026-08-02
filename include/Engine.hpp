#ifndef H_3NGIN3_H
#define H_3NGIN3_H

//#define VMA_IMPLEMENTATION
//#include "vma/vk_mem_alloc.h"

#include <string>
#include "Khidki.hpp"
#include <vulkan/vulkan.hpp>
#include <iostream>
#include <stdexcept>

#include "vma/vk_mem_alloc.h"


class Engine
{
  private:
    // Members
    const std::string appName = "VULKAN";
    uint32_t vulkanApiVersion = VK_API_VERSION_1_4;
    Khidki khidki{WIDTH, HEIGHT, appName};

    VkInstance instance = nullptr;
    VkSurfaceKHR surface = nullptr;
    VkPhysicalDevice physicalDevice = nullptr;
    uint32_t graphicsQueueFamilyIndex = 0;
    VkDevice logicalDevice = nullptr;
    VkQueue graphicsQueue = nullptr;
    VmaAllocator vmaAllocator = nullptr;

    // Methods
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);


    bool createVulkanInstance();
    VkPhysicalDevice getPhysicalDevice();
    bool getGraphicsQueue();
    bool createLogicalDevice();
    bool initializeVMA();


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
