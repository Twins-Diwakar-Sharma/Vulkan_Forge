#define VMA_IMPLEMENTATION 
#include "VulkanContext.hpp"

namespace vulkancontext
{
  
  namespace {

    bool createVulkanInstance(const std::string& appName)
    {
      uint32_t glfwExtensionsCount = 0;
      std::vector<const char *> requestedExtensions
      {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
      };
      const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
      for(int i=0; i<glfwExtensionsCount; i++)
      {
        requestedExtensions.push_back(glfwExtensions[i]);
      }
      std::vector<const char *> requestedLayers
      {
        "VK_LAYER_KHRONOS_validation"
      };
      VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
      debugCreateInfo.sType =
          VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

      debugCreateInfo.messageSeverity =
          VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
          VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
          VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

      debugCreateInfo.messageType =
          VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
          VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
          VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

      debugCreateInfo.pfnUserCallback = debugCallback;
      debugCreateInfo.pUserData = nullptr;

      VkApplicationInfo appInfo
      {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
          .pApplicationName = appName.c_str(),
          .applicationVersion = VK_MAKE_VERSION(0,0,0),
          //.pVulkanContextName = "Cosmic_Forge", //error
          .engineVersion = VK_MAKE_VERSION(0, 0, 0),
          .apiVersion = vulkanApiVersion 
      };

      VkInstanceCreateInfo instCreateInfo
      {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = &debugCreateInfo,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = (uint32_t)requestedLayers.size(),
        .ppEnabledLayerNames = requestedLayers.data(),
        .enabledExtensionCount = (uint32_t)requestedExtensions.size(),
        .ppEnabledExtensionNames = requestedExtensions.data()
      };

      if(vkCreateInstance(&instCreateInfo, nullptr, &instance) != VK_SUCCESS)
      {
        return false;
      }

      //volkLoadInstance(instance); 
      return true;
    }

    VkPhysicalDevice selectPhysicalDevice()
    {
      uint32_t physicalDeviceCount = 0;
      vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
      std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
      vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

      std::cout << "Number of GPUs : " << physicalDevices.size() << std::endl;
      
      bool descreteSelected = false;
      VkPhysicalDevice physicalDevice = nullptr;
      if(physicalDeviceCount)
      {
        physicalDevice = physicalDevices[0];
        for(auto &pDev : physicalDevices)
        {
          VkPhysicalDeviceProperties props{};
          vkGetPhysicalDeviceProperties(pDev, &props);
          std::cout << " -- device name -- " << props.deviceName << std::endl;
          std::cout << " -- device type -- " << props.deviceType << std::endl;
          if(props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
          {
            physicalDevice = pDev;
            descreteSelected = true;
            break;
          }
        }
      }
      if(descreteSelected)
        std::cout << "Selected DESCRETE GPU" << std::endl;
      else
        std::cout << "Selected INTEGRATED GPU" << std::endl;
      

      return physicalDevice;
    }

    bool findGraphicsQueue()
    {
      uint32_t queueFamilyCount = 0;
      vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queueFamilyCount, nullptr);
      std::vector<VkQueueFamilyProperties2> queueFamilyProps(queueFamilyCount, 
          {.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2});
      vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queueFamilyCount, queueFamilyProps.data());
      for(int currentFamilyIdx = 0; currentFamilyIdx < queueFamilyCount; currentFamilyIdx++)
      {
        VkBool32 hasPresentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, currentFamilyIdx, surface, &hasPresentSupport);
        const auto &props = queueFamilyProps[currentFamilyIdx];
        if( (props.queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT ) && ( hasPresentSupport) )
        {
          graphicsQueueFamilyIndex = currentFamilyIdx;
          return true;
        }
      }
      return false;
    }

    bool createLogicalDevice()
    {
      VkPhysicalDeviceVulkan14Features supported14Features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES, .pNext = nullptr};
      VkPhysicalDeviceVulkan13Features supported13Features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, .pNext = &supported14Features};
      VkPhysicalDeviceVulkan12Features supported12Features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, .pNext = &supported13Features};

      VkPhysicalDeviceFeatures2 supportedFeatures{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = &supported12Features};
      vkGetPhysicalDeviceFeatures2(physicalDevice, &supportedFeatures);

      if(!supported13Features.dynamicRendering || !supported13Features.synchronization2 || !supported12Features.timelineSemaphore)
      {
        std::cerr << "Physical Device does not meet required features" << std::endl;
        return false;
      }

      // Now produces seperate list for features you will use, edit it to add more later
      // Directly using above instead, is a bad practice, it will enable everything and 
      // add features which we donot need in this game
      // NOTE: try to avoid NVIDIA or AMD specific features
      VkPhysicalDeviceVulkan14Features featuresBeingUsed14
      {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES, .pNext = nullptr
      };

      VkPhysicalDeviceVulkan13Features featuresBeingUsed13
      {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = &featuresBeingUsed14,
        .synchronization2 = VK_TRUE,
        .dynamicRendering = VK_TRUE,
      };

      VkPhysicalDeviceVulkan12Features featuresBeingUsed12
      {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .pNext = &featuresBeingUsed13,
        .timelineSemaphore = VK_TRUE,
      };

      VkPhysicalDeviceFeatures2 usingFeatures{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = &featuresBeingUsed12};

      // setting queue priorities
      std::vector<float> queuePriorities{1.0f};
      VkDeviceQueueCreateInfo graphicsQueueInfo
      {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = graphicsQueueFamilyIndex,
        .queueCount = 1,
        .pQueuePriorities = queuePriorities.data()
      };

      const std::vector<const char *> deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

      VkDeviceCreateInfo deviceCreateInfo
      {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &usingFeatures,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &graphicsQueueInfo,
        .enabledExtensionCount = (uint32_t)(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.data(),
        .pEnabledFeatures = nullptr
      };

      if(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice) != VK_SUCCESS)
      {
        return false;
      }

      vkGetDeviceQueue(logicalDevice, graphicsQueueFamilyIndex, 0, &graphicsQueue);
      if(!graphicsQueue)
      {
        std::cerr << "UNABLE TO CREATE GRAPHICS QUEUE" << std::endl;
        return false;
      }

      return true;
    }

    bool createVMA()
    {

      VmaVulkanFunctions vulkanFunctions = {};
       
      VmaAllocatorCreateInfo allocatorCreateInfo = {};
      allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
      allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_2;
      allocatorCreateInfo.physicalDevice = physicalDevice;
      allocatorCreateInfo.device = logicalDevice;
      allocatorCreateInfo.instance = instance;
      allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;
      
      if(vmaCreateAllocator(&allocatorCreateInfo, &vmaAllocator) != VK_SUCCESS)
      {
        return false;
      }
      return true;
    }

    bool createSwapchain(uint32_t width, uint32_t height)
    {
      swapchainWidth = width;
      swapchainHeight = height;

      VkSurfaceCapabilitiesKHR surfaceCaps{};
      if(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCaps) != VK_SUCCESS)
      {
        std::cerr << "Couldn't get surface capabilities for swapchain" << std::endl;
        return false;
      }
      
      uint32_t requestedImageCount = std::max(numSwapchainImages, surfaceCaps.minImageCount);
      if(surfaceCaps.maxImageCount > 0)
      {
        requestedImageCount = std::min(requestedImageCount, surfaceCaps.maxImageCount);
      }

      uint32_t formatCount = 0;
      vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
      std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
      vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());

      bool formatSupported = false;
      for (const VkSurfaceFormatKHR &surfFormat : surfaceFormats)
      {
        if(surfFormat.format == swapchainFormat)
        {
          formatSupported = true;
          break;
        }
      }
      
      if(!formatSupported)
      {
        std::cerr << "Requested format for swapchain: " << swapchainFormat << " not supported ny surface " << std::endl;
        return false;
      }

      VkSwapchainCreateInfoKHR swapchainCreateInfo
      {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = requestedImageCount,
        .imageFormat = swapchainFormat,
        .imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR,
        .imageExtent{.width = swapchainWidth, .height = swapchainHeight },
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = surfaceCaps.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_MAILBOX_KHR
      };

      if (vkCreateSwapchainKHR(logicalDevice, &swapchainCreateInfo, nullptr, &swapchain) != VK_SUCCESS)
      {
        std::cerr << "Error creating swapchain" << std::endl;
        return false;
      }

      uint32_t imageCount = 0;
      vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, nullptr);
      swapchainImages.resize(imageCount);
      vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, swapchainImages.data());
      swapchainImageViews.resize(imageCount);

      std::cout << "Swap chain images count " << imageCount << std::endl;

      for(size_t i = 0; i < swapchainImages.size(); i++)
      {
        VkImageViewCreateInfo imgViewInfo
        {
          .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
          .image = swapchainImages[i],
          .viewType = VK_IMAGE_VIEW_TYPE_2D,
          .format = swapchainFormat,
          .subresourceRange
          {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
          }
        };

        if(vkCreateImageView(logicalDevice, &imgViewInfo, nullptr, &swapchainImageViews[i]) != VK_SUCCESS)
        {
          std::cerr << " Error creating swap chain image views" << std::endl;
          return false;
        }
      }
      
      renderCompleteSemaphores.resize(imageCount);
      for(VkSemaphore &semaphore : renderCompleteSemaphores)
      {
        VkSemaphoreCreateInfo semaphoreInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        if(vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS)
        {
          std::cerr << "Error creating render-complete semaphores" << std::endl;
          return false;
        }
      }
      
      VkImageCreateInfo depthCreateInfo
      {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = depthFormat,
        .extent{.width = swapchainWidth, .height = swapchainHeight, .depth = 1 },
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
      if(vmaCreateImage(vmaAllocator, &depthCreateInfo, &allocInfo, &depthImage, &depthImageAllocation, nullptr) != VK_SUCCESS)
      {
        std::cerr << "VMA: Error allocating depth image" << std::endl;
        return false;
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
      if(vkCreateImageView(logicalDevice, &depthImgViewInfo, nullptr, &depthImageView) != VK_SUCCESS)
      {
        std::cerr << "Error creating depth image view" << std::endl;
        return false;
      }
      return true;
    }



    void destroySwapchainResources()
    {
      for(unsigned int i=0; i<swapchainImageViews.size(); i++)
      {
        vkDestroyImageView(logicalDevice, swapchainImageViews[i], nullptr);
      }
      swapchainImageViews.clear();

      for(unsigned int i=0; i<renderCompleteSemaphores.size(); i++)
      {
        vkDestroySemaphore(logicalDevice, renderCompleteSemaphores[i], nullptr);
      }
      renderCompleteSemaphores.clear();

      if(swapchain)
      {
        vkDestroySwapchainKHR(logicalDevice, swapchain, nullptr);
        swapchain = nullptr;
      }

      if(depthImageView)
      {
        vkDestroyImageView(logicalDevice, depthImageView, nullptr);
        vmaDestroyImage(vmaAllocator, depthImage, depthImageAllocation);
        depthImageView = nullptr;
      }
    }


  }

  
  // PUBLIC
  uint32_t vulkanApiVersion = VK_API_VERSION_1_4;
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

  // methods
  void initialize(const std::string* appName, GLFWwindow* glfwWindowPointer, uint32_t width, uint32_t height)
  { 
    if(appName == nullptr || glfwWindowPointer == nullptr) 
    {
      throw std::runtime_error("Error: have you called initialize(..) before getting context ? ");
    }
    if(!createVulkanInstance(*appName))
    {
      throw std::runtime_error("unable to create Vulkan INSTANCE");
    }

    if(glfwCreateWindowSurface(instance,glfwWindowPointer,nullptr, &surface) != VK_SUCCESS)
    {
      throw std::runtime_error("unable to create Vulkan SURFACE");
    }

    if(physicalDevice = selectPhysicalDevice(); !physicalDevice)
    {
      throw std::runtime_error("unable to get Vulkan PHYSICAL DEVICE");
    }

    if(!findGraphicsQueue())
    {
      throw std::runtime_error("unable to get Vulkan GRAPHICS QUEUE");
    }

    if(!createLogicalDevice())
    {
      throw std::runtime_error("unable to get Vulkan LOGICAL DEVICE");
    }
    
    if(!createVMA())
    {
      throw std::runtime_error("unable to initialize Vulkan Memory allocator");
    }

    if(!createSwapchain(width,height))
    {
      throw std::runtime_error("unable to create swapchain images");
    }
    
    std::cout << "VulkanContext created" << std::endl;
  }

  VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
      VkDebugUtilsMessageTypeFlagsEXT messageType,
      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
      void* pUserData)
  {
      std::cerr << "Validation: " << pCallbackData->pMessage << '\n';
      return VK_FALSE;
  }

  void destroy()
  {
    destroySwapchainResources();

    vmaDestroyAllocator(vmaAllocator);
    if(surface)
    {
      vkDestroySurfaceKHR(instance, surface, nullptr);
    }
    if(instance)
    {
      vkDestroyInstance(instance, nullptr);
    }
    std::cout << "VulkanContext destroyed" << std::endl;
  }
}

