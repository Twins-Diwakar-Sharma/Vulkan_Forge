#include "sanctum.hpp"

namespace sanctum
{
  namespace pipesouls
  {
    PipeSoul simple
    {
      .name = std::string("simple"),
      .setLayoutCount = 0,
      .pushConstantRangeCount = 0,
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      .blendEnable = VK_FALSE,
    };
  }
}
