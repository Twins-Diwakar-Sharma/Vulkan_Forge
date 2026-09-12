#include "crypt.hpp"

namespace crypt 
{
  PFN_vkQueueSubmit2KHR vkQueueSubmit2KHR = VK_NULL_HANDLE;

  PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR = VK_NULL_HANDLE;
  PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR = VK_NULL_HANDLE;
  PFN_vkCmdPipelineBarrier2KHR vkCmdPipelineBarrier2KHR = VK_NULL_HANDLE;

  void necromancy()
  {
    if(forge::arsenal::device == VK_NULL_HANDLE)
    {
      scribe("sanctum: required arsenal from forge in order to manifest sanctum, it cannot be manifest from thin air");
      throw std::runtime_error("sanctum: evoke forge::arsenal first");
    }
    
    crypt::vkQueueSubmit2KHR = 
      reinterpret_cast<PFN_vkQueueSubmit2KHR>(
          vkGetDeviceProcAddr(
              forge::arsenal::device,
              "vkQueueSubmit2KHR"
          )
      );

    crypt::vkCmdBeginRenderingKHR = 
      reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(
          vkGetDeviceProcAddr(
              forge::arsenal::device,
              "vkCmdBeginRenderingKHR"
          )
      );
    
    crypt::vkCmdEndRenderingKHR = 
      reinterpret_cast<PFN_vkCmdEndRenderingKHR>(
          vkGetDeviceProcAddr(
              forge::arsenal::device,
              "vkCmdEndRenderingKHR"
          )
      );

    crypt::vkCmdPipelineBarrier2KHR = 
      reinterpret_cast<PFN_vkCmdPipelineBarrier2KHR>(
          vkGetDeviceProcAddr(
              forge::arsenal::device,
              "vkCmdPipelineBarrier2KHR"
          )
      );


  } // end necromancy
}
