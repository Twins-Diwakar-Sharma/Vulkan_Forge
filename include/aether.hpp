#ifndef H_43THER_H
#define H_43THER_H

#include "scribe.hpp"
#include "arsenal.hpp"

namespace forge::aether
{
  namespace frames
  {
    static constexpr uint8_t max_in_flight = 2;
    extern VkSemaphore timelineSemaphore;
    extern uint64_t index;
    extern uint64_t nextSignalValue;
    extern VkCommandPool commandPools[max_in_flight];
    extern VkCommandBuffer commandBuffers[max_in_flight];
    extern VkSemaphore presentDoneSemaphores[max_in_flight];

    void evoke();
    void eradicate();
  }

  namespace swap
  {
    extern VkSwapchainKHR swapchain;
    extern VkFormat colorFormat;
    extern std::vector<VkImage> images;
    extern std::vector<VkImageView> imageViews;
    extern std::vector<VkSemaphore> renderDoneSemaphores;

    extern VkFormat depthFormat;
    extern VkImage depthImage;
    extern VkImageView depthImageView;
    extern VmaAllocation depthImageAllocation;

    void evoke();
    void eradicate();
  }
}

#endif
