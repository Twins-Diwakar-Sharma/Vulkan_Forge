#include "Pipe.hpp"

Pipe::Pipe()
{}

Pipe::~Pipe()
{
  if(soul != nullptr)
    soul = nullptr; // donot delete yourself, only external, souls always remain
}

VkShaderModule Pipe::createShaderModule(const std::string& filePath)
{
  std::ifstream file(filePath, std::ios::ate | std::ios::binary);
  if(!file.is_open()) 
  {
    throw std::runtime_error("Pipe: failed to open file: " + filePath);
  }
  size_t fileSize = static_cast<size_t>(file.tellg());
  std::vector<char> buffer(fileSize);
  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();
  VkShaderModuleCreateInfo createInfo
  {
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .codeSize = buffer.size(),
    .pCode = (uint32_t*)(buffer.data())
  };
  VkShaderModule shaderModule;
  if(vkCreateShaderModule(forge::arsenal::device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
  {
    throw std::runtime_error("Pipe: Failed to create shader module for: " + filePath); 
  }
  return shaderModule;

}
void Pipe::ensoul(PipeSoul *soul)
{
  scribe("Pipe: ensouling with soul: " + (soul->name));
  std::string vertFilePath = "shaders/" + (soul->name) + ".vert.spv";
  std::string fragFilePath = "shaders/" + (soul->name) + ".frag.spv";

  VkShaderModule vertShaderModule = createShaderModule(vertFilePath);
  VkShaderModule fragShaderModule = createShaderModule(fragFilePath);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo
  {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .setLayoutCount = soul->setLayoutCount,
    .pushConstantRangeCount = soul->pushConstantRangeCount,
  };

  if(vkCreatePipelineLayout(forge::arsenal::device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
  {
    throw std::runtime_error("Pipe: unable to create pipelinelayout"); 
  }

  uint32_t shaderStagesCount = 2;
  const char* entryPoint = "main";
  VkPipelineShaderStageCreateInfo shaderStages[] = 
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
     .topology = soul->topology 
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
    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT 
      | VK_COLOR_COMPONENT_G_BIT 
      | VK_COLOR_COMPONENT_B_BIT 
      | VK_COLOR_COMPONENT_A_BIT,
  };
  VkPipelineColorBlendStateCreateInfo blendInfo
  {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .attachmentCount = 1,
    .pAttachments = &blendAttachState
  };

  uint32_t dynamicStatesCount = 2;
  VkDynamicState dynamicStates[] = 
  {
    VK_DYNAMIC_STATE_VIEWPORT, 
    VK_DYNAMIC_STATE_SCISSOR
  };

  VkPipelineDynamicStateCreateInfo dynamicStateInfo
  {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    .dynamicStateCount = dynamicStatesCount,
    .pDynamicStates = dynamicStates
  };
  
  // [ALERT] KHR dynamic render
  VkPipelineRenderingCreateInfo renderInfo
  {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
    .colorAttachmentCount = 1,
    .pColorAttachmentFormats = &forge::aether::swap::colorFormat,
    .depthAttachmentFormat = forge::aether::swap::depthFormat,
  };

  // summary
  VkGraphicsPipelineCreateInfo pipelineInfo
  {
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .pNext = &renderInfo,
    .stageCount = shaderStagesCount,
    .pStages = shaderStages,
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

  if(vkCreateGraphicsPipelines(forge::arsenal::device, nullptr, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
  {
    throw std::runtime_error("Pipe: unable to create vulkan graphics pipeline"); 
  }

  vkDestroyShaderModule(forge::arsenal::device, fragShaderModule, nullptr);
  vkDestroyShaderModule(forge::arsenal::device, vertShaderModule, nullptr);

  scribe("Pipe: ensouling done");
}

void Pipe::draw(uint32_t inFlightIndex, uint32_t swapchainImageIndex)
{
  

  // [TODO] make it specific to soul,
  // [TODO] loop each subpass and add layer for each
  uint32_t layoutBarriersCount = 2;

  // [ALERT] KHR dynm render
  VkImageMemoryBarrier2 layoutBarriers[]
  { 
    {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
      .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
      .srcAccessMask = 0,
      .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
      .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .image = forge::aether::swap::images[swapchainImageIndex],
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
      .image = forge::aether::swap::depthImage,
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

  VkDependencyInfo initBarrierDependInfo
  {
    .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
    .imageMemoryBarrierCount = layoutBarriersCount,
    .pImageMemoryBarriers = layoutBarriers,
  };

  crypt::vkCmdPipelineBarrier2KHR(forge::aether::frames::commandBuffers[inFlightIndex], &initBarrierDependInfo);

  VkRenderingAttachmentInfo colorAttachInfo
  {
    .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
    .imageView = forge::aether::swap::imageViews[swapchainImageIndex],
    .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
    .clearValue{.color{0.44f, 0.44f, 1.0f, 1}}
  };

  VkRenderingAttachmentInfo depthAttachmentInfo
  {
    .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
    .imageView = forge::aether::swap::depthImageView,
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
      .extent{.width = forge::aether::swap::width, .height = forge::aether::swap::height}
    },
    .layerCount = 1,
    .colorAttachmentCount = 1,
    .pColorAttachments = &colorAttachInfo,
    .pDepthAttachment = &depthAttachmentInfo
  };

  // [ALERTcheck]
  // vkCmdBeginRendering(forge::aether::frames::commandBuffers[inFlightIndex], &renderingInfo);
  crypt::vkCmdBeginRenderingKHR(forge::aether::frames::commandBuffers[inFlightIndex], &renderingInfo);
  VkViewport viewport
  {
    .x = 0, .y = 0,
    .width = (float)(forge::aether::swap::width),
    .height = (float)(forge::aether::swap::height)
  };
  vkCmdSetViewport(forge::aether::frames::commandBuffers[inFlightIndex], 0, 1, &viewport);
  VkRect2D scissor
  {
    .offset{.x = 0, .y = 0},
    .extent{.width = forge::aether::swap::width, .height = forge::aether::swap::height}
  };
  vkCmdSetScissor(forge::aether::frames::commandBuffers[inFlightIndex], 0, 1, &scissor);
  
  // [ALERT] 
  vkCmdBindPipeline(forge::aether::frames::commandBuffers[inFlightIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
  vkCmdDraw(forge::aether::frames::commandBuffers[inFlightIndex], 3, 1, 0, 0);
 
  // [ALERT] 
  //vkCmdEndRendering(forge::aether::frames::commandBuffers[inFlightIndex]);
  crypt::vkCmdEndRenderingKHR(forge::aether::frames::commandBuffers[inFlightIndex]);
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
    .image = forge::aether::swap::images[swapchainImageIndex],
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
  crypt::vkCmdPipelineBarrier2KHR(forge::aether::frames::commandBuffers[inFlightIndex], &presentDependInfo);


}
