#include "Window.h"

#include <algorithm>
#include <unordered_set>
#include <stdexcept>

#include <GLFW/glfw3.h>

#include "../../Utils/Logger.h"
#include "../Renderer.h"
#include "../Vulkan/VulkanPhysicalDevice.h"

Window::Window(uint32_t w, uint32_t h, std::string&& windowName) :
	name(windowName),
	width(w),
	height(h)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), name.c_str(), nullptr, nullptr);

	if (!window)
	{
		Logger::Log(std::string("Could not create GLFW window"), Logger::Category::Error);
		throw std::runtime_error("Could not create GLFW window");
	}

	// Create the Vulkan Surface.
	VkResult result = glfwCreateWindowSurface(Renderer::GetVulkanInstance(), window, nullptr, &surface);
	if (result != VK_SUCCESS)
	{
		std::string errorStr = std::string("Could not create a surface for window ") + name + std::string(" Window::Window");
		Logger::Log(std::string(errorStr), Logger::Category::Error);
		throw std::runtime_error(errorStr.c_str());
	}

	Renderer::ChooseDevice(*this);
}

Window::~Window()
{
	for (const auto& imageView : swapchainImageViews)
	{
		vkDestroyImageView(Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice(), imageView, nullptr);
	}
	vkDestroySwapchainKHR(Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice(), swapchain, nullptr);
	vkDestroySurfaceKHR(Renderer::GetVulkanInstance(), surface, nullptr);
}

bool Window::Update()
{
	if (glfwWindowShouldClose(window))
	{
		return false;
	}

	glfwPollEvents();

	return true;
}

const Window::SurfaceInfo& Window::GetSurfaceInfo(const VulkanPhysicalDevice& device)
{
	VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device(), surface, &surfaceInfo.surfaceInfo);

	if (result != VK_SUCCESS)
	{
		Logger::Log(std::string("Could not get surface info for window ") + name + std::string(" using device ") + std::string(device.GetProperties().deviceName), Logger::Category::Error);
	}

	uint32_t formatCount = 0;
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(device(), surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		surfaceInfo.surfaceFormats.resize(formatCount);
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(device(), surface, &formatCount, surfaceInfo.surfaceFormats.data());
	}

	if (result != VK_SUCCESS)
	{
		Logger::Log(std::string("Could not get surface formats for window ") + name + std::string(" using device ") + std::string(device.GetProperties().deviceName), Logger::Category::Error);
	}

	uint32_t presentModeCount = 0;
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(device(), surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		surfaceInfo.presentModes.resize(presentModeCount);
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(device(), surface, &presentModeCount, surfaceInfo.presentModes.data());
	}

	if (result != VK_SUCCESS)
	{
		Logger::Log(std::string("Could not get surface present modes for window ") + name + std::string(" using device ") + std::string(device.GetProperties().deviceName), Logger::Category::Error);
	}

	bool foundDesiredFormat = false;
	for (const auto& format : surfaceInfo.surfaceFormats)
	{
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			surfaceFormat = format;
			Logger::Log(std::string("Using VK_FORMAT_B8G8R8A8_SRGB format with VK_COLOR_SPACE_SRGB_NONLINEAR_KHR color space."), Logger::Category::Success);
			foundDesiredFormat = true;
			break;
		}
	}

	if (!foundDesiredFormat)
	{
		surfaceFormat = surfaceInfo.surfaceFormats.front();
		Logger::Log(std::string("Defaulting to first found surface format because desired format was not found."), Logger::Category::Warning);
	}

	std::unordered_set<VkPresentModeKHR> modes(surfaceInfo.presentModes.begin(), surfaceInfo.presentModes.end());

	// Print all of the available present modes.
	for (const auto& mode : modes)
	{
		switch (mode)
		{
		case VK_PRESENT_MODE_MAILBOX_KHR:
			Logger::Log(std::string("Present Mode Mailbox available"));
			break;
		case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
			Logger::Log(std::string("Present Mode FIFO Relaxed available"));
			break;
		case VK_PRESENT_MODE_IMMEDIATE_KHR:
			Logger::Log(std::string("Present Mode Immediate available"));
			break;
		case VK_PRESENT_MODE_FIFO_KHR:
			Logger::Log(std::string("Present Mode FIFO available"));
			break;
		default:
			Logger::Log(std::string("Unknown Present Mode available"));
			break;
		}
	}
	
	// Choose the desired present mode.
	if (modes.find(VK_PRESENT_MODE_MAILBOX_KHR) != modes.end())
	{
		presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		Logger::Log(std::string("Present Mode set to Mailbox"), Logger::Category::Success);
	}
	else if (modes.find(VK_PRESENT_MODE_FIFO_RELAXED_KHR) != modes.end())
	{
		presentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
		Logger::Log(std::string("Present Mode set to FIFO Relaxed"), Logger::Category::Warning);
	}
	else
	{
		presentMode = VK_PRESENT_MODE_FIFO_KHR;
		Logger::Log(std::string("Present Mode set to FIFO"), Logger::Category::Warning);
	}

	// Vulkan uses pixels and GLFW uses screen coordinates so we need to convert when choosing the correct extent values for the swap chain.
	if (surfaceInfo.surfaceInfo.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		swapchainExtent = surfaceInfo.surfaceInfo.currentExtent;
	}
	else
	{
		int w, h;
		glfwGetFramebufferSize(window, &w, &h);

		VkExtent2D extentInPixels{
			static_cast<uint32_t>(w),
			static_cast<uint32_t>(h)
		};

		swapchainExtent = extentInPixels;
	}

	// Make sure the extent is allowed.
	swapchainExtent.width = std::clamp(swapchainExtent.width, surfaceInfo.surfaceInfo.minImageExtent.width, surfaceInfo.surfaceInfo.maxImageExtent.width);
	swapchainExtent.height = std::clamp(swapchainExtent.height, surfaceInfo.surfaceInfo.minImageExtent.height, surfaceInfo.surfaceInfo.maxImageExtent.height);

	return surfaceInfo;
}

void Window::CreateSwapchain()
{
	Logger::Log(std::string("Min sawpchain images: ") + std::to_string(surfaceInfo.surfaceInfo.minImageCount));
	Logger::Log(std::string("Max swapchain images: ") + std::to_string(surfaceInfo.surfaceInfo.maxImageCount));

	// Image count is 1 more than the minimum if allowed.
	uint32_t imageCount = surfaceInfo.surfaceInfo.minImageCount + ((surfaceInfo.surfaceInfo.maxImageCount > surfaceInfo.surfaceInfo.minImageCount) ? 1 : 0);

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = swapchainExtent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.preTransform = surfaceInfo.surfaceInfo.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	// If the images are shared between more than one queue we need to handle that.
	VulkanPhysicalDevice* device = Renderer::GetVulkanPhysicalDevice();
	if (!device)
	{
		Logger::Log(std::string("Could not get VulkanPhysicalDevice in Window::CreateSwapchain"), Logger::Category::Error);
		throw std::runtime_error("Could not get VulkanPhysicalDevice in Window::CreateSwapchain");
		return;
	}

	VulkanPhysicalDevice::QueueFamilies queueFamilies = device->GetQueueFamilies();

	if (queueFamilies.graphicsIndex.has_value() && queueFamilies.presentationIndex.has_value())
	{
		if (queueFamilies.graphicsIndex.value() != queueFamilies.presentationIndex.value())
		{
			uint32_t queueFamilyIndices[] = { queueFamilies.graphicsIndex.value(), queueFamilies.presentationIndex.value() };
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
			Logger::Log(std::string("Image sharing mode set to concurrent for swapchain"), Logger::Category::Warning);
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
			Logger::Log(std::string("Image sharing mode set to exclusive for swapchain"));
		}
	}
	else
	{
		Logger::Log(std::string("Graphics queue or Present queue not set for chosen vulkan physical device"), Logger::Category::Error);
		throw std::runtime_error("Graphics queue or Present queue not set for chosen vulkan physical device");
		return;
	}
	
	// Create the swapchain finally.
	VkResult result = vkCreateSwapchainKHR(device->GetLogicalDevice(), &createInfo, nullptr, &swapchain);
	if (result != VK_SUCCESS)
	{
		Logger::Log(std::string("Failed to create swapchain"), Logger::Category::Error);
		throw std::runtime_error("Failed to create swapchain");
		return;
	}

	vkGetSwapchainImagesKHR(device->GetLogicalDevice(), swapchain, &imageCount, nullptr);
	swapchainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device->GetLogicalDevice(), swapchain, &imageCount, swapchainImages.data());

	uint32_t index = 0;
	swapchainImageViews.resize(imageCount);
	for (const auto& image : swapchainImages)
	{
		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = image;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = surfaceFormat.format;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		result = vkCreateImageView(device->GetLogicalDevice(), &imageViewCreateInfo, nullptr, &swapchainImageViews[index]);

		if (result != VK_SUCCESS)
		{
			Logger::Log(std::string("Failed to create image view"), Logger::Category::Error);
			throw std::runtime_error("Failed to create image view");
			return;
		}

		index++;
	}

	Logger::Log(std::string("Created swapchain with ") + std::to_string(imageCount) + std::string(" images"), Logger::Category::Success);
}
