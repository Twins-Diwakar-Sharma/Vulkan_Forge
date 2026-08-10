#ifndef H_3NGIN3_H
#define H_3NGIN3_H

#include <string>
#include "Khidki.hpp"
#include <vulkan/vulkan.hpp>
#include <iostream>
#include <stdexcept>

#include "vk_mem_alloc.h"


class Engine
{
  private:
    // Members
    const std::string appName = "VULKAN";
    uint32_t vulkanApiVersion = VK_API_VERSION_1_4;
    Khidki khidki{WIDTH, HEIGHT, appName};
    uint32_t swapchainWidth, swapchainHeight;
    uint32_t numSwapchainImages = 2u;


    VkInstance instance = nullptr;
    VkSurfaceKHR surface = nullptr;
    VkPhysicalDevice physicalDevice = nullptr;
    uint32_t graphicsQueueFamilyIndex = 0;
    VkDevice logicalDevice = nullptr;
    VkQueue graphicsQueue = nullptr;
    VmaAllocator vmaAllocator = nullptr;
    VkFormat swapchainFormat = VK_FORMAT_B8G8R8A8_SRGB;
    VkSwapchainKHR swapchain = nullptr;
    VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
    VkImage depthImage = nullptr;
    VkImageView depthImageView = nullptr;
    VmaAllocation depthImageAllocation = nullptr;

    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    std::vector<VkSemaphore> renderCompleteSemaphores;
    bool requireSwapchainRecreate = false;

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
    bool createSwapchain(uint32_t width, uint32_t height);


  public:
    // Members
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = WIDTH/16 * 9;

    // Methods
    void run();
    Engine();
    ~Engine();

    void destroySwapchainResources();

};

#endif
