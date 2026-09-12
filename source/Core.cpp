#include "Core.hpp"

int main()
{
  Core core;
  core.pump();
}

Core::Core()
{
  forge::arsenal::evoke(&appName, win.getWindowPointer(), WIDTH, HEIGHT);
  forge::aether::frames::evoke();
  forge::aether::swap::evoke(win.getWindowPointer());
  crypt::necromancy();

  simplePipe.ensoul(&sanctum::pipesouls::simple);
}

Core::~Core()
{
  vkDeviceWaitIdle(forge::arsenal::device);
 
  // crypt: cannot kill the dead
  forge::aether::swap::eradicate();
  forge::aether::frames::eradicate();
  forge::arsenal::eradicate();
}

void Core::pump()
{
  while(!win.shouldClose())
  {
    input();
    update();
    render();
  }
}

void Core::input()
{
  win.handleKey(); 
  glfwPollEvents();
}


void Core::update()
{
  if(forge::aether::swap::recreate)
  {
    vkDeviceWaitIdle(forge::arsenal::device);
    forge::aether::swap::recreate = false;
    forge::aether::swap::eradicate();
    forge::aether::swap::evoke(win.getWindowPointer());
  }
}


void Core::render()
{
  const uint64_t inFlightIndex = forge::aether::frames::index++ % forge::aether::frames::in_flight;
  const uint64_t signalValue = forge::aether::frames::nextSignalValue++;
  const uint64_t waitValue = signalValue - forge::aether::frames::in_flight;

  // wait for prev (n - in_flight) frames work to finish
  // only then we will use its command buffers
  VkSemaphoreWaitInfo timeWaitInfo
  {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
    .semaphoreCount = 1,
    .pSemaphores = &forge::aether::frames::timelineSemaphore,
    .pValues = &waitValue
  };

  // [BLOCKING]
  vkWaitSemaphores(forge::arsenal::device, &timeWaitInfo, UINT64_MAX);  

  // we can now record commands as command buffer is free
  vkResetCommandPool(forge::arsenal::device, forge::aether::frames::commandPools[inFlightIndex], 0);
  
  // check if presentation is done
  uint32_t swapchainImageIndex = 0;
  // [Non-Blocking GPU, BLOCKING CPU??], it does 2 things
  // bloking: waits for swapchain image to be acquired by cpu
  // non blocking sends a semaphore to GPU, this semaphore will be signaled in future when presentation is done
  VkResult acquireResult = vkAcquireNextImageKHR(forge::arsenal::device, forge::aether::swap::swapchain, UINT64_MAX, forge::aether::frames::presentDoneSemaphores[inFlightIndex], VK_NULL_HANDLE, &swapchainImageIndex);

  if(acquireResult == VK_ERROR_OUT_OF_DATE_KHR) // cannot render this frame, recreate this
  { 
    forge::aether::swap::recreate = true;
    return;
  }
  else if(acquireResult == VK_SUBOPTIMAL_KHR) // can render this frame, recreate next
  {
    forge::aether::swap::recreate = true;
  }
  
  VkCommandBufferBeginInfo cmdBeginInfo
  {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
  };
  // [Non-Blocking]
  vkBeginCommandBuffer(forge::aether::frames::commandBuffers[inFlightIndex], &cmdBeginInfo);
  simplePipe.draw(inFlightIndex, swapchainImageIndex);
  vkEndCommandBuffer(forge::aether::frames::commandBuffers[inFlightIndex]);
  
  VkSemaphoreSubmitInfoKHR drawWaitInfos[]
  {
    {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
      .pNext = nullptr,
      .semaphore = forge::aether::frames::presentDoneSemaphores[inFlightIndex],
      .value = 0,
      .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
      .deviceIndex = 0
    }
  };
  VkCommandBufferSubmitInfo drawCommandBufferInfos[]
  {
    {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
      .pNext = nullptr,
      .commandBuffer = forge::aether::frames::commandBuffers[inFlightIndex],
      .deviceMask = 0
    }
  };
  VkSemaphoreSubmitInfo drawSignalInfos[]
  {
    {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
      .pNext = nullptr,
      .semaphore = forge::aether::swap::renderDoneSemaphores[swapchainImageIndex],
      .value = 0,
      .stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
    },
    {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
      .pNext = nullptr,
      .semaphore = forge::aether::frames::timelineSemaphore,
      .value = signalValue,
      .stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
    },
  };
  
  VkSubmitInfo2KHR submitInfoV2
  {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2_KHR,
    .pNext = nullptr,
    .flags = 0,
    .waitSemaphoreInfoCount = 1,
    .pWaitSemaphoreInfos = drawWaitInfos,
    .commandBufferInfoCount = 1,
    .pCommandBufferInfos = drawCommandBufferInfos,
    .signalSemaphoreInfoCount = 2,
    .pSignalSemaphoreInfos = drawSignalInfos,
  };
  
  // [Non-Blocking] will continue to draw
  crypt::vkQueueSubmit2KHR(forge::arsenal::graphicsQueue, 1, &submitInfoV2, VK_NULL_HANDLE);
  
  // [QUESTION]: what semaphore does it suppose to signal after finishing
  // [ANSWER]: none, vkAcquireNextImageKHR will signal the semaphore instead
  // so they both must be used one after another
  VkPresentInfoKHR presentInfo
  {
    .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    .waitSemaphoreCount = 1,
    .pWaitSemaphores = &forge::aether::swap::renderDoneSemaphores[swapchainImageIndex],
    .swapchainCount = 1,
    .pSwapchains = &forge::aether::swap::swapchain,
    .pImageIndices = &swapchainImageIndex,
    .pResults = nullptr
  };
  // [Non-Blocking] GPU will wait for semaphore above 
  vkQueuePresentKHR(forge::arsenal::graphicsQueue, &presentInfo);
}


