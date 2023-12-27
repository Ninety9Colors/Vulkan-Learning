#pragma once
#include "Headers.h"
namespace VkInit {
  struct QueueFamilyIndices {
    std::optional<uint32_t> GraphicsFamily;
    std::optional<uint32_t> PresentFamily;

    bool is_complete() {
      return GraphicsFamily.has_value() && PresentFamily.has_value();
    }
  };

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
      std::cout << std::endl << "Supported Device Extensions: ";
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

  vk::PhysicalDevice choose_physical_device(vk::Instance& instance, const bool debug) {
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
        return dev;
      }
    }

    return nullptr;
  }

  QueueFamilyIndices find_queue_families(const vk::PhysicalDevice& device, const bool debug) {
    if (debug) {
      std::cout << "Finding Queue Families..." << std::endl;
    }
    QueueFamilyIndices indices;

    std::vector<vk::QueueFamilyProperties> family_props { device.getQueueFamilyProperties() };
    if (debug) {
      std::cout << "Device supports " << family_props.size() << " queue families!" << std::endl;
    }

    int indice { 0 };
    for (const vk::QueueFamilyProperties& prop : family_props) {
      if (prop.queueFlags & vk::QueueFlagBits::eGraphics) {
        indices.GraphicsFamily = indice;
        indices.PresentFamily = indice;
        if (debug) {
          std::cout << "Queue family " << indice << " is suitable for graphics and presenting!" << std::endl;
        }
      }
      if (indices.is_complete()) {
        break;
      }
      ++indice;
    }

    return indices;
  }

  vk::Device create_logical_device(const vk::PhysicalDevice& device, const bool debug) {
    if (debug) {
      std::cout << "Creating Logical Device..." << std::endl;
    }
    QueueFamilyIndices indices = find_queue_families(device, debug);
    //VULKAN_HPP_NAMESPACE::DeviceQueueCreateFlags flags_ = {},
      //uint32_t                                     queueFamilyIndex_ = {},
     // uint32_t                                     queueCount_ = {},
     // const float* pQueuePriorities_ = {},
     // const void* pNext_ = nullptr
    float queue_priority{ 1.0f };
    vk::DeviceQueueCreateInfo queue_info {
      vk::DeviceQueueCreateFlags(),
      indices.GraphicsFamily.value(),
      1,
      &queue_priority
    };

    vk::PhysicalDeviceFeatures device_features {};
    /*ULKAN_HPP_NAMESPACE::DeviceCreateFlags              flags_                   = {},
      uint32_t                                             queueCreateInfoCount_    = {},
      const VULKAN_HPP_NAMESPACE::DeviceQueueCreateInfo *  pQueueCreateInfos_       = {},
      uint32_t                                             enabledLayerCount_       = {},
      const char * const *                                 ppEnabledLayerNames_     = {},
      uint32_t                                             enabledExtensionCount_   = {},
      const char * const *                                 ppEnabledExtensionNames_ = {},
      const VULKAN_HPP_NAMESPACE::PhysicalDeviceFeatures * pEnabledFeatures_        = {},
      const void *                                         pNext_                   = nullptr ) VULKAN_HPP_NOEXCEPT
    */

    std::vector<const char*> layers{};
    if (debug) {
      layers.push_back("VK_LAYER_KHRONOS_validation");
    }

    vk::DeviceCreateInfo create_info {
      vk::DeviceCreateFlags{},
      1, &queue_info,
      static_cast<uint32_t> (layers.size()), layers.data(),
      0, nullptr,
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

  vk::Queue get_queue(const vk::PhysicalDevice & physical_device, const vk::Device& device, const bool debug) {
    if (debug) {
      std::cout << "Retrieving Graphics Queue..." << std::endl;
    }
    QueueFamilyIndices indices = find_queue_families(physical_device, debug);

    return device.getQueue(indices.GraphicsFamily.value(), 0);
  }

} // namespace VkInit