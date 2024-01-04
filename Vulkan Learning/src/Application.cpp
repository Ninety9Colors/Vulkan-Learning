#include "Application.h"
#include "Device.h"
#include "Init.h"
#include "Swapchain.h"

Application::Application() {
  window_ = new WindowsWindow(debug_);
  create_instance();
  if (debug_) {
    create_debug_messenger();
  }
  window_->create_surface(instance_, surface_, debug_);
  create_physical_device();
  create_logical_device();
  create_swapchain();
}

Application::~Application() {
  instance_.destroyDebugUtilsMessengerEXT(debug_messenger_, nullptr, dispatch_loader_);
  instance_.destroySurfaceKHR(surface_, nullptr);
  device_.destroySwapchainKHR(swapchain_);
  device_.destroy();
  instance_.destroy();
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
  physical_device_ = VkInit::choose_physical_device(instance_, surface_, debug_);
}

void Application::create_logical_device() {
  device_ = VkInit::create_logical_device(physical_device_, surface_, debug_);
  std::array<vk::Queue, 2> queues = VkInit::get_queue(physical_device_, device_, surface_, debug_);
  graphics_queue_ = queues[0];
  present_queue_ = queues[1];
}

void Application::create_swapchain() {
  VkUtils::SwapChainBundle bundle = VkInit::create_swapchain(physical_device_, device_, surface_, window_->get_glfw_window(), debug_);
  swapchain_ = bundle.Swapchain;
  swapchain_images_ = device_.getSwapchainImagesKHR(swapchain_);
  swapchain_format_ = bundle.Format;
  swapchain_extent_ = bundle.Extent;
}
