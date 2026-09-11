#include "aether.hpp"

namespace forge::aether
{
  namespace frames
  {
    VkSemaphore timelineSemaphore = VK_NULL_HANDLE;
    uint64_t index = 0;
    uint64_t nextSignalValue = in_flight + 1;
    VkCommandPool commandPools[in_flight];
    VkCommandBuffer commandBuffers[in_flight];
    VkSemaphore presentDoneSemaphores[in_flight];
    
    void evoke()
    {
      scribe("evoking aether::frames");

      index = 0;
      nextSignalValue = in_flight + 1;

      // timelineSemaphore
      VkSemaphoreTypeCreateInfo semaTypeInfo
      {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
        .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE_KHR,
        .initialValue = in_flight
      };
      VkSemaphoreCreateInfo semaInfo
      {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = &semaTypeInfo,
      };
      if(vkCreateSemaphore(forge::arsenal::device, &semaInfo, nullptr, &timelineSemaphore) != VK_SUCCESS)
      {
        throw std::runtime_error("aether::frames: unable to create timeline semaphore");
      }
      
      // presentDoneSemaphores 
      for(unsigned int i=0; i<in_flight; i++)
      {
        VkSemaphoreCreateInfo presnetSemaInfo
        {
          .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };
        if(vkCreateSemaphore(forge::arsenal::device, &presnetSemaInfo, nullptr, presentDoneSemaphores + i ) != VK_SUCCESS)
        {
          throw std::runtime_error("aether::frames: unable to create presentationDone semaphore for frame index " + std::to_string(i));
        }
      }
      
      // command pool and buffer
      for(unsigned int i=0; i<in_flight; i++)
      {
        VkCommandPoolCreateInfo poolInfo
        {
          .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
          .queueFamilyIndex = forge::arsenal::graphicsQueueFamilyIndex
        };
        if(vkCreateCommandPool(forge::arsenal::device, &poolInfo, nullptr, commandPools + i) != VK_SUCCESS)
        {
          throw std::runtime_error("aether::frames: unable to create command pool for frame index " + std::to_string(i));
        }
        VkCommandBufferAllocateInfo cmdAllocInfo
        {
          .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
          .commandPool = commandPools[i],
          .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
          .commandBufferCount = 1,
        };
        if(vkAllocateCommandBuffers(forge::arsenal::device, &cmdAllocInfo, commandBuffers + i) != VK_SUCCESS)
        {
          throw std::runtime_error("aether::frames: unable to create command buffer for frame index " + std::to_string(i));
        }
      }
      scribe("aether::frames evoked");
    }

    void eradicate()
    {
      for(unsigned int i=0; i<in_flight; i++)
      {
        vkDestroyCommandPool(forge::arsenal::device, commandPools[i], nullptr); // also destroys command buffer
        vkDestroySemaphore(forge::arsenal::device, presentDoneSemaphores[i], nullptr);
      }
      vkDestroySemaphore(forge::arsenal::device, timelineSemaphore, nullptr);
      scribe("aether::frames eradicated");
    }

  } // end of frames
  
  namespace swap
  {

    uint32_t width=512, height=512;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkFormat colorFormat = VK_FORMAT_B8G8R8A8_SRGB;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    std::vector<VkSemaphore> renderDoneSemaphores;
    uint32_t num_images = 2;
    bool recreate = false;

    VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
    VkImage depthImage = VK_NULL_HANDLE;
    VkImageView depthImageView = VK_NULL_HANDLE;
    VmaAllocation depthImageAllocation = VK_NULL_HANDLE;

    void evoke(GLFWwindow* pGLFWwindow)
    {
      scribe("evoking aether::frames");
      num_images = 2;
      int w = 0, h= 0;
      glfwGetFramebufferSize(pGLFWwindow, &w, &h);
      swap::width = w;
      swap::height = h;

      VkSurfaceCapabilitiesKHR surfaceCaps{};
      if(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(forge::arsenal::physicalDevice, forge::arsenal::surface, &surfaceCaps) != VK_SUCCESS)
      {
        throw std::runtime_error("aether::swap: unable to get surface capabilities");
      }

      if(surfaceCaps.currentExtent.width == 0xFFFFFFFF) 
      {
        int i_width, i_height;
        glfwGetWindowSize(pGLFWwindow, &i_width, &i_height);
        swap::width = i_width;
        swap::height = i_height;
      }

      swap::width = std::clamp(swap::width, surfaceCaps.minImageExtent.width, surfaceCaps.maxImageExtent.width);
      swap::height = std::clamp(swap::height, surfaceCaps.minImageExtent.height, surfaceCaps.maxImageExtent.height);

      uint32_t requestedImageCount = std::max(num_images, surfaceCaps.minImageCount);
      if(surfaceCaps.maxImageCount > 0)
      {
        requestedImageCount = std::min(requestedImageCount, surfaceCaps.maxImageCount);
      }

      uint32_t formatCount = 0;
      vkGetPhysicalDeviceSurfaceFormatsKHR(forge::arsenal::physicalDevice, forge::arsenal::surface, &formatCount, nullptr);
      std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
      vkGetPhysicalDeviceSurfaceFormatsKHR(forge::arsenal::physicalDevice, forge::arsenal::surface, &formatCount, surfaceFormats.data());

      bool formatSupported = false;
      for (const VkSurfaceFormatKHR &surfFormat : surfaceFormats)
      {
        if(surfFormat.format == colorFormat)
        {
          formatSupported = true;
          break;
        }
      }
      
      if(!formatSupported)
      {
        throw std::runtime_error("aether::swap: unable to find requested color format for swapchain images");
      }


      VkSwapchainCreateInfoKHR swapchainCreateInfo
      {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = forge::arsenal::surface,
        .minImageCount = requestedImageCount,
        .imageFormat = swap::colorFormat,
        .imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR,
        .imageExtent{.width = swap::width, .height = swap::height},
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = surfaceCaps.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_MAILBOX_KHR
      };

      if (vkCreateSwapchainKHR(forge::arsenal::device, &swapchainCreateInfo, nullptr, &swapchain) != VK_SUCCESS)
      {
        throw std::runtime_error("aether::swap: unable to create swapchain"); 
      }

      uint32_t imageCount = 0;
      vkGetSwapchainImagesKHR(forge::arsenal::device, swapchain, &imageCount, nullptr);
      num_images = imageCount;
      images.resize(imageCount);
      vkGetSwapchainImagesKHR(forge::arsenal::device, swapchain, &imageCount, images.data());
      imageViews.resize(imageCount);

      scribe("Swap chain images count " + std::to_string(num_images));

        for(size_t i = 0; i < num_images; i++)
        {
          VkImageViewCreateInfo imgViewInfo
          {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = colorFormat,
            .subresourceRange
            {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .baseMipLevel = 0,
              .levelCount = 1,
              .baseArrayLayer = 0,
              .layerCount = 1
            }
          };
          if(vkCreateImageView(forge::arsenal::device, &imgViewInfo, nullptr, &imageViews[i]) != VK_SUCCESS)
          {
            throw std::runtime_error("aether::swap: unable to create image views");
          }
        }
      
        renderDoneSemaphores.resize(num_images);
        for(VkSemaphore &semaphore : renderDoneSemaphores)
        {
          VkSemaphoreCreateInfo semaphoreInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
          if(vkCreateSemaphore(forge::arsenal::device, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS)
          {
            throw std::runtime_error("aether::swap: unable to create renderDoneSemaphore");
          }
        }
      
  
        VkImageCreateInfo depthCreateInfo
        {
          .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
          .imageType = VK_IMAGE_TYPE_2D,
          .format = depthFormat,
          .extent{.width = swap::width, .height = swap::height, .depth = 1 },
          .mipLevels = 1,
          .arrayLayers = 1,
          .samples = VK_SAMPLE_COUNT_1_BIT,
          .tiling = VK_IMAGE_TILING_OPTIMAL,
          .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
          .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        VmaAllocationCreateInfo allocInfo
        {
          .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
          .usage = VMA_MEMORY_USAGE_AUTO
        };
        if(vmaCreateImage(forge::arsenal::vmaAllocator, &depthCreateInfo, &allocInfo, &depthImage, &depthImageAllocation, nullptr) != VK_SUCCESS)
        {
          throw std::runtime_error("VMA: Error allocating depth image");
        }

        VkImageViewCreateInfo depthImgViewInfo
        {
          .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
          .image = depthImage,
          .viewType = VK_IMAGE_VIEW_TYPE_2D,
          .format = depthFormat,
          .subresourceRange{
            .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT, 
            .levelCount = 1, 
            .layerCount = 1
          },
        };
        if(vkCreateImageView(forge::arsenal::device, &depthImgViewInfo, nullptr, &depthImageView) != VK_SUCCESS)
        {
          throw std::runtime_error("Error creating depth image view");
        }

        scribe("aether::frames evoked");
      }

      void eradicate()
      {
        for(unsigned int i=0; i<num_images; i++)
        {
          vkDestroyImageView(forge::arsenal::device, imageViews[i], nullptr);
        }
        imageViews.clear();

        for(unsigned int i=0; i<num_images; i++)
        {
          vkDestroySemaphore(forge::arsenal::device, renderDoneSemaphores[i], nullptr);
        }
        renderDoneSemaphores.clear();

        if(swapchain)
        {
          vkDestroySwapchainKHR(forge::arsenal::device, swapchain, nullptr);
          swapchain = nullptr;
        }

        if(depthImageView)
        {
          vkDestroyImageView(forge::arsenal::device, depthImageView, nullptr);
          vmaDestroyImage(forge::arsenal::vmaAllocator, depthImage, depthImageAllocation);
          depthImageView = nullptr;
        }
        
        scribe("aether::swap: eradictaed");
      }
    } // end of swap
}
