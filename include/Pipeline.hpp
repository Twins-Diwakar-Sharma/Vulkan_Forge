#ifndef H_PIPELINE_H
#define H_PIPELINE_H

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include "VulkanContext.hpp"

class Pipeline
{
  private:
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
    std::string name;
  public:
    void makePipeline(std::string name);
    // @ TODO: seperate name and shader specific info for
    // creation and rendering into a new class PipelineConfig
    // now pipeline will take config as input
    // make a seperate file namespace containing all configs,
    // use those objects as input for Pipeline
    // now you dont have to create new class for new pipeline,
    // instead create only new object for config in namespace, use that
    Pipeline();
    ~Pipeline();
    void render(VkCommandBuffer commandBuffer, uint32_t imageIndex);

  private:
    static std::vector<char> readFile(const std::string& filepath);
    VkShaderModule createShaderModule(const std::vector<char>& code);

    void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath);
};

#endif
