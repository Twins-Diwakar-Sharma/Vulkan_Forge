#define VMA_IMPLEMENTATION 
#include "arsenal.hpp"
namespace forge::arsenal
{
  namespace 
  {
    uint8_t createVulkanInstance(const std::string& appName)
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
      uint32_t size_requestedLayers = 1;
      const char * requestedLayers[]
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
        .enabledLayerCount = size_requestedLayers,
        .ppEnabledLayerNames = requestedLayers,
        .enabledExtensionCount = (uint32_t)requestedExtensions.size(),
        .ppEnabledExtensionNames = requestedExtensions.data()
      };

      if(vkCreateInstance(&instCreateInfo, nullptr, &instance) != VK_SUCCESS)
      {
        return arsenal::failure;
      }

      //volkLoadInstance(instance); 
      return arsenal::success;
    }

    void selectPhysicalDevice()
    {
      uint32_t physicalDeviceCount = 0;
      vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

      std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
      vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());
      
      scribe("Number of GPUs : "  + std::to_string(physicalDevices.size()) );
      
      bool descreteSelected = false;
      VkPhysicalDeviceProperties props{};
      if(physicalDeviceCount)
      {
        physicalDevice = physicalDevices[0];
        for(auto &pDev : physicalDevices)
        {
          //VkPhysicalDeviceProperties props{};
          vkGetPhysicalDeviceProperties(pDev, &props);
          scribe(" -- device name -- " + std::string(props.deviceName) );;
          if(props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
          {
            physicalDevice = pDev;
            descreteSelected = true;

            break;
          }
        }
      }
      scribe(" -- forcing to select llvpipewire -- ");
      physicalDevice = physicalDevices[1];
      if(descreteSelected)
      {
        scribe("Selected DESCRETE GPU");
      }
      else
      {
        scribe("Selected INTEGRATED GPU");
      }

        std::cout << "------ yahan pe-----------" << std::endl;
std::cout
    << "Device Vulkan API version: "
    << VK_VERSION_MAJOR(props.apiVersion) << "."
    << VK_VERSION_MINOR(props.apiVersion) << "."
    << VK_VERSION_PATCH(props.apiVersion)
    << '\n';

    }

    uint8_t findGraphicsQueue()
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
          return arsenal::success;
        }
      }
      return arsenal::failure;
    }

    uint8_t createLogicalDevice()
    {
      VkPhysicalDeviceVulkan14Features supported14Features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES, .pNext = nullptr};
      VkPhysicalDeviceVulkan13Features supported13Features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, .pNext = &supported14Features};
      VkPhysicalDeviceVulkan12Features supported12Features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, .pNext = &supported13Features};

      VkPhysicalDeviceFeatures2 supportedFeatures{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = &supported12Features};
      vkGetPhysicalDeviceFeatures2(physicalDevice, &supportedFeatures);

      if(!supported13Features.dynamicRendering || !supported13Features.synchronization2 || !supported12Features.timelineSemaphore)
      {
        scribe("Physical Device does not meet required features");
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

      if(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS)
      {
        return arsenal::failure;
      }
  std::cout << "---- yahan pe 2 -----" << std::endl;
auto pSubmit2 =
    vkGetDeviceProcAddr(
        device,
        "vkQueueSubmit2"
    );

std::cout << "device vkQueueSubmit2 = "
          << reinterpret_cast<void*>(pSubmit2)
          << '\n';
auto pGetQueue =
    vkGetDeviceProcAddr(device, "vkGetDeviceQueue");

auto pQueueSubmit =
    vkGetDeviceProcAddr(device, "vkQueueSubmit");

auto pQueueSubmit2 =
    vkGetDeviceProcAddr(device, "vkQueueSubmit2");

auto pQueueWaitIdle =
    vkGetDeviceProcAddr(device, "vkQueueWaitIdle");

std::cout << "vkGetDeviceQueue  = "
          << reinterpret_cast<void*>(pGetQueue) << '\n';

std::cout << "vkQueueSubmit     = "
          << reinterpret_cast<void*>(pQueueSubmit) << '\n';

std::cout << "vkQueueSubmit2    = "
          << reinterpret_cast<void*>(pQueueSubmit2) << '\n';

std::cout << "vkQueueWaitIdle   = "
          << reinterpret_cast<void*>(pQueueWaitIdle) << '\n';



      return arsenal::success;
    }

    uint8_t getGraphicsQueuefromDevice()
    {
      vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);
      if(graphicsQueue == VK_NULL_HANDLE)
      {
        return arsenal::failure;
      }

      return arsenal::success;
    }

    uint8_t createVMA()
    {
      VmaVulkanFunctions vulkanFunctions = {};
      VmaAllocatorCreateInfo allocatorCreateInfo = {};
      allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
      allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_2;
      allocatorCreateInfo.physicalDevice = physicalDevice;
      allocatorCreateInfo.device = device;
      allocatorCreateInfo.instance = instance;
      allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;
      
      if(vmaCreateAllocator(&allocatorCreateInfo, &vmaAllocator) != VK_SUCCESS)
      {
        return arsenal::failure;
      }
      return arsenal::success;
    }

  } // end of unnamed namespace, private
  

  VkInstance instance = VK_NULL_HANDLE;
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  uint32_t graphicsQueueFamilyIndex = 0;
  VkDevice device = VK_NULL_HANDLE;
  VkQueue graphicsQueue = VK_NULL_HANDLE;
  VmaAllocator vmaAllocator = VK_NULL_HANDLE;

  VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
      VkDebugUtilsMessageTypeFlagsEXT messageType,
      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
      void* pUserData)
  {
      scribe("Validation: " + std::string(pCallbackData->pMessage) );
      return VK_FALSE;
  }



  void evoke(const std::string* appName, GLFWwindow* pGLFWwindow, uint32_t width, uint32_t height)
  {
    if(appName == nullptr || pGLFWwindow == nullptr)
    {
      throw std::runtime_error("arsenal: Window have not been created yet");
    }

    if(createVulkanInstance(*appName) != arsenal::success)
    {
      throw std::runtime_error("arenal: unable to create Vulkan INSTANCE");
    }

    if(glfwCreateWindowSurface(instance,pGLFWwindow,nullptr, &surface) != VK_SUCCESS)
    {
      throw std::runtime_error("arsenal: unable to create Vulkan SURFACE");
    }

    if(selectPhysicalDevice(); !physicalDevice)
    {
      throw std::runtime_error("arsenal: unable to get Vulkan PHYSICAL DEVICE");
    }

    if(findGraphicsQueue() != arsenal::success)
    {
      throw std::runtime_error("arsenal: unable to get Vulkan GRAPHICS QUEUE");
    }

    if(createLogicalDevice() != arsenal::success)
    {
      throw std::runtime_error("arsenal: unable to get Vulkan LOGICAL DEVICE");
    }

    if(getGraphicsQueuefromDevice() != arsenal::success)
    {
      throw std::runtime_error("arsenal: unable to get graphics queue family index");
    }
    
    if(createVMA() != arsenal::success)
    {
      throw std::runtime_error("arsenal: unable to initialize Vulkan Memory allocator");
    }

  }

  void eradicate()
  {
    vmaDestroyAllocator(vmaAllocator);
    if(surface)
    {
      vkDestroySurfaceKHR(instance, surface, nullptr);
    }
    if(instance)
    {
      vkDestroyInstance(instance, nullptr);
    }
    scribe("arenal eradicated");
  }
}
