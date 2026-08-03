#define VMA_IMPLEMENTATION 
#include "Engine.hpp"

bool Engine::createVulkanInstance()
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
      .pEngineName = "Cosmic_Forge",
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

VkPhysicalDevice Engine::getPhysicalDevice()
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

bool Engine::getGraphicsQueue()
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

bool Engine::createLogicalDevice()
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

bool Engine::initializeVMA()
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

Engine::Engine()
{ 
  if(!createVulkanInstance())
  {
    throw std::runtime_error("unable to create Vulkan INSTANCE");
  }

  if(glfwCreateWindowSurface(instance,khidki.getWindowPointer(),nullptr, &surface) != VK_SUCCESS)
  {
    throw std::runtime_error("unable to create Vulkan SURFACE");
  }

  if(physicalDevice = getPhysicalDevice(); !physicalDevice)
  {
    throw std::runtime_error("unable to get Vulkan PHYSICAL DEVICE");
  }

  if(!getGraphicsQueue())
  {
    throw std::runtime_error("unable to get Vulkan GRAPHICS QUEUE");
  }

  if(!createLogicalDevice())
  {
    throw std::runtime_error("unable to get Vulkan LOGICAL DEVICE");
  }
  
  if(!initializeVMA())
  {
    throw std::runtime_error("unable to initialize Vulkan Memory allocator");
  }
  
}

Engine::~Engine()
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

}

void Engine::run()
{
  while (!khidki.shouldClose())
  {
    glfwPollEvents();
  }
}


VKAPI_ATTR VkBool32 VKAPI_CALL Engine::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    std::cerr << "Validation: " << pCallbackData->pMessage << '\n';
    return VK_FALSE;
}
