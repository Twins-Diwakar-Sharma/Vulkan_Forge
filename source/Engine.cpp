
#include "Engine.hpp"

Engine::Engine() 
{ 
  vulkancontext::initialize(&appName, khidki.getWindowPointer(), (uint32_t)WIDTH, (uint32_t)HEIGHT);
  pipeline.makePipeline(vulkancontext::logicalDevice, "simple");
}

Engine::~Engine()
{
  vulkancontext::destroy();
}

void Engine::run()
{
  while (!khidki.shouldClose())
  {
    glfwPollEvents();
  }
}


