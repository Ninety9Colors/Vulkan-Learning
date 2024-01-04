#pragma once
#include "Headers.h"
#include "VkUtils/SwapchainDetails.h"
#include "VkUtils/QueueFamilies.h"

namespace VkInit {

  // only run in debug
  void log_physical_device_properties(const vk::PhysicalDevice& device) {
    vk::PhysicalDeviceProperties props = device.getProperties();

    std::cout << "Device Name: " << props.deviceName << std::endl;
    std::cout << "Device Type: ";

    switch (props.deviceType) {
    case vk::PhysicalDeviceType::eDiscreteGpu:{
      std::cout << "Discrete Gpu" << std::endl;
      break;
    }
    case vk::PhysicalDeviceType::eIntegratedGpu: {
      std::cout << "Integrated Gpu" << std::endl;
      break;
    }
    case vk::PhysicalDeviceType::eCpu: {
      std::cout << "Cpu" << std::endl;
      break;
    }
    case vk::PhysicalDeviceType::eVirtualGpu: {
      std::cout << "Virtual Cpu" << std::endl;
      break;
    }
    default: {
      std::cout << "Other" << std::endl;
      break;
    }
    }
  }

  bool device_is_supported(const vk::PhysicalDevice& device, const bool debug) {
    std::vector<const char*> extensions{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    if (debug) {
      std::cout << std::endl << "Requesting Device Extensions: ";
      for (const char* extension : extensions) {
        std::cout << extension << ", ";
      }
      std::cout << std::endl << std::endl;
    }

    std::vector<vk::ExtensionProperties> supported_extensions = device.enumerateDeviceExtensionProperties();

    if (debug) {
      std::cout << "Supported Device Extensions: ";
      for (vk::ExtensionProperties props : supported_extensions) {
        std::cout << props.extensionName << ", ";
      }
      std::cout << std::endl << std::endl;
    }

    for (const char* req : extensions) {
      bool temp_supported_layers = false;
      for (vk::ExtensionProperties prop : supported_extensions) {
        if (strcmp(req, prop.extensionName) == 0)
          temp_supported_layers = true;
      }
      if (temp_supported_layers) {
        if (debug)
          std::cout << req << " is supported!" << std::endl << std::endl;
      }
      else {
        if (debug)
          std::cout << req << " is not supported!" << std::endl << std::endl;
        return false;
      }
    }

    return true;
  }

  vk::PhysicalDevice choose_physical_device(const vk::Instance& instance, const vk::SurfaceKHR& surface, const bool debug) {
    if (debug) {
      std::cout << "Choosing Physical Device..." << std::endl;
    }

    std::vector<vk::PhysicalDevice> devices_vector{ instance.enumeratePhysicalDevices() };

    if (debug) {
      std::cout << std::endl << "Physical Devices: " << std::endl << std::endl;
    }
    for (vk::PhysicalDevice dev : devices_vector) {
      if (debug) {
        log_physical_device_properties(dev);
      }
      if (device_is_supported(dev, debug)) {
        VkUtils::SwapChainSupportDetails swapchain_support = VkUtils::query_swapchain_support(dev, surface, debug);
        if (!swapchain_support.Formats.empty() && !swapchain_support.PresentModes.empty()) {
          return dev;
        }
      }
    }

    return nullptr;
  }

  vk::Device create_logical_device(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface, const bool debug) {
    if (debug) {
      std::cout << "Creating Logical Device..." << std::endl;
    }
    VkUtils::QueueFamilyIndices indices = VkUtils::find_queue_families(device, surface, debug);
    std::vector<uint32_t> unique_indices;
    unique_indices.push_back(indices.GraphicsFamily.value());
    if (indices.GraphicsFamily.value() != indices.PresentFamily.value()) {
      unique_indices.push_back(indices.PresentFamily.value());
    }

    float queue_priority{ 1.0f };
    std::vector<vk::DeviceQueueCreateInfo> queue_infos;
    for (uint32_t unique_indice : unique_indices) {
      vk::DeviceQueueCreateInfo queue_info {
      vk::DeviceQueueCreateFlags(),
      unique_indice,
      1,
      &queue_priority
      };

      queue_infos.push_back(queue_info);
    }

    vk::PhysicalDeviceFeatures device_features {};

    std::vector<const char*> layers{};
    if (debug) {
      layers.push_back("VK_LAYER_KHRONOS_validation");
    }

    std::vector<const char*> device_extensions{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    vk::DeviceCreateInfo create_info {
      vk::DeviceCreateFlags{},
      static_cast<uint32_t>(queue_infos.size()), queue_infos.data(),
      static_cast<uint32_t>(layers.size()), layers.data(),
      static_cast<uint32_t>(device_extensions.size()), device_extensions.data(),
      &device_features
    };

    try {
      vk::Device logical_device = device.createDevice(create_info);
      if (debug) {
        std::cout << "Device successfully abstracted!" << std::endl << std::endl;
      }
      return logical_device;
    }
    catch (vk::SystemError e) {
      if (debug) {
        std::cout << "Device creation failed!" << std::endl;
      }
      return nullptr;
    }
    
    return nullptr;
  }

  std::array<vk::Queue, 2> get_queue(const vk::PhysicalDevice & physical_device, const vk::Device& device, const vk::SurfaceKHR& surface, const bool debug) {
    if (debug) {
      std::cout << "Retrieving Graphics Queue..." << std::endl;
    }
    VkUtils::QueueFamilyIndices indices = VkUtils::find_queue_families(physical_device, surface, debug);

    return {
      device.getQueue(indices.GraphicsFamily.value(), 0),
      device.getQueue(indices.PresentFamily.value(), 0)
    };
  }
} // namespace VkInit