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
    Pipeline();
    ~Pipeline();

  private:
    static std::vector<char> readFile(const std::string& filepath);
    VkShaderModule createShaderModule(const std::vector<char>& code);

    void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath);
};

#endif
