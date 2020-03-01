#include "vulkan_setup_helper.hpp"

#include "range/v3/algorithm.hpp"
#include "range/v3/view/filter.hpp"
#include "range/v3/range/operations.hpp"

#include <stdexcept>
#include <vector>

graphics::VulkanSetupHelper::VulkanSetupHelper(GLFWwindow* window)
    : window{ window }
    , instance{ VK_NULL_HANDLE }
    , surface{ VK_NULL_HANDLE }
    , device{ VK_NULL_HANDLE }
    , physical_device{ VK_NULL_HANDLE }
    , graphics_queue { VK_NULL_HANDLE }
    , present_queue { VK_NULL_HANDLE }
{
    if (!window)
    {
        throw std::runtime_error("Null window handle passed to VulkanSetupHelper.");
    }
}

graphics::VulkanSetupHelper::~VulkanSetupHelper()
{
    vkDeviceWaitIdle(device);

    vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyDevice(device, nullptr);
	vkDestroyInstance(instance, nullptr);
}

// Some helper functions
namespace
{
    // a named lambda for easier readability
    auto are_in(const std::vector<VkLayerProperties>& available_layers)
    {
        return [&available_layers](const char* required_layer_name)
        {
            return available_layers.end() != std::find_if(available_layers.begin(), available_layers.end(),
                [&required_layer_name](const VkLayerProperties& properties) { return std::strcmp(required_layer_name, properties.layerName) == 0; });
        };
    }

    bool are_validation_layers_available(const std::vector<const char*>& requested_layer_names)
    {
        if (requested_layer_names.empty()) // no layers requested so everything is supported
        {
            return true;
        }

        uint32_t layer_count;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

        std::vector<VkLayerProperties> available_layers{ layer_count };
        vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

        return ranges::all_of(requested_layer_names, are_in(available_layers));
    }

    VkInstance create_instance(const std::vector<const char*> required_extensions,
        const std::vector<const char*> required_validation_layers)
    {
        if (!are_validation_layers_available(required_validation_layers))
        {
            throw std::runtime_error("Validation layers requested but not available");
        }

        VkApplicationInfo application_info{
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = "Terms of Enrampagement",
            .applicationVersion = VK_MAKE_VERSION(0, 9, 0),
            .pEngineName = "No Engine",
            .engineVersion = VK_MAKE_VERSION(0, 9, 0),
            .apiVersion = VK_API_VERSION_1_0
        };

        VkInstanceCreateInfo instance_info{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &application_info,
            .enabledLayerCount = static_cast<uint32_t>(required_validation_layers.size()),
            .ppEnabledLayerNames = required_validation_layers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(required_extensions.size()),
            .ppEnabledExtensionNames = required_extensions.data(),
        };

        VkInstance instance;
        if (vkCreateInstance(&instance_info, nullptr, &instance) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create vulkan instance!");
        }

        return instance;
    }

    // named lambda you know the drill
    auto are_in(const std::vector<VkExtensionProperties>& properties)
    {
        return [&properties](const char* name) { return std::find_if(properties.begin(), properties.end(),
            [name](const VkExtensionProperties properties) {return strcmp(name, properties.extensionName) == 0; }) != properties.end(); };
    }

    std::vector<graphics::QueueFamily> available_queue_families(VkPhysicalDevice device)
    {
        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

        std::vector<VkQueueFamilyProperties> queue_family_properties{ queue_family_count };
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_family_properties.data());

        std::vector<graphics::QueueFamily> result;
        for (uint32_t index = 0; index < queue_family_properties.size(); ++index)
        {
            result.push_back(graphics::QueueFamily{ .index = index, .properties = queue_family_properties[index] });
        }
        return result;
    }

    auto supports_graphics_queue()
    {
        return [](const graphics::QueueFamily& family)->bool {
            return family.properties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
        };
    }

    auto supports_present_queue(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        return [device, surface](const graphics::QueueFamily& family)->bool {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, family.index, surface, &presentSupport);
            return presentSupport;
        };
    }

    using graphics_and_present_family_t = std::pair<graphics::QueueFamily, graphics::QueueFamily>;
    graphics_and_present_family_t get_graphics_and_present_family(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        auto available_families = available_queue_families(device);

        auto graphics_queue = ranges::find_if(available_families, supports_graphics_queue());
        auto present_queue = ranges::find_if(available_families, supports_present_queue(device, surface));

        return { *graphics_queue, *present_queue };
    }

    bool are_required_queue_families_supported(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        // We need at least one queue with graphics support and one with present support ( can also be the same ones).
        auto available_families = available_queue_families(device);

        return ranges::any_of(available_families, supports_graphics_queue())
            && ranges::any_of(available_families, supports_present_queue(device, surface));
    }  

    struct SwapChainCapabilities
    {
        VkSurfaceCapabilitiesKHR capabilities = VkSurfaceCapabilitiesKHR{};
        std::vector<VkSurfaceFormatKHR> formats = {};
        std::vector<VkPresentModeKHR> presentation_modes = {};
    };

    SwapChainCapabilities query_swap_chain_capabilities(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        SwapChainCapabilities result;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &result.capabilities);

        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);

        result.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, result.formats.data());

        uint32_t presentation_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentation_mode_count, nullptr);

        result.presentation_modes.resize(presentation_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentation_mode_count, result.presentation_modes.data());

        return result;
    }

    bool is_device_suitable(VkPhysicalDevice device, const std::vector<const char*>& required_device_extensions, VkSurfaceKHR surface)
    {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        uint32_t extension_count;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
        std::vector<VkExtensionProperties> available_extensions(extension_count);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

        bool are_all_required_extensions_supported = ranges::all_of(required_device_extensions, are_in(available_extensions));

        SwapChainCapabilities swapchain_capabilities = query_swap_chain_capabilities(device, surface);

        return are_all_required_extensions_supported
            && are_required_queue_families_supported(device, surface)
            && !swapchain_capabilities.formats.empty()
            && !swapchain_capabilities.presentation_modes.empty();

    }

    VkPhysicalDevice select_physical_device(const std::vector<const char*>& required_device_extensions, VkInstance instance, VkSurfaceKHR surface)
    {
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(instance, &device_count, nullptr);

        std::vector<VkPhysicalDevice> available_devices{ device_count };
        vkEnumeratePhysicalDevices(instance, &device_count, available_devices.data());

        auto suitable_devices = available_devices | ranges::views::filter([&required_device_extensions, surface](VkPhysicalDevice device) {return is_device_suitable(device, required_device_extensions, surface); });

        if (ranges::empty(suitable_devices))
        {
            throw std::runtime_error("No suitable devices found");
        }

        // For now we just select the first of all available devices
        return ranges::front(suitable_devices);
    }

    VkSurfaceKHR create_surface(VkInstance instance, GLFWwindow* window)
    {
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface!");
        }
        return surface;
    }

    std::vector<VkDeviceQueueCreateInfo> get_device_queue_create_infos(const graphics::QueueFamily& graphics_family, const graphics::QueueFamily& present_family)
    {
        static const float priority = 1.0f; // static here is important since the create infos rever to the priority by pointer!
        std::vector<VkDeviceQueueCreateInfo> result = {
                VkDeviceQueueCreateInfo
                {
                    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .queueFamilyIndex = graphics_family.index,
                    .queueCount = 1, // We usually only want one queue, could not find a reason to create more although some HW supports more (NVIDIA)
                    .pQueuePriorities = &priority
                }
        };
        
        if (graphics_family != present_family)
        {
            result.emplace_back(VkDeviceQueueCreateInfo
                {
                    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .queueFamilyIndex = present_family.index,
                    .queueCount = 1, // We usually only want one queue, could not find a reason to create more although some HW supports more (NVIDIA)
                    .pQueuePriorities = &priority
                });
        }

        return result;
    }

    std::pair<VkQueue, VkQueue> get_graphics_and_present_queue_handles(VkDevice device, const graphics::QueueFamily& graphics_family, const graphics::QueueFamily& present_family)
    {
        std::pair<VkQueue, VkQueue> result;
        vkGetDeviceQueue(device, graphics_family.index, 0, &result.first);
        vkGetDeviceQueue(device, present_family.index, 0, &result.second);
        return result;
    }
}

/*********** Actual implementations start here ****************/
VkDevice graphics::VulkanSetupHelper::acquire_device(const std::vector<const char*>& required_extensions, 
                                                     const std::vector<const char*>& required_validation_layers, 
                                                     const std::vector<const char*>& required_device_extensions)
{
    instance = create_instance( required_extensions, required_validation_layers);

    surface = create_surface(instance, window);

    // Surface is required to make sure the physical device actually supports rendering to our window / surface
    physical_device = select_physical_device(required_device_extensions, instance, surface);

    auto&& [graphics_family, present_family] = get_graphics_and_present_family(physical_device, surface);

    auto queue_create_infos = get_device_queue_create_infos(graphics_family, present_family);

    // Nothing specific yet!
    VkPhysicalDeviceFeatures device_features = {};

    VkDeviceCreateInfo device_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size()),
        .pQueueCreateInfos = queue_create_infos.data(),
        .enabledLayerCount = static_cast<uint32_t>(required_validation_layers.size()),
        .ppEnabledLayerNames = required_validation_layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(required_device_extensions.size()),
        .ppEnabledExtensionNames = required_device_extensions.data(),
        .pEnabledFeatures = &device_features,

    };

    if (vkCreateDevice(physical_device, &device_create_info, nullptr, &device) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create logical device!");
    }

    // As Queues are part of the device set them up
    std::tie(graphics_queue, present_queue) = get_graphics_and_present_queue_handles(device, graphics_family, present_family);
 
    return device;
}

bool graphics::operator==(const QueueFamily& lhs, const QueueFamily& rhs) noexcept
{
    // Two queue families must be equivalent if they have the same index.
    return lhs.index == rhs.index;
}