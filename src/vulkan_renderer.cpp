#include "vulkan_renderer.hpp"

#include <stdexcept>
#include <vector>
#include <string_view>
#include <algorithm>
#include <optional>

namespace
{
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enable_validation_layers = true;
#endif

    const std::vector<const char*> required_validation_layers = 
    {
        "VK_LAYER_KHRONOS_validation"
    };

    // a named lambda for easier readability
    auto is_in(const std::vector<VkLayerProperties>& available_layers)
    {
        return [&available_layers](const char* required_layer_name)
        {
            return available_layers.end() != std::find_if(available_layers.begin(), available_layers.end(),
                [&required_layer_name](const VkLayerProperties& properties) { return std::strcmp(required_layer_name, properties.layerName) == 0; });
        };
    }

    bool are_required_validation_layers_available() 
    {
        uint32_t layer_count;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

        std::vector<VkLayerProperties> available_layers{ layer_count };
        vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

        return std::all_of(required_validation_layers.begin(), required_validation_layers.end(), is_in(available_layers));
    }
}

void graphics::VulkanRenderer::init()
{
    create_instance();
    pick_physical_device();
    create_logical_device();
}

void graphics::VulkanRenderer::render()
{
}

graphics::VulkanRenderer::~VulkanRenderer()
{
    vkDestroyInstance(instance, nullptr);
    vkDestroyDevice(device, nullptr);
}

void graphics::VulkanRenderer::create_instance()
{
    if (enable_validation_layers && !are_required_validation_layers_available())
    {
        throw std::runtime_error("Validation layers requested but not available");
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Terms of Enrampagement";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 9, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 9, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    createInfo.enabledLayerCount = 0;

    if (enable_validation_layers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(required_validation_layers.size());
        createInfo.ppEnabledLayerNames = required_validation_layers.data();
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create vulkan instance!");
    }
}

namespace
{
    struct QueueFamily
    {
        uint32_t index;
        VkQueueFamilyProperties properties;
    };

    std::vector<QueueFamily> available_queue_families(VkPhysicalDevice device) 
    {
        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

        std::vector<VkQueueFamilyProperties> queue_family_properties { queue_family_count };
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_family_properties.data());

        std::vector<QueueFamily> result;
        for (uint32_t index = 0; index < queue_family_properties.size(); ++index)
        {
            result.push_back(QueueFamily{ .index = index, .properties = queue_family_properties[index] });
        }
        return result;
    }

    QueueFamily select_queue_family(const std::vector<QueueFamily>& families)
    {
        // TODO select appropriately
        return families.front();
    }

    VkPhysicalDevice select_physical_device(const std::vector<VkPhysicalDevice>& devices)
    {
        // TODO select appropriately
        return devices.front();
    }
}

void graphics::VulkanRenderer::pick_physical_device()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices{ deviceCount };
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    physical_device = select_physical_device(devices);
}

void graphics::VulkanRenderer::create_logical_device()
{
    auto selected_queue_family = select_queue_family(available_queue_families(physical_device));

    VkDeviceQueueCreateInfo queue_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = selected_queue_family.index,
        .queueCount = 1
    };

    float queue_priority = 1.0f;
    queue_create_info.pQueuePriorities = &queue_priority;

    VkPhysicalDeviceFeatures device_features = {};

    VkDeviceCreateInfo device_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queue_create_info,
        .enabledLayerCount = 0,
        .enabledExtensionCount = 0,
        .pEnabledFeatures = &device_features
    };

    if (enable_validation_layers)
    {
        device_create_info.enabledLayerCount = static_cast<uint32_t>(required_validation_layers.size());
        device_create_info.ppEnabledLayerNames = required_validation_layers.data();
    }

    if (vkCreateDevice(physical_device, &device_create_info, nullptr, &device) != VK_SUCCESS) 
    {
        throw std::runtime_error("Failed to create logical device!");
    }

    vkGetDeviceQueue(device, selected_queue_family.index, 0, &graphics_queue);
}
