#include "vulkan_renderer.hpp"

#include <stdexcept>
#include <vector>
#include <string_view>
#include <algorithm>
#include <optional>
#include <fstream>

namespace
{
    // TODO replace / inject properly. Duplicated in main.
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enable_validation_layers = true;
#endif

    const std::vector<const char*> required_validation_layers = 
    {
        "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> required_device_extensions =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
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

graphics::VulkanRenderer::VulkanRenderer(GLFWwindow* window) : window(window)
{
}

void graphics::VulkanRenderer::init()
{
    create_instance();
    pick_physical_device();
    create_logical_device();
    create_surface();
    create_swapchain();
    create_imageviews();
    create_render_pass();
    create_graphics_pipeline();
}

void graphics::VulkanRenderer::render()
{
}

graphics::VulkanRenderer::~VulkanRenderer()
{
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
    vkDestroyRenderPass(device, render_pass, nullptr);
    std::for_each(swapchain_imageviews.begin(), swapchain_imageviews.end(), [device = device](VkImageView view) {vkDestroyImageView(device, view, nullptr); });
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, rendering_surface, nullptr);
    vkDestroyInstance(instance, nullptr);
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

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) 
    {
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
        // TODO select appropriately and also handle different ones.
        return families.front();
    }

    auto is_in(const std::vector<VkExtensionProperties>& properties)
    {
        return [&properties](const char* name) { return std::find_if(properties.begin(), properties.end(),
            [name](const VkExtensionProperties properties) {return strcmp(name, properties.extensionName) == 0; }) != properties.end(); };
    }

    bool are_all_required_extensions_supported(VkPhysicalDevice device)
    {
        uint32_t extension_count;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

        std::vector<VkExtensionProperties> available_extensions(extension_count);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

        return std::all_of(required_device_extensions.begin(), required_device_extensions.end(), is_in(available_extensions));
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

    VkPhysicalDevice select_physical_device(const std::vector<VkPhysicalDevice>& devices)
    {
        VkPhysicalDevice current_device = devices.front();

        if (!are_all_required_extensions_supported(current_device))
        {
            throw std::runtime_error("Device does not support all required extensions.");
        }

        // TODO select appropriately
        return current_device;
    }

    VkSurfaceFormatKHR select_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats)
    {
        auto preferred_format = std::find_if(available_formats.begin(), available_formats.end(), 
            [](VkSurfaceFormatKHR available) { return available.format == VK_FORMAT_B8G8R8A8_SRGB && available.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; });
    
        if (preferred_format == available_formats.end())
        {
            return available_formats.front();
        }
        return *preferred_format;
    }

    VkPresentModeKHR select_presentation_mode(const std::vector<VkPresentModeKHR>& available_presentation_modes)
    {
        auto preferred_mode = std::find_if(available_presentation_modes.begin(), available_presentation_modes.end(),
            [](VkPresentModeKHR mode) { return mode == VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR; });

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D select_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        VkExtent2D result{ capabilities.currentExtent };

        // Vulkan treats this value special, if its set to the maximum then the window manager allows us different resolutions
        // and we can pick one ourselves within minExtent and maxExtent
        if (capabilities.currentExtent.width == std::numeric_limits<uint32_t>::max())
        {
            result.width = std::clamp(WIDTH, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            result.height = std::clamp(HEIGHT, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        }

        return result;
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
        .enabledExtensionCount = static_cast<uint32_t>(required_device_extensions.size()),
        .ppEnabledExtensionNames = required_device_extensions.data(),
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

void graphics::VulkanRenderer::create_surface()
{
    if (glfwCreateWindowSurface(instance, window, nullptr, &rendering_surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }

    VkBool32 is_surface_supported = VK_FALSE;
    // TODO replace with proper queue index
    if (vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, 0, rendering_surface, &is_surface_supported) != VK_SUCCESS ||
        is_surface_supported == VK_FALSE)
    {
        throw std::runtime_error("Surface not supported by device and queue");
    }
}

void graphics::VulkanRenderer::create_swapchain()
{
    SwapChainCapabilities selected_swapchain = query_swap_chain_capabilities(physical_device, rendering_surface);

    VkSurfaceFormatKHR selected_surface_format = select_surface_format(selected_swapchain.formats);
    VkPresentModeKHR selected_presentation_mode = select_presentation_mode(selected_swapchain.presentation_modes);
    VkExtent2D selected_extent = select_swap_extent(selected_swapchain.capabilities);

    uint32_t imageCount = std::max(selected_swapchain.capabilities.minImageCount + 1, selected_swapchain.capabilities.maxImageCount);

    VkSwapchainCreateInfoKHR swapchain_create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = rendering_surface,
        .minImageCount = imageCount,
        .imageFormat = selected_surface_format.format,
        .imageColorSpace = selected_surface_format.colorSpace,
        .imageExtent = selected_extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE, // TODO need to change if we have different queue families and want to share ownership between them
        .preTransform = selected_swapchain.capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = selected_presentation_mode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE
    };

    if (vkCreateSwapchainKHR(device, &swapchain_create_info, nullptr, &swapchain) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create swap chain!");
    }

    // refresh image count as vulkan can create more images than our recommendation
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
    swapchain_images.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchain_images.data());

    swapchain_format = selected_surface_format.format;
    swapchain_extent = selected_extent;
}

void graphics::VulkanRenderer::create_imageviews()
{
    swapchain_imageviews.resize(swapchain_images.size());

    size_t index = 0;
    for ( auto image : swapchain_images)
    {
        VkImageViewCreateInfo imageview_create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = swapchain_format
        };

        imageview_create_info.components = {
            .r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY
        };

        imageview_create_info.subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        };

        if (vkCreateImageView(device, &imageview_create_info, nullptr, &swapchain_imageviews[index++]) != VK_SUCCESS) 
        {
            throw std::runtime_error("Failed to create image views!");
        }
    }
}

namespace
{
    std::vector<char> file_to_buffer(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }

        size_t size = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(size);

        file.seekg(0);
        file.read(buffer.data(), size);

        return buffer;
    }

    VkShaderModule create_shader_module(const std::vector<char>& shader_bytecode, VkDevice device)
    {
        VkShaderModuleCreateInfo shader_module_create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = shader_bytecode.size(),
        .pCode = reinterpret_cast<const uint32_t*>(shader_bytecode.data()) // Make sure alignment is correct, which is true for vector std::allocator
        };

        VkShaderModule shader_module;
        if (vkCreateShaderModule(device, &shader_module_create_info, nullptr, &shader_module) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create shader module!");
        }
        return shader_module;
    }
}

void graphics::VulkanRenderer::create_graphics_pipeline()
{
    VkShaderModule vertex_shader_module = create_shader_module(file_to_buffer("vert.spv"), device);
    VkShaderModule fragment_shader_module = create_shader_module(file_to_buffer("frag.spv"), device);

    VkPipelineShaderStageCreateInfo vertex_shader_stage = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertex_shader_module,
        .pName = "main"
    };

    VkPipelineShaderStageCreateInfo fragment_shader_stage = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragment_shader_module,
        .pName = "main"
    };

    VkPipelineShaderStageCreateInfo shader_stages[] = { vertex_shader_stage, fragment_shader_stage };

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr
    };

    // Assembly: how vertexes should be interpreted, e.g. how many belong together for lines / triangles 
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };

    // Viewport: what area you see (culling)
    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(swapchain_extent.width),
        .height = static_cast<float>(swapchain_extent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D scissor = {
        .offset = { 0, 0 },
        .extent = swapchain_extent
    };

    VkPipelineViewportStateCreateInfo viewport_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor
    };

    // Rasterizer: how to render our polygons, e.g. fill, wireframe, how o cull, etc. 
    VkPipelineRasterizationStateCreateInfo rasterizer = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .lineWidth = 1.0f,
    };

    // Multisampling: a type of aliasing
    VkPipelineMultisampleStateCreateInfo multisampling = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE
    };

    // Color blending: mixing new color with color that is already in the framebuffer.
    VkPipelineColorBlendAttachmentState color_blend_attachment = {
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE, // Optional
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
        .colorBlendOp = VK_BLEND_OP_ADD, // Optional
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE, // Optional
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
        .alphaBlendOp = VK_BLEND_OP_ADD, // Optional
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    };
    
    // Here we can add some constant colors for blending
    VkPipelineColorBlendStateCreateInfo color_blending = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY, // Optional
        .attachmentCount = 1,
        .pAttachments = &color_blend_attachment
    };

    // Pipeline Layout: ...
    VkPipelineLayoutCreateInfo pipeline_layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0, // Optional
        .pSetLayouts = nullptr, // Optional
        .pushConstantRangeCount = 0, // Optional
        .pPushConstantRanges = nullptr, // Optional
    };

    if (vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout!");
    }

    // Finally the Pipeline
    VkGraphicsPipelineCreateInfo pipeline_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shader_stages,
        .pVertexInputState = &vertex_input_info,
        .pInputAssemblyState = &input_assembly,
        .pViewportState = &viewport_state,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = nullptr, // Optional
        .pColorBlendState = &color_blending,
        .pDynamicState = nullptr, // Optional
        .layout = pipeline_layout,
        .renderPass = render_pass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE, // Optional
        .basePipelineIndex = -1 // Optional
    };

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline) != VK_SUCCESS) 
    {
        throw std::runtime_error("Failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(device, fragment_shader_module, nullptr);
    vkDestroyShaderModule(device, vertex_shader_module, nullptr);
}

void graphics::VulkanRenderer::create_render_pass()
{
    VkAttachmentDescription color_attachment = {
        .format = swapchain_format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    VkAttachmentReference color_attachment_reference = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_attachment_reference
    };

    VkRenderPassCreateInfo render_pass_create_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &color_attachment,
        .subpassCount = 1,
        .pSubpasses = &subpass
    };

    if (vkCreateRenderPass(device, &render_pass_create_info, nullptr, &render_pass) != VK_SUCCESS) 
    {
        throw std::runtime_error("Failed to create render pass!");
    }
}

