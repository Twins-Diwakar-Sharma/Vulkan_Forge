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
      .apiVersion = VK_API_VERSION_1_0
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

  return true;
}

VkPhysicalDevice Engine::getPhysicalDevice()
{
  uint32_t physicalDeviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
  std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
  vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());
  
  bool descreteSelected = false;
  VkPhysicalDevice physicalDevice = nullptr;
  if(physicalDeviceCount)
  {
    physicalDevice = physicalDevices[0];
    for(auto &pDev : physicalDevices)
    {
      VkPhysicalDeviceProperties props{};
      vkGetPhysicalDeviceProperties(pDev, &props);
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

Engine::Engine()
{ 
  if(!createVulkanInstance())
  {
    std::cerr << "UNABLE TO CREATE Vulkan INSTANCE" << std::endl;
    initializationStatus = 404;
  }

  if(glfwCreateWindowSurface(instance,khidki.getWindowPointer(),nullptr, &surface) != VK_SUCCESS)
  {
    std::cerr << "UNABLE TO CREATE Vulkan SURFACE" << std::endl;
    initializationStatus = 404;
  }

  if(physicalDevice = getPhysicalDevice(); !physicalDevice)
  {
    std::cerr << "UNABLE TO CREATE Vulkan Physical Device" << std::endl;
    initializationStatus = 404;
  }
}

Engine::~Engine()
{
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
  if(initializationStatus != 200)
  {
    std::cerr << "Stopped because initialization failed, error code " << initializationStatus << std::endl;
    return;
  }
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
