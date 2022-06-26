#include "VulkanPhysicalDevice.h"

#include <stdexcept>
#include <set>

#include "../../Utils/Logger.h"

VulkanPhysicalDevice::VulkanPhysicalDevice(VkPhysicalDevice&& device, VkPhysicalDeviceProperties&& p, VkPhysicalDeviceFeatures&& f) :
	physicalDevice(device),
	properties(p),
	features(f)
{
	FindQueueFamilies();
	//GetDisplayProperties();
}

VulkanPhysicalDevice::~VulkanPhysicalDevice()
{
	vkDestroyDevice(logicalDevice, nullptr);
}

void VulkanPhysicalDevice::GetDisplayProperties()
{
	uint32_t displayCount = 0;

	VkResult result = vkGetPhysicalDeviceDisplayPropertiesKHR(physicalDevice,&displayCount, nullptr);

	if (result != VK_SUCCESS)
	{
		std::string errorLog = std::string("Failed to get physical device display properties for ") + std::string(properties.deviceName) + std::string(" is a display connected to this device?");
		Logger::Log(std::string(errorLog), Logger::Category::Error);
		throw std::runtime_error(errorLog.c_str());
	}

	std::vector<VkDisplayPropertiesKHR> displayProperties(displayCount);

	result = vkGetPhysicalDeviceDisplayPropertiesKHR(physicalDevice, &displayCount, displayProperties.data());

	if (result != VK_SUCCESS)
	{
		std::string errorLog = std::string("Failed to get physical device display properties for ") + std::string(properties.deviceName) + std::string(" is a display connected to this device?");
		Logger::Log(std::string(errorLog), Logger::Category::Error);
		throw std::runtime_error(errorLog.c_str());
	}

	for (const auto& display : displayProperties)
	{
		Logger::Log(std::string(display.displayName));
	}
}

bool VulkanPhysicalDevice::FindPresentationQueueFamily(const VkSurfaceKHR& surface)
{
	uint32_t queueIndex = 0;
	for (const auto& queueFamily : queueFamilyProperties)
	{
		VkBool32 supported = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueIndex, surface, &supported);

		if (supported)
		{
			queueFamilies.presentationIndex.emplace(queueIndex);

			// Only break if this queue also support graphics. We want to prefer a queue that supports both graphics and presentation for improved performance.
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				queueFamilies.graphicsIndex.emplace(queueIndex);
				break;
			}
		}

		queueIndex++;
	}

	if (!queueFamilies.presentationIndex.has_value())
	{
		Logger::Log(std::string(properties.deviceName) + std::string(" does not support presentation"), Logger::Category::Warning);
		return false;
	}

	return true;
}

bool VulkanPhysicalDevice::IsDeviceSuitable()
{
	return queueFamilies.graphicsIndex.has_value() && queueFamilies.presentationIndex.has_value();
}

void VulkanPhysicalDevice::FinalizeDevice()
{
	// You can only finalize a physical device once.
	static bool finalized = false;
	
	if(!finalized)
	{
		CreateLogicalDevice();
		finalized = true;
	}
}

bool VulkanPhysicalDevice::SupportsSwapChain()
{
	static bool checkSwapchainSupport = true;

	if (checkSwapchainSupport)
	{
		checkSwapchainSupport = false;

		uint32_t count = 0;
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, nullptr);

		std::vector<VkExtensionProperties> extensions(count);
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, extensions.data());

		for (const auto& extension : extensions)
		{
			if (std::string(extension.extensionName) == std::string(VK_KHR_SWAPCHAIN_EXTENSION_NAME))
			{
				supportsSwapchain = true;
				return true;
			}
		}

		return false;
	}
	else
	{
		return supportsSwapchain;
	}
}

void VulkanPhysicalDevice::FindQueueFamilies()
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	queueFamilyProperties.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

	// Currently it will set the queue family index to the same index for each flag if it has more than one flag.
	// TODO: Find out if this is optimal.
	// EDIT: This should be optimal because VK_SHARING_MODE_EXCLUSIVE seems to be better performing than VK_SHARING_MODE_CONCURRENT

	uint32_t i = 0;
	for (const auto& queueFamily : queueFamilyProperties)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			queueFamilies.graphicsIndex.emplace(i);
		}

		if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			queueFamilies.computeIndex.emplace(i);
		}

		if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			queueFamilies.transferIndex.emplace(i);
		}

		if (queueFamily.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
		{
			queueFamilies.sparseBindingIndex.emplace(i);
		}

		if (queueFamily.queueFlags & VK_QUEUE_PROTECTED_BIT)
		{
			queueFamilies.protectedIndex.emplace(i);
		}
		i++;
	}
}

void VulkanPhysicalDevice::CreateLogicalDevice()
{
	if (!queueFamilies.graphicsIndex.has_value())
	{
		Logger::Log(std::string("Cannot create a logical device. No queue family with graphics support. VulkanPhysicalDevice::CreateLogicalDevice"), Logger::Category::Error);
		throw std::runtime_error("Cannot create a logical device. No queue family with graphics support. VulkanPhysicalDevice::CreateLogicalDevice");
	}

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { queueFamilies.graphicsIndex.value(), queueFamilies.presentationIndex.value() };
	
	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}
	

	VkPhysicalDeviceFeatures deviceFeatures{};

	const char* extensions[] = {"VK_KHR_swapchain"};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = SupportsSwapChain() ? sizeof(extensions) / sizeof(extensions[0]) : 0;
	createInfo.ppEnabledExtensionNames = SupportsSwapChain() ? &extensions[0] : nullptr;
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;

	VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice);

	if (result != VK_SUCCESS)
	{
		Logger::Log(std::string("Could not create logical device. VulkanPhysicalDevice::CreateLogicalDevice"), Logger::Category::Error);
		throw std::runtime_error("Could not create logical device. VulkanPhysicalDevice::CreateLogicalDevice");
		return;
	}

	Logger::Log(std::string("Created logical device using ") + std::string(properties.deviceName), Logger::Category::Success);

	vkGetDeviceQueue(logicalDevice, queueFamilies.graphicsIndex.value(), 0, &graphicsQueue);
	if (queueFamilies.graphicsIndex.value() == queueFamilies.presentationIndex.value())
	{
		presentationQueue = graphicsQueue;
	}
	else
	{
		vkGetDeviceQueue(logicalDevice, queueFamilies.presentationIndex.value(), 0, &presentationQueue);
	}
}
