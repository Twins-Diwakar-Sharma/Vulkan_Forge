#ifndef H_4143_H
#define H_4143_H

#include "scribe.hpp"
#include "arsenal.hpp"
#include "aether.hpp"
#include "crypt.hpp"
#include <fstream>

struct PipeSoul
{
  std::string name; 
  uint32_t setLayoutCount;
  uint32_t pushConstantRangeCount;
  VkPrimitiveTopology topology;
  VkBool32 blendEnable;
};

class Pipe
{
private:
  PipeSoul* soul;
  VkPipelineLayout pipelineLayout;
  VkPipeline pipeline;
  VkShaderModule createShaderModule(const std::string& filePath);
public:
  Pipe();
  ~Pipe();
  void ensoul(PipeSoul* soul);
  void draw(uint32_t inFlightIndex, uint32_t swapchainImageIndex);
};

#endif
