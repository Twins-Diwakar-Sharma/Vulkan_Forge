
#include "Engine.hpp"

Engine::Engine() 
{ 
  vulkancontext::initialize(&appName, khidki.getWindowPointer(), (uint32_t)WIDTH, (uint32_t)HEIGHT);
  pipeline.makePipeline( "simple");
}

Engine::~Engine()
{
  vulkancontext::destroy();
}

void Engine::run()
{
  while (!khidki.shouldClose())
  {
    input();
    update();
    render();
  }
}

void Engine::update()
{
  if(requireSwapchainRecreate)
  {
    vkDeviceWaitIdle(vulkancontext::logicalDevice);
    vulkancontext::destroySwapchainResources();
    vulkancontext::createSwapchain(khidki.getWindowPointer());
    requireSwapchainRecreate = false;
  }
}

void Engine::input()
{
  glfwPollEvents();
}

void Engine::render()
{
//  const uint32_t frameResIndex = frameIndex++ % vulkan::MaxFramesInFlight; // not working using macro

  const uint64_t frameResIndex = frameIndex++ % vulkancontext_MaxFramesInFlight;
  const uint64_t signalValue = nextSignalValue++;
  const uint64_t waitValue = signalValue - vulkancontext_MaxFramesInFlight; // wait for prev frame to finish

  VkSemaphoreWaitInfo waitInfo
  {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
    .semaphoreCount = 1,
    .pSemaphores = &vulkancontext::timelineSemaphore,
    .pValues = &waitValue
  };
  vkWaitSemaphores(vulkancontext::logicalDevice, &waitInfo, UINT64_MAX);
  
  // Now start recording commands
  FrameResource& res = vulkancontext::frameResources[frameResIndex];
  vkResetCommandPool(vulkancontext::logicalDevice, res.commandPool, 0);
  VkSemaphore imageAcquiredSemaphore = vulkancontext::frameResources[frameResIndex].imageAcquiredSemaphore;

  uint32_t imageIndex = 0;
  VkResult acquireResult = vkAcquireNextImageKHR(vulkancontext::logicalDevice, vulkancontext::swapchain, UINT64_MAX, imageAcquiredSemaphore, VK_NULL_HANDLE, &imageIndex);
  
  if(acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
  {
    requireSwapchainRecreate = true;
  }
  
  VkCommandBufferBeginInfo cmdBeginInfo
  {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
  };
  vkBeginCommandBuffer(res.commandBuffer, &cmdBeginInfo);
  // render calls // pipeline.render()
  vkEndCommandBuffer(res.commandBuffer);
  
  VkPresentInfoKHR presentInfo
  {
    .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    .waitSemaphoreCount = 0, //1
    .pWaitSemaphores = nullptr, // & renderCompleteSemaphores[imageIndex]
    .swapchainCount = 1,
    .pSwapchains = &vulkancontext::swapchain,
    .pImageIndices = &imageIndex,
    .pResults = nullptr
  };
  vkQueuePresentKHR(vulkancontext::graphicsQueue, &presentInfo);
}
