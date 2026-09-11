#ifndef H_43THER_H
#define H_43THER_H

#include "scribe.hpp"
#include "arsenal.hpp"
#include "vk_mem_alloc.h"
namespace forge::aether
{
  namespace frames
  {
    static constexpr uint8_t in_flight = 2;
    extern VkSemaphore timelineSemaphore;
    extern uint64_t index;
    extern uint64_t nextSignalValue;
    extern VkCommandPool commandPools[in_flight];
    extern VkCommandBuffer commandBuffers[in_flight];
    extern VkSemaphore presentDoneSemaphores[in_flight];

    void evoke();
    void eradicate();
  }

  namespace swap
  {
    extern uint32_t width, height;
    extern VkSwapchainKHR swapchain;
    extern VkFormat colorFormat;
    extern std::vector<VkImage> images;
    extern std::vector<VkImageView> imageViews;
    extern std::vector<VkSemaphore> renderDoneSemaphores;
    extern uint32_t num_images; 

    extern VkFormat depthFormat;
    extern VkImage depthImage;
    extern VkImageView depthImageView;
    extern VmaAllocation depthImageAllocation;

    void evoke(GLFWwindow*);
    void eradicate();
  }
}

#endif
