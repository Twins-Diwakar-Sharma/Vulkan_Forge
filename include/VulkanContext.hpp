#ifndef H_VULK4N_C0NT3XT_H
#define H_VULK4N_C0NT3XT_H

#include <vector>
#include <vulkan/vulkan.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include "vk_mem_alloc.h"
#include <GLFW/glfw3.h>

struct FrameResource
{
  VkCommandPool commandPool = nullptr;
  VkCommandBuffer commandBuffer = nullptr;
  VkSemaphore imageAcquiredSemaphore = nullptr;
};

namespace vulkancontext
{
  extern uint32_t vulkanApiVersion;
  extern uint32_t swapchainWidth, swapchainHeight;
  extern uint32_t numSwapchainImages;

  extern VkInstance instance;
  extern VkSurfaceKHR surface;
  extern VkPhysicalDevice physicalDevice;
  extern uint32_t graphicsQueueFamilyIndex;
  extern VkDevice logicalDevice;
  extern VkQueue graphicsQueue;
  extern VmaAllocator vmaAllocator;
  extern VkFormat swapchainFormat;
  extern VkSwapchainKHR swapchain;
  extern VkFormat depthFormat;
  extern VkImage depthImage;
  extern VkImageView depthImageView;
  extern VmaAllocation depthImageAllocation;

  extern std::vector<VkImage> swapchainImages;
  extern std::vector<VkImageView> swapchainImageViews;
  extern std::vector<VkSemaphore> renderCompleteSemaphores;
  
  extern VkSemaphore timelineSemaphore;
  #define vulkancontext_MaxFramesInFlight 2
  extern FrameResource frameResources[vulkancontext_MaxFramesInFlight]; 

  // Methods public
  static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
      VkDebugUtilsMessageTypeFlagsEXT messageType,
      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
      void* pUserData);

  void initialize(const std::string* appName, GLFWwindow* glfwWindowPointer, uint32_t width, uint32_t height);
  bool createSwapchain(GLFWwindow* glfwWindowPointer);
  void destroySwapchainResources();
  void destroy();

  // Methods private = unnamed nested namespace they are all in cpp not in header remember that, here is just a list
  /*
  namespace
  {
    bool createVulkanInstance(const std::string& appName);
    bool createWindowSurface();
    VkPhysicalDevice selectPhysicalDevice();
    bool findGraphicsQueue();
    bool createLogicalDevice();
    bool createVMA();
    bool createOtherSemaphores();
    bool createCommandBuffers();
  }
*/
}

#endif
