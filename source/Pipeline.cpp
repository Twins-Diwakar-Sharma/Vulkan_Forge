#include "Pipeline.hpp"

std::vector<char> Pipeline::readFile(const std::string& filePath) 
{

    std::ifstream file(filePath, std::ios::ate | std::ios::binary);
    
    if(!file.is_open()) 
    {
      throw std::runtime_error("failed to open file:" + filePath);
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}

VkShaderModule Pipeline::createShaderModule(const std::vector<char>& code)
{
  VkShaderModuleCreateInfo createInfo
  {
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .codeSize = code.size(),
    .pCode = (uint32_t*)(code.data())
  };
  VkShaderModule shaderModule;
  if(vkCreateShaderModule(vulkancontext::logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to create shader module!"); 
  }
  return shaderModule;
}

void Pipeline::createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath)
{
  std::vector<char> vertShaderCode = readFile(vertFilePath);
  std::vector<char> fragShaderCode = readFile(fragFilePath);
  VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
  VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);
  
  VkPipelineLayoutCreateInfo pipelineLayoutInfo
  {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .setLayoutCount = 0,
    .pushConstantRangeCount = 0
  };
  
  if(vkCreatePipelineLayout(vulkancontext::logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
  {
    std::cout << "Unable to create pipeline layout" << std::endl; 
    return;
  }

  const char* entryPoint = "main";
  std::vector<VkPipelineShaderStageCreateInfo> shaderStages
  {
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .module = vertShaderModule,
      .pName = entryPoint
    },
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = fragShaderModule,
      .pName = entryPoint
    }
  };

  VkPipelineVertexInputStateCreateInfo vertInputInfo
  {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
  };

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo
  {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
  };

  VkPipelineDepthStencilStateCreateInfo depthStencilInfo
  {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
    .depthTestEnable = VK_TRUE,
    .depthWriteEnable = VK_TRUE,
    .depthCompareOp = VK_COMPARE_OP_LESS,
    .stencilTestEnable = VK_FALSE
  };

  VkPipelineViewportStateCreateInfo viewportInfo
  {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .viewportCount = 1,
    .pViewports = nullptr, // dynamic
    .scissorCount = 1,
    .pScissors = nullptr // dynamic
  };

  VkPipelineRasterizationStateCreateInfo rasterInfo
  {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .polygonMode = VK_POLYGON_MODE_FILL,
    .cullMode = VK_CULL_MODE_BACK_BIT,
    .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
    .lineWidth = 1.0f,
  };

  VkPipelineMultisampleStateCreateInfo multiSampleInfo
  {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT
  };

  VkPipelineColorBlendAttachmentState blendAttachState
  {
    .blendEnable = VK_FALSE,
    .colorWriteMask = VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_A_BIT,
  };

  VkPipelineColorBlendStateCreateInfo blendInfo
  {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .attachmentCount = 1,
    .pAttachments = &blendAttachState
  };

  std::vector<VkDynamicState> dynamicStates
  {
    VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
  };
  VkPipelineDynamicStateCreateInfo dynamicStateInfo
  {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    .dynamicStateCount = (uint32_t)dynamicStates.size(),
    .pDynamicStates = dynamicStates.data()
  };

  VkPipelineRenderingCreateInfo renderInfo
  {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
    .colorAttachmentCount = 1,
    .pColorAttachmentFormats = &vulkancontext::swapchainFormat,
    .depthAttachmentFormat = vulkancontext::depthFormat,
  };
  
  // summary
  VkGraphicsPipelineCreateInfo pipelineInfo
  {
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .pNext = &renderInfo,
    .stageCount = (uint32_t)shaderStages.size(),
    .pStages = shaderStages.data(),
    .pVertexInputState = &vertInputInfo,
    .pInputAssemblyState = &inputAssemblyInfo,
    .pViewportState = &viewportInfo,
    .pRasterizationState = &rasterInfo,
    .pMultisampleState = &multiSampleInfo,
    .pDepthStencilState = &depthStencilInfo,
    .pColorBlendState = &blendInfo,
    .pDynamicState = &dynamicStateInfo,
    .layout = pipelineLayout,
    .renderPass = VK_NULL_HANDLE,
  };
  
  if(vkCreateGraphicsPipelines(vulkancontext::logicalDevice, nullptr, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
  {
    throw std::runtime_error("unable to create vulkan graphics pipeline"); 
  }
  vkDestroyShaderModule(vulkancontext::logicalDevice, fragShaderModule, nullptr);
  vkDestroyShaderModule(vulkancontext::logicalDevice, vertShaderModule, nullptr);
}

void Pipeline::makePipeline(std::string name)
{
  this->name = name;
  std::string vertFilePath = "shaders/" + name + ".vert.spv";
  std::string fragFilePath = "shaders/" + name + ".frag.spv";
  createGraphicsPipeline(vertFilePath, fragFilePath);
}

Pipeline::Pipeline()
{}
Pipeline::~Pipeline()
{}

void Pipeline::render(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
  std::vector<VkImageMemoryBarrier2> layoutBarriers
  {
    {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
      .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
      .srcAccessMask = 0,
      .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
      .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .image = vulkancontext::swapchainImages[imageIndex],
      .subresourceRange
      {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      }
    },
    {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
      .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
      .srcAccessMask = 0,
      .dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
      .dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .newLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
      .image = vulkancontext::depthImage,
      .subresourceRange
      {
        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      }
    }
  };

  VkDependencyInfo dependInfo
  {
    .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
    .imageMemoryBarrierCount = (uint32_t)(layoutBarriers.size()),
    .pImageMemoryBarriers = layoutBarriers.data()
  };
  
  VkRenderingAttachmentInfo colorAttachInfo
  {
    .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
    .imageView = vulkancontext::swapchainImageViews[imageIndex],
    .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
    .clearValue{.color{0.93f, 0.9f, 0.9f, 1}}
  };

  VkRenderingAttachmentInfo depthAttachmentInfo
  {
    .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
    .imageView = vulkancontext::depthImageView,
    .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
    .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
    .clearValue{.depthStencil{1.0f, 0}}
  };

  VkRenderingInfo renderingInfo
  {
    .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
    .renderArea
    {
      .offset{.x = 0, .y = 0},
      .extent{.width = vulkancontext::swapchainWidth, .height = vulkancontext::swapchainHeight}
    },
    .layerCount = 1,
    .colorAttachmentCount = 1,
    .pColorAttachments = &colorAttachInfo,
    .pDepthAttachment = &depthAttachmentInfo
  };

  vkCmdBeginRendering(commandBuffer, &renderingInfo);
  VkViewport viewport
  {
    .x = 0, .y = 0,
    .width = (float)(vulkancontext::swapchainWidth),
    .height = (float)(vulkancontext::swapchainHeight)
  };
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
  VkRect2D scissor
  {
    .offset{.x = 0, .y = 0},
    .extent{.width = vulkancontext::swapchainWidth, .height = vulkancontext::swapchainHeight}
  };
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
  vkCmdDraw(commandBuffer, 3, 1, 0, 0);
  
  vkCmdEndRendering(commandBuffer);
  // below might come in main engine::render() 
  VkImageMemoryBarrier2 presentLayoutBarrier
  {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
    .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
    .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
    .dstStageMask = VK_PIPELINE_STAGE_2_NONE,
    .dstAccessMask = 0,
    .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    .image = vulkancontext::swapchainImages[imageIndex],
    .subresourceRange
    {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .baseMipLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
    }
  };
  VkDependencyInfo presentDependInfo
  {
    .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
    .imageMemoryBarrierCount = 1,
    .pImageMemoryBarriers = &presentLayoutBarrier
  };
  vkCmdPipelineBarrier2(commandBuffer, &presentDependInfo);
}
