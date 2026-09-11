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
}

Core::~Core()
{
  vkDeviceWaitIdle(forge::arsenal::device);
  
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
  // [BLOCKING] [QUESTION: why presentDoneSemaphores indexed with frame inflight index and not swapchainIndex]
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
  vkEndCommandBuffer(forge::aether::frames::commandBuffers[inFlightIndex]);
  
  // [QUESTION]: why waiting for presentDoneSemaphores again? didnt we waited for it above in vkAcquireNextImageKHR
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
  VkSubmitInfo2 submitInfoV2
  {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
    .pNext = nullptr,
    .flags = 0,
    .waitSemaphoreInfoCount = 1,
    .pWaitSemaphoreInfos = drawWaitInfos,
    .commandBufferInfoCount = 1,
    .pCommandBufferInfos = drawCommandBufferInfos,
    .signalSemaphoreInfoCount = 2,
    .pSignalSemaphoreInfos = drawSignalInfos,
  };
  VkSubmitInfo2 empty
  {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
  };

PFN_vkQueueSubmit2KHR submit2KHR =
    reinterpret_cast<PFN_vkQueueSubmit2KHR>(
        vkGetDeviceProcAddr(
            forge::arsenal::device,
            "vkQueueSubmit2KHR"
        )
    );

std::cout << "vkQueueSubmit2KHR = "
          << reinterpret_cast<void*>(submit2KHR)
          << '\n';
VkSubmitInfo2KHR submit{
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2_KHR,
    .pNext = nullptr,
    .flags = 0,
    .waitSemaphoreInfoCount = 0,
    .pWaitSemaphoreInfos = nullptr,
    .commandBufferInfoCount = 0,
    .pCommandBufferInfos = nullptr,
    .signalSemaphoreInfoCount = 0,
    .pSignalSemaphoreInfos = nullptr
};

VkResult result = submit2KHR(
    forge::arsenal::graphicsQueue,
    1,
    &submit,
    VK_NULL_HANDLE
);


  //vkQueueSubmit2(forge::arsenal::graphicsQueue, 1, &empty, VK_NULL_HANDLE);
  /*
  uint64_t signalValues[] = {0, signalValue};
  VkTimelineSemaphoreSubmitInfoKHR timeSemaSubmit
  {
    .sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO_KHR,
    .pNext = nullptr,
    .waitSemaphoreValueCount = 0,
    .pWaitSemaphoreValues = nullptr,
    .signalSemaphoreValueCount = 2,
    .pSignalSemaphoreValues = signalValues,
  };
  VkSemaphore signalSemaphoresArray[] = {forge::aether::swap::renderDoneSemaphores[swapchainImageIndex], forge::aether::frames::timelineSemaphore};
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT};
  VkSubmitInfo submitInfoV1
  {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .pNext = &timeSemaSubmit,
    .waitSemaphoreCount = 1,
    .pWaitSemaphores = &forge::aether::frames::presentDoneSemaphores[inFlightIndex],
    .pWaitDstStageMask = waitStages,
    .commandBufferCount = 1,
    .pCommandBuffers = &forge::aether::frames::commandBuffers[inFlightIndex],
    .signalSemaphoreCount = 2,
    .pSignalSemaphores = signalSemaphoresArray 
  };
  vkQueueSubmit(forge::arsenal::graphicsQueue, 1, &submitInfoV1, VK_NULL_HANDLE);
  */
  
  // [QUESTION]: what semaphore does it suppose to signal after finishing
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
  // [Non-Blocking]
  vkQueuePresentKHR(forge::arsenal::graphicsQueue, &presentInfo);
}


