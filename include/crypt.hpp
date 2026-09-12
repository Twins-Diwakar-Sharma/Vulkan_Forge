#ifndef H_SAnCtUM_H
#define H_SAnCtUM_H

#include "arsenal.hpp"
#include "scribe.hpp"

namespace crypt 
{
  extern PFN_vkQueueSubmit2KHR vkQueueSubmit2KHR;
  extern PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR;
  extern PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR;
  extern PFN_vkCmdPipelineBarrier2KHR vkCmdPipelineBarrier2KHR;

  void necromancy(); // only creation no destruction
}

#endif
