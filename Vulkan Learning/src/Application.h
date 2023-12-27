#pragma once
#include "WindowsWindow.h"

class Application {
public:
  Application();
  ~Application();
  
  void run();

private:
  void create_instance();
  void create_debug_messenger();

  void create_physical_device();
  void create_logical_device();

  vk::Instance instance_;
  vk::DebugUtilsMessengerEXT debug_messenger_;
  vk::DispatchLoaderDynamic dispatch_loader_;

  vk::PhysicalDevice physical_device_;
  vk::Device device_;
  vk::Queue graphics_queue_;

  WindowsWindow* window_;

  bool running_ = true;
  bool debug_ = true;
  const char* name_ = "Vulkan Learning";
};

