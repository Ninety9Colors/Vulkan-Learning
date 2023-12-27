#include "Application.h"
#include "Device.h"
#include "Init.h"

Application::Application() {
  window_ = new WindowsWindow(debug_);
  create_instance();
  if (debug_) {
    create_debug_messenger();
  }
  create_physical_device();
  create_logical_device();
}

Application::~Application() {
  instance_.destroyDebugUtilsMessengerEXT(debug_messenger_, nullptr, dispatch_loader_);
  device_.destroy();
  vkDestroyInstance(instance_, nullptr);
  delete window_;
}

void Application::run() {
  while (running_) {
    window_->on_update();
  }
}

void Application::create_instance() {
  instance_ = VkInit::make_instance(name_, debug_);
}

void Application::create_debug_messenger() {
  dispatch_loader_ = vk::DispatchLoaderDynamic(instance_, vkGetInstanceProcAddr);
  debug_messenger_ = VkInit::create_debug_messenger(instance_, dispatch_loader_);
}

void Application::create_physical_device() {
  physical_device_ = VkInit::choose_physical_device(instance_, debug_);
}

void Application::create_logical_device() {
  device_ = VkInit::create_logical_device(physical_device_, debug_);
  graphics_queue_ = VkInit::get_queue(physical_device_, device_, debug_);
}
