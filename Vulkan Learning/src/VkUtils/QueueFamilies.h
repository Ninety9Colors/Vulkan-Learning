#pragma once
namespace VkUtils {
  struct QueueFamilyIndices {
    std::optional<uint32_t> GraphicsFamily;
    std::optional<uint32_t> PresentFamily;

    bool is_complete() {
      return GraphicsFamily.has_value() && PresentFamily.has_value();
    }
  };

  QueueFamilyIndices find_queue_families(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface, const bool debug) {
    if (debug) {
      std::cout << "Finding Queue Families..." << std::endl;
    }
    QueueFamilyIndices indices;

    std::vector<vk::QueueFamilyProperties> family_props{ device.getQueueFamilyProperties() };
    if (debug) {
      std::cout << "Device supports " << family_props.size() << " queue families!" << std::endl;
    }

    int indice{ 0 };
    for (const vk::QueueFamilyProperties& prop : family_props) {
      if (prop.queueFlags & vk::QueueFlagBits::eGraphics) {
        indices.GraphicsFamily = indice;
        if (debug) {
          std::cout << "Queue family " << indice << " is suitable for graphics!" << std::endl;
        }
      }

      if (device.getSurfaceSupportKHR(indice, surface)) {
        indices.PresentFamily = indice;
        if (debug) {
          std::cout << "Queue family " << indice << " is suitable for presenting!" << std::endl;
        }
      }

      if (indices.is_complete()) {
        break;
      }
      ++indice;
    }

    return indices;
  }

}// namespace VkUtils