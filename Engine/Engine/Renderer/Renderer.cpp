#include "Renderer.h"

#include <stdexcept>

#include "../Engine.h"
#include "../Utils/Logger.h"
#include "Vulkan/VulkanPhysicalDevice.h"
#include "Windows/WindowManager.h"
#include "Windows/Window.h"
#include "Commands/CommandManager.h"

Renderer* Renderer::instance = nullptr;

const unsigned int Renderer::maxFramesInFlight = 2;

void Renderer::Initialize()
{
	if (!instance)
	{
		instance = new Renderer();
		Logger::Log(std::string("Initialized Renderer"), Logger::Category::Success);
		return;
	}

	Logger::Log(std::string("Calling Renderer::Initialize when the renderer has already been initialized."), Logger::Category::Warning);
}

void Renderer::Terminate()
{
	if (instance)
	{
		CommandManager::Terminate();
		delete instance;
		Logger::Log(std::string("Terminated Renderer"), Logger::Category::Success);
		return;
	}

	Logger::Log(std::string("Calling Renderer::Terminate when the renderer has not been initialized."), Logger::Category::Warning);
}

bool Renderer::Update()
{
	return WindowManager::Update();
}

const VkInstance& Renderer::GetVulkanInstance()
{
	static VkInstance nullVulkanInstance = VK_NULL_HANDLE;

	if (!instance)
	{
		Logger::Log(std::string("Calling Renderer::GetVulkanInstance before Renderer::Initialize"), Logger::Category::Error);
		return nullVulkanInstance;
	}

	return instance->vulkanInstance;
}

void Renderer::ChooseDevice(Window& window)
{
	// We only need to choose a device once.
	static bool chosen = false;

	if (!chosen)
	{
		if (!instance)
		{
			Logger::Log(std::string("Calling Renderer::ChooseDevice before Renderer::Initialize"), Logger::Category::Error);
			throw std::runtime_error("Calling Renderer::ChooseDevice before Renderer::Initialize");
			return;
		}

		for (const auto& device : instance->vulkanPhysicalDevices)
		{
			if (device->FindPresentationQueueFamily(window.GetSurface()) && device->IsDeviceSuitable() && device->SupportsSwapChain())
			{
				const Window::SurfaceInfo& surfaceInfo = window.GetSurfaceInfo(*device);
				if (!(surfaceInfo.surfaceFormats.size() && surfaceInfo.presentModes.size()))
				{
					continue;
				}

				instance->chosenDevice = device;
				device->FinalizeDevice();
				window.CreateSwapchain();
				Logger::Log(std::string(device->GetName()) + std::string(" has been chosen as the rendering device."), Logger::Category::Success);
				chosen = true;

				// We need to initialize the command manager after a device is chosen because the command manager needs to use the chosen device.
				CommandManager::Initialize();
				return;
			}
		}

		Logger::Log(std::string("Could not find a suitable vulkan physical device. Renderer::ChooseDevice"), Logger::Category::Error);
	}
}

VulkanPhysicalDevice* Renderer::GetVulkanPhysicalDevice()
{
	if (!instance)
	{
		Logger::Log(std::string("Calling Renderer::GetVulkanPhysicalDevice before Renderer::Initialize"), Logger::Category::Error);
		return nullptr;
	}

	return instance->chosenDevice;
}

const unsigned int Renderer::GetMaxFramesInFlight()
{
	if (!instance)
	{
		static const char* invalidInstanceError = "Calling Renderer::GetMaxFramesInFlight before Renderer::Initialize.";
		Logger::Log(std::string(invalidInstanceError));
		throw std::runtime_error(invalidInstanceError);
		return 0;
	}

	return maxFramesInFlight;
}

Renderer::Renderer() :
	vulkanInstance(),
	applicationInfo(),
	vulkanPhysicalDevices(std::vector<VulkanPhysicalDevice*>())
{
	WindowManager::Initialize();
	InitializeVulkan();
}

Renderer::~Renderer()
{
	TerminateVulkan();
	WindowManager::Terminate();
}

const std::vector<std::string>& Renderer::GetSupportedInstanceExtensions()
{
	static std::vector<std::string> supportedInstanceExtensions;
	static std::vector<VkExtensionProperties> supportedInstanceExtensionProperties;

	if (supportedInstanceExtensionProperties.size() == 0)
	{
		uint32_t supportedExtensionsCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionsCount, nullptr);
		supportedInstanceExtensionProperties.resize(supportedExtensionsCount);
		supportedInstanceExtensions.reserve(supportedExtensionsCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionsCount, supportedInstanceExtensionProperties.data());

		// If this is the first time log them and store them for the future.
		std::string log("Supported Vulkan Instance Extensions: ");
		for (const VkExtensionProperties& extension : supportedInstanceExtensionProperties)
		{
			supportedInstanceExtensions.push_back(std::string(extension.extensionName));
			log.append(std::string("\n\t") + std::string(extension.extensionName));
		}
		Logger::Log(std::move(log));
	}

	return supportedInstanceExtensions;
}

const std::vector<std::string>& Renderer::GetSupportedInstanceLayers()
{
	static std::vector<std::string> supportedInstanceLayers;
	static std::vector<VkLayerProperties> supportedInstanceLayersProperties;

	if (supportedInstanceLayersProperties.size() == 0)
	{
		uint32_t supportedLayersCount = 0;
		vkEnumerateInstanceLayerProperties(&supportedLayersCount, nullptr);
		supportedInstanceLayersProperties.resize(supportedLayersCount);
		supportedInstanceLayers.reserve(supportedLayersCount);
		vkEnumerateInstanceLayerProperties(&supportedLayersCount, supportedInstanceLayersProperties.data());

		// If this is the first time log them and store them for the future.
		std::string log("Supported Vulkan Instance Layers: ");
		for (const VkLayerProperties& layer : supportedInstanceLayersProperties)
		{
			supportedInstanceLayers.push_back(std::string(layer.layerName));
			log.append(std::string("\n\t") + std::string(layer.layerName));
		}
		Logger::Log(std::move(log));
	}

	return supportedInstanceLayers;
}

void Renderer::InitializeVulkan()
{
	CreateVulkanInstance();
	CreateVulkanDebugMessenger();
	FindVulkanPhysicalDevices();
}

void Renderer::CreateVulkanInstance()
{
	// Define the application info for the engine and the game the engine is running.
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pApplicationName = Engine::GetGameName().c_str();
	applicationInfo.applicationVersion = Engine::GetGameVersion().version;
	applicationInfo.pEngineName = "Engine";
	applicationInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
	applicationInfo.apiVersion = VK_API_VERSION_1_3;
	applicationInfo.pNext = nullptr;

	// Defime the cretion info for the vulkan instance.
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &applicationInfo;
	static const char* extensions[] = { 
		"VK_KHR_surface"
#ifdef _WIN32
		,"VK_KHR_win32_surface"
#endif
#ifndef NDEBUG
		,"VK_EXT_debug_utils"
#endif
	};
	createInfo.enabledExtensionCount = sizeof(extensions) / sizeof(extensions[0]);
	createInfo.ppEnabledExtensionNames = &extensions[0];
#ifndef NDEBUG
	static const char* layers[] = {
		"VK_LAYER_KHRONOS_validation"
	};
	createInfo.enabledLayerCount = sizeof(layers)/sizeof(layers[0]);
#endif
#ifndef NDEBUG
	createInfo.ppEnabledLayerNames = &layers[0];
#else
	static const char** layers = nullptr;
	createInfo.ppEnabledLayerNames = nullptr;
#endif

	// Create the vulkan instance.
	VkResult result = vkCreateInstance(&createInfo, nullptr, &vulkanInstance);

	const std::vector<std::string>& supportedExtensions = GetSupportedInstanceExtensions();
	const std::vector<std::string>& supportedLayers = GetSupportedInstanceLayers();

	// Check if the vulkan instance was created successfully
	if (result != VK_SUCCESS)
	{
		// If a required instance extension is not supported log it.
		if (result == VK_ERROR_EXTENSION_NOT_PRESENT)
		{
			for (uint32_t i = 0; i < createInfo.enabledExtensionCount; i++)
			{
				bool extensionSupported = false;
				for (const std::string& extension : supportedExtensions)
				{
					if (extension == std::string(extensions[i]))
					{
						extensionSupported = true;
						break;
					}
				}

				if (!extensionSupported)
				{
					Logger::Log(std::string("Failed to create the vulkan instance. ") + std::string(extensions[i]) + " extension not supported.", Logger::Category::Error);
					throw std::runtime_error((std::string("Failed to create the vulkan instance. ") + std::string(extensions[i]) + " extension not supported.").c_str());
					break;
				}
			}
		}
		else if (result == VK_ERROR_LAYER_NOT_PRESENT)
		{
			for (uint32_t i = 0; i < createInfo.enabledLayerCount; i++)
			{
				bool layerSupported = false;
				for (const std::string& layer : supportedLayers)
				{
					if (layer == std::string(layers[i]))
					{
						layerSupported = true;
						break;
					}
				}

				if (!layerSupported)
				{
					Logger::Log(std::string("Failed to create the vulkan instance. ") + std::string(layers[i]) + " layer not supported.", Logger::Category::Error);
					throw std::runtime_error((std::string("Failed to create the vulkan instance. ") + std::string(layers[i]) + " layer not supported.").c_str());
					break;
				}
			}
		}
		else
		{
			Logger::Log(std::string("Failed to create the vulkan instance"), Logger::Category::Error);
			throw std::runtime_error("Failed to create the vulkan instance");
		}
	}
	else
	{
		Logger::Log(std::string("Created the vulkan instance"), Logger::Category::Success);
	}
}

void Renderer::CreateVulkanDebugMessenger()
{
#ifndef NDEBUG
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = VulkanDebugCallback;
	createInfo.pUserData = nullptr; // Optional

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkanInstance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		VkResult result = func(vulkanInstance, &createInfo, nullptr, &vulkanDebugMessenger);
		if (result == VK_SUCCESS)
		{
			Logger::Log(std::string("Created the vulkan debug messenger"), Logger::Category::Success);
		}
		else
		{
			Logger::Log(std::string("Failed to create the vulkan debug messenger"), Logger::Category::Error);
			throw std::runtime_error("Failed to set up debug messenger");
		}
	}
	else {
		Logger::Log(std::string("Could not get the vulkan function to create the vulkan debug messenger"), Logger::Category::Error);
		throw std::runtime_error("Failed to set up debug messenger");
	}
#endif // NDEBUG
}

void Renderer::DestroyVulkanDebugMessenger()
{
#ifndef NDEBUG
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkanInstance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(vulkanInstance, vulkanDebugMessenger, nullptr);
	}
#endif // NDEBUG
}

void Renderer::TerminateVulkan()
{
	for (VulkanPhysicalDevice* device : vulkanPhysicalDevices)
	{
		delete device;
	}

	DestroyVulkanDebugMessenger();
	vkDestroyInstance(vulkanInstance, nullptr);
}

void Renderer::FindVulkanPhysicalDevices()
{
	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(vulkanInstance, &physicalDeviceCount, nullptr);

	if (physicalDeviceCount == 0)
	{
		Logger::Log(std::string("Could not find any vulkan enabled physical deveices"), Logger::Category::Error);
		throw std::runtime_error("Could not find any vulkan enabled physical devices");
		return;
	}

	vulkanPhysicalDevices.reserve(physicalDeviceCount);
	std::vector<VkPhysicalDevice> vkPhysicalDevices(physicalDeviceCount);
	vkEnumeratePhysicalDevices(vulkanInstance, &physicalDeviceCount, vkPhysicalDevices.data());

	std::string vulkanPhysicalDeviceLog("Vulkan Physical Devices: \n\t");

	for (uint32_t i = 0; i < physicalDeviceCount; i++)
	{
		VkPhysicalDeviceProperties deviceProperties{};
		VkPhysicalDeviceFeatures deviceFeatures{};

		vkGetPhysicalDeviceProperties(vkPhysicalDevices[i], &deviceProperties);
		LogVulkanPhysicalDeviceProperties(deviceProperties, vulkanPhysicalDeviceLog);
		vkGetPhysicalDeviceFeatures(vkPhysicalDevices[i], &deviceFeatures);
		LogVulkanPhysicalDeviceFeatures(deviceFeatures, vulkanPhysicalDeviceLog);


		// TODO: Add better device picking. Currently it will prefer discrete GPUs and pick the first discrete GPU.
		VkPhysicalDevice vulkanPhysicalDevice = VK_NULL_HANDLE;
		static bool deviceFound = false;
		
		if (!deviceFound)
		{
			vulkanPhysicalDevice = vkPhysicalDevices[i];

			// Wrap the physical device.
			vulkanPhysicalDevices.push_back(new VulkanPhysicalDevice(std::move(vulkanPhysicalDevice), std::move(deviceProperties), std::move(deviceFeatures)));
		}

		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			vulkanPhysicalDevice = vkPhysicalDevices[i];
			deviceFound = true;

			// Wrap the physical device.
			vulkanPhysicalDevices.push_back(new VulkanPhysicalDevice(std::move(vulkanPhysicalDevice), std::move(deviceProperties), std::move(deviceFeatures)));
		}

	}

	Logger::Log(move(vulkanPhysicalDeviceLog));
}

static const std::string whiteSpace("\n\t\t");
static const std::string tab("\t");

void Renderer::LogVulkanPhysicalDeviceProperties(const VkPhysicalDeviceProperties& deviceProperties, std::string& vulkanPhysicalDeviceLog)
{
	vulkanPhysicalDeviceLog.append(std::string(deviceProperties.deviceName));
	vulkanPhysicalDeviceLog.append(std::string(" Device Properties: ") + whiteSpace);
	vulkanPhysicalDeviceLog.append(std::string("API Version: ") +
		std::to_string(VK_API_VERSION_VARIANT(deviceProperties.apiVersion)) + "." +
		std::to_string(VK_API_VERSION_MAJOR(deviceProperties.apiVersion)) + "." +
		std::to_string(VK_API_VERSION_MINOR(deviceProperties.apiVersion)) + "." +
		std::to_string(VK_API_VERSION_PATCH(deviceProperties.apiVersion)) +
		whiteSpace);

	vulkanPhysicalDeviceLog.append(std::string("DeviceID: ") + std::to_string(deviceProperties.deviceID) + whiteSpace);

	std::string deviceType("");
	switch (deviceProperties.deviceType)
	{
	case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_OTHER:
		deviceType.append("Other");
		break;
	case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
		deviceType.append("Integrated GPU");
		break;
	case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
		deviceType.append("Discrete GPU");
		break;
	case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
		deviceType.append("Virtual GPU");
		break;
	case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_CPU:
		deviceType.append("CPU");
		break;
	default:
		deviceType.append("Invalid type");
		break;
	}

	vulkanPhysicalDeviceLog.append(std::string("Device Type: ") + deviceType + whiteSpace);

	vulkanPhysicalDeviceLog.append(std::string("Driver Version: ") +
		std::to_string(VK_API_VERSION_VARIANT(deviceProperties.driverVersion)) + "." +
		std::to_string(VK_API_VERSION_MAJOR(deviceProperties.driverVersion)) + "." +
		std::to_string(VK_API_VERSION_MINOR(deviceProperties.driverVersion)) + "." +
		std::to_string(VK_API_VERSION_PATCH(deviceProperties.driverVersion)) +
		whiteSpace);

	vulkanPhysicalDeviceLog.append(std::string("Limits: ") + whiteSpace + tab);
	vulkanPhysicalDeviceLog.append(std::string("Buffer Image Granularity: ") + std::to_string(deviceProperties.limits.bufferImageGranularity) + whiteSpace + tab);
	vulkanPhysicalDeviceLog.append(std::string("Discrete Queue Priorities: ") + std::to_string(deviceProperties.limits.discreteQueuePriorities) + whiteSpace + tab);

	vulkanPhysicalDeviceLog.append(std::string("Framebuffer Color Sample Counts: "));

	if (deviceProperties.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_1_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_1_BIT"));
	}
	if (deviceProperties.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_2_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_2_BIT"));
	}
	if (deviceProperties.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_4_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_4_BIT"));
	}
	if (deviceProperties.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_8_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_8_BIT"));
	}
	if (deviceProperties.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_16_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_16_BIT"));
	}
	if (deviceProperties.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_32_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_32_BIT"));
	}
	if (deviceProperties.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_64_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_64_BIT"));
	}

	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Framebuffer Depth Sample Counts: "));

	if (deviceProperties.limits.framebufferDepthSampleCounts & VK_SAMPLE_COUNT_1_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_1_BIT"));
	}
	if (deviceProperties.limits.framebufferDepthSampleCounts & VK_SAMPLE_COUNT_2_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_2_BIT"));
	}
	if (deviceProperties.limits.framebufferDepthSampleCounts & VK_SAMPLE_COUNT_4_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_4_BIT"));
	}
	if (deviceProperties.limits.framebufferDepthSampleCounts & VK_SAMPLE_COUNT_8_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_8_BIT"));
	}
	if (deviceProperties.limits.framebufferDepthSampleCounts & VK_SAMPLE_COUNT_16_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_16_BIT"));
	}
	if (deviceProperties.limits.framebufferDepthSampleCounts & VK_SAMPLE_COUNT_32_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_32_BIT"));
	}
	if (deviceProperties.limits.framebufferDepthSampleCounts & VK_SAMPLE_COUNT_64_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_64_BIT"));
	}

	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Framebuffer No Attachment Sample Counts: "));

	if (deviceProperties.limits.framebufferNoAttachmentsSampleCounts & VK_SAMPLE_COUNT_1_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_1_BIT"));
	}
	if (deviceProperties.limits.framebufferNoAttachmentsSampleCounts & VK_SAMPLE_COUNT_2_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_2_BIT"));
	}
	if (deviceProperties.limits.framebufferNoAttachmentsSampleCounts & VK_SAMPLE_COUNT_4_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_4_BIT"));
	}
	if (deviceProperties.limits.framebufferNoAttachmentsSampleCounts & VK_SAMPLE_COUNT_8_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_8_BIT"));
	}
	if (deviceProperties.limits.framebufferNoAttachmentsSampleCounts & VK_SAMPLE_COUNT_16_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_16_BIT"));
	}
	if (deviceProperties.limits.framebufferNoAttachmentsSampleCounts & VK_SAMPLE_COUNT_32_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_32_BIT"));
	}
	if (deviceProperties.limits.framebufferNoAttachmentsSampleCounts & VK_SAMPLE_COUNT_64_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_64_BIT"));
	}

	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Framebuffer Stencil Sample Counts: "));

	if (deviceProperties.limits.framebufferStencilSampleCounts & VK_SAMPLE_COUNT_1_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_1_BIT"));
	}
	if (deviceProperties.limits.framebufferStencilSampleCounts & VK_SAMPLE_COUNT_2_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_2_BIT"));
	}
	if (deviceProperties.limits.framebufferStencilSampleCounts & VK_SAMPLE_COUNT_4_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_4_BIT"));
	}
	if (deviceProperties.limits.framebufferStencilSampleCounts & VK_SAMPLE_COUNT_8_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_8_BIT"));
	}
	if (deviceProperties.limits.framebufferStencilSampleCounts & VK_SAMPLE_COUNT_16_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_16_BIT"));
	}
	if (deviceProperties.limits.framebufferStencilSampleCounts & VK_SAMPLE_COUNT_32_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_32_BIT"));
	}
	if (deviceProperties.limits.framebufferStencilSampleCounts & VK_SAMPLE_COUNT_64_BIT)
	{
		vulkanPhysicalDeviceLog.append(whiteSpace + tab + tab + std::string("VK_SAMPLE_COUNT_64_BIT"));
	}

	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Line Width Granularity: ") + std::to_string(deviceProperties.limits.lineWidthGranularity));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Line Width Range: ") + std::to_string(deviceProperties.limits.lineWidthRange[0]) + " : " + std::to_string(deviceProperties.limits.lineWidthRange[1]));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Bound Descriptor Sets: ") + std::to_string(deviceProperties.limits.maxBoundDescriptorSets));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Clip Distances: ") + std::to_string(deviceProperties.limits.maxClipDistances));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Color Attachments: ") + std::to_string(deviceProperties.limits.maxColorAttachments));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Combined Clip and Cull Distances: ") + std::to_string(deviceProperties.limits.maxCombinedClipAndCullDistances));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Compute Shared Memory Size: ") + std::to_string(deviceProperties.limits.maxComputeSharedMemorySize));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Compute Work Group Count: ") + std::to_string(deviceProperties.limits.maxComputeWorkGroupCount[0]) + " " + std::to_string(deviceProperties.limits.maxComputeWorkGroupCount[1]) + " " + std::to_string(deviceProperties.limits.maxComputeWorkGroupCount[2]));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Compute Work Group Invocations: ") + std::to_string(deviceProperties.limits.maxComputeWorkGroupInvocations));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Compute Work Group Size: ") + std::to_string(deviceProperties.limits.maxComputeWorkGroupSize[0]) + " " + std::to_string(deviceProperties.limits.maxComputeWorkGroupSize[1]) + " " + std::to_string(deviceProperties.limits.maxComputeWorkGroupSize[2]));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Cull Distances: ") + std::to_string(deviceProperties.limits.maxCullDistances));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Descriptor Set Input Attachments: ") + std::to_string(deviceProperties.limits.maxDescriptorSetInputAttachments));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Descriptor Set Sampled Images: ") + std::to_string(deviceProperties.limits.maxDescriptorSetSampledImages));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Descriptor Set Samplers: ") + std::to_string(deviceProperties.limits.maxDescriptorSetSamplers));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Descriptor Set Storage Buffers: ") + std::to_string(deviceProperties.limits.maxDescriptorSetStorageBuffers));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Descriptor Set Storage Buffers Dynamic: ") + std::to_string(deviceProperties.limits.maxDescriptorSetStorageBuffersDynamic));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Descriptor Set Storage Images: ") + std::to_string(deviceProperties.limits.maxDescriptorSetStorageImages));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Descriptor Set Storage Uniform Buffers: ") + std::to_string(deviceProperties.limits.maxDescriptorSetUniformBuffers));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Descriptor Set Storage Uniform Buffers Dynamic: ") + std::to_string(deviceProperties.limits.maxDescriptorSetUniformBuffersDynamic));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Draw Indexed Index Value: ") + std::to_string(deviceProperties.limits.maxDrawIndexedIndexValue));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Draw Indirect Count: ") + std::to_string(deviceProperties.limits.maxDrawIndirectCount));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Fragment Combined Output Resources: ") + std::to_string(deviceProperties.limits.maxFragmentCombinedOutputResources));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Fragment Dual Src Attachments: ") + std::to_string(deviceProperties.limits.maxFragmentDualSrcAttachments));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Fragment Input Components: ") + std::to_string(deviceProperties.limits.maxFragmentInputComponents));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Fragment Output Attachments: ") + std::to_string(deviceProperties.limits.maxFragmentOutputAttachments));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Framebuffer Height: ") + std::to_string(deviceProperties.limits.maxFramebufferHeight));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Framebuffer Layers: ") + std::to_string(deviceProperties.limits.maxFramebufferLayers));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Framebuffer Width: ") + std::to_string(deviceProperties.limits.maxFramebufferWidth));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Geometry Input Components: ") + std::to_string(deviceProperties.limits.maxGeometryInputComponents));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Geometry Output Components: ") + std::to_string(deviceProperties.limits.maxGeometryOutputComponents));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Geometry Output Vertices: ") + std::to_string(deviceProperties.limits.maxGeometryOutputVertices));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Geometry Shader Invocations: ") + std::to_string(deviceProperties.limits.maxGeometryShaderInvocations));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Geometry Total Output Components: ") + std::to_string(deviceProperties.limits.maxGeometryTotalOutputComponents));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Image Array Layers: ") + std::to_string(deviceProperties.limits.maxImageArrayLayers));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Image Dimension 1D: ") + std::to_string(deviceProperties.limits.maxImageDimension1D));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Image Dimension 2D: ") + std::to_string(deviceProperties.limits.maxImageDimension2D));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Image Dimension 3D: ") + std::to_string(deviceProperties.limits.maxImageDimension3D));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Image Dimension Cube: ") + std::to_string(deviceProperties.limits.maxImageDimensionCube));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Interpolation Offset: ") + std::to_string(deviceProperties.limits.maxInterpolationOffset));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Memory Allocation Count: ") + std::to_string(deviceProperties.limits.maxMemoryAllocationCount));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max PerStage Descriptor Input Attachments: ") + std::to_string(deviceProperties.limits.maxPerStageDescriptorInputAttachments));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Per Stage Descriptor Sampled Images: ") + std::to_string(deviceProperties.limits.maxPerStageDescriptorSampledImages));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Per Stage Descriptor Samplers: ") + std::to_string(deviceProperties.limits.maxPerStageDescriptorSamplers));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Per Stage Descriptor Storage Buffers: ") + std::to_string(deviceProperties.limits.maxPerStageDescriptorStorageBuffers));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Per Stage Descriptor Storage Images: ") + std::to_string(deviceProperties.limits.maxPerStageDescriptorStorageImages));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Per Stage Descriptor Uniform Buffers: ") + std::to_string(deviceProperties.limits.maxPerStageDescriptorUniformBuffers));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Per Stage Resources: ") + std::to_string(deviceProperties.limits.maxPerStageResources));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Push Constants Size: ") + std::to_string(deviceProperties.limits.maxPushConstantsSize));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Sample Mask Words: ") + std::to_string(deviceProperties.limits.maxSampleMaskWords));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Sampler Allocation Count: ") + std::to_string(deviceProperties.limits.maxSamplerAllocationCount));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Sampler Anisotropy: ") + std::to_string(deviceProperties.limits.maxSamplerAnisotropy));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Sampler Lod Bias: ") + std::to_string(deviceProperties.limits.maxSamplerLodBias));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Storage Buffer Range: ") + std::to_string(deviceProperties.limits.maxStorageBufferRange));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Tessellation Control Per Patch Output Components: ") + std::to_string(deviceProperties.limits.maxTessellationControlPerPatchOutputComponents));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Tessellation Control Per Vertex Input Components: ") + std::to_string(deviceProperties.limits.maxTessellationControlPerVertexInputComponents));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Tessellation Control Per Vertex Output Components: ") + std::to_string(deviceProperties.limits.maxTessellationControlPerVertexOutputComponents));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Tessellation Control Total Output Components: ") + std::to_string(deviceProperties.limits.maxTessellationControlTotalOutputComponents));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Tessellation Evaluation Input Components: ") + std::to_string(deviceProperties.limits.maxTessellationEvaluationInputComponents));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Tessellation Evaluation Output Components: ") + std::to_string(deviceProperties.limits.maxTessellationEvaluationOutputComponents));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Tessellation Generation Level: ") + std::to_string(deviceProperties.limits.maxTessellationGenerationLevel));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Tessellation Patch Size: ") + std::to_string(deviceProperties.limits.maxTessellationPatchSize));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Texel Buffer Elements: ") + std::to_string(deviceProperties.limits.maxTexelBufferElements));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Texel Gather Offset: ") + std::to_string(deviceProperties.limits.maxTexelGatherOffset));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Texel Offset: ") + std::to_string(deviceProperties.limits.maxTexelOffset));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Uniform Buffer Range: ") + std::to_string(deviceProperties.limits.maxUniformBufferRange));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Vertex Input Attribute Offset: ") + std::to_string(deviceProperties.limits.maxVertexInputAttributeOffset));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Vertex Input Attributes: ") + std::to_string(deviceProperties.limits.maxVertexInputAttributes));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Vertex Input Bindings: ") + std::to_string(deviceProperties.limits.maxVertexInputBindings));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Vertex Input Bindings Stride: ") + std::to_string(deviceProperties.limits.maxVertexInputBindingStride));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Vertex Output Components: ") + std::to_string(deviceProperties.limits.maxVertexOutputComponents));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Viewport Dimensions: ") + std::to_string(deviceProperties.limits.maxViewportDimensions[0]) + " : " + std::to_string(deviceProperties.limits.maxViewportDimensions[1]));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Max Viewports: ") + std::to_string(deviceProperties.limits.maxViewports));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Min Interpolation Offset: ") + std::to_string(deviceProperties.limits.minInterpolationOffset));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Min Memory Map Alignment: ") + std::to_string(deviceProperties.limits.minMemoryMapAlignment));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Min Storage Buffer Offset Alignment: ") + std::to_string(deviceProperties.limits.minStorageBufferOffsetAlignment));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Min Texel Buffer Offset Alignment: ") + std::to_string(deviceProperties.limits.minTexelBufferOffsetAlignment));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Min Texel Gather Offset: ") + std::to_string(deviceProperties.limits.minTexelGatherOffset));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Min Texel Offset: ") + std::to_string(deviceProperties.limits.minTexelOffset));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Min Uniform Buffer Offset Alignment: ") + std::to_string(deviceProperties.limits.minUniformBufferOffsetAlignment));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Mipmap Precision Bits: ") + std::to_string(deviceProperties.limits.mipmapPrecisionBits));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Non Coherent Atom Size: ") + std::to_string(deviceProperties.limits.nonCoherentAtomSize));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Optimal Buffer Copy Offset Alignment: ") + std::to_string(deviceProperties.limits.optimalBufferCopyOffsetAlignment));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Optimal Buffer Copy Row Pitch Alignment: ") + std::to_string(deviceProperties.limits.optimalBufferCopyRowPitchAlignment));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Point Size Range: ") + std::to_string(deviceProperties.limits.pointSizeRange[0]) + " : " + std::to_string(deviceProperties.limits.pointSizeRange[1]));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Point Size Granularity: ") + std::to_string(deviceProperties.limits.pointSizeGranularity));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Sampled Image Color Sample Counts: ") + std::to_string(deviceProperties.limits.sampledImageColorSampleCounts));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Sampled Image Depth Sample Counts: ") + std::to_string(deviceProperties.limits.sampledImageDepthSampleCounts));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Sampled Image Integer Sample Counts: ") + std::to_string(deviceProperties.limits.sampledImageIntegerSampleCounts));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Sampled Image Stencil Sample Counts: ") + std::to_string(deviceProperties.limits.sampledImageStencilSampleCounts));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Sparse Address SpaceSize: ") + std::to_string(deviceProperties.limits.sparseAddressSpaceSize));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Standard Sample Locations: ") + std::to_string(deviceProperties.limits.standardSampleLocations));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Storage Image Sample Counts: ") + std::to_string(deviceProperties.limits.storageImageSampleCounts));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Strict Lines: ") + std::to_string(deviceProperties.limits.strictLines));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Sub Pixel Interpolation Offset Bits: ") + std::to_string(deviceProperties.limits.subPixelInterpolationOffsetBits));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Sub Pixel Precision Bits: ") + std::to_string(deviceProperties.limits.subPixelPrecisionBits));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Sub Texel Precision Bits: ") + std::to_string(deviceProperties.limits.subTexelPrecisionBits));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Timestamp Compute and Graphics: ") + std::to_string(deviceProperties.limits.timestampComputeAndGraphics));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Timestamp Period: ") + std::to_string(deviceProperties.limits.timestampPeriod));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Viewport Bounds Range: ") + std::to_string(deviceProperties.limits.viewportBoundsRange[0]) + " : " + std::to_string(deviceProperties.limits.viewportBoundsRange[1]));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Viewport Sub Pixel Bits: ") + std::to_string(deviceProperties.limits.viewportSubPixelBits));

	std::string UUID("Pipeline Cache UUID: ");

	for (uint32_t i = 0; i < VK_UUID_SIZE; i++)
	{
		UUID += std::to_string(deviceProperties.pipelineCacheUUID[i]) + " ";
	}

	vulkanPhysicalDeviceLog.append(whiteSpace + UUID);
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Sparse Properties: "));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Residency Aligned Mip Size: ") + std::to_string(deviceProperties.sparseProperties.residencyAlignedMipSize));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Residency Non Resident Strict: ") + std::to_string(deviceProperties.sparseProperties.residencyNonResidentStrict));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Residency Standard 2D Block Shape: ") + std::to_string(deviceProperties.sparseProperties.residencyStandard2DBlockShape));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Residency Standard 2D Multisample Block Shape: ") + std::to_string(deviceProperties.sparseProperties.residencyStandard2DMultisampleBlockShape));
	vulkanPhysicalDeviceLog.append(whiteSpace + tab + std::string("Residency Standard 3D Block Shape: ") + std::to_string(deviceProperties.sparseProperties.residencyStandard3DBlockShape));

	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("VendorID: ") + std::to_string(deviceProperties.vendorID));
}

void Renderer::LogVulkanPhysicalDeviceFeatures(const VkPhysicalDeviceFeatures& features, std::string& vulkanPhysicalDeviceLog)
{
	vulkanPhysicalDeviceLog.append(std::string("\n\t Device Features: "));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Alpha To One: ") + std::to_string(features.alphaToOne));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Depth Bias Clamp: ") + std::to_string(features.depthBiasClamp));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Depth Bounds: ") + std::to_string(features.depthBounds));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Depth Clamp: ") + std::to_string(features.depthClamp));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Draw Indirect First Instance: ") + std::to_string(features.drawIndirectFirstInstance));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Dual Src Blend: ") + std::to_string(features.dualSrcBlend));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Fill Mode Non Solid: ") + std::to_string(features.fillModeNonSolid));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Fragment Stores and Atomics: ") + std::to_string(features.fragmentStoresAndAtomics));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Full Draw Index Uint32: ") + std::to_string(features.fullDrawIndexUint32));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Geometry Shader: ") + std::to_string(features.geometryShader));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Image Cube Array: ") + std::to_string(features.imageCubeArray));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Independent Blend: ") + std::to_string(features.independentBlend));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Inherited Queries: ") + std::to_string(features.inheritedQueries));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Large Points: ") + std::to_string(features.largePoints));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Logic Op: ") + std::to_string(features.logicOp));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Multi Draw Indirect: ") + std::to_string(features.multiDrawIndirect));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Multi Viewport: ") + std::to_string(features.multiViewport));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Occlusion Query Precise: ") + std::to_string(features.occlusionQueryPrecise));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Pipeline Statistics Query: ") + std::to_string(features.pipelineStatisticsQuery));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Robust Buffer Access: ") + std::to_string(features.robustBufferAccess));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Sampler Anisotropy: ") + std::to_string(features.samplerAnisotropy));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Sample Rate Shading: ") + std::to_string(features.sampleRateShading));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Shader Clip Distance: ") + std::to_string(features.shaderClipDistance));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Shader Cull Distance: ") + std::to_string(features.shaderCullDistance));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Shader Float64: ") + std::to_string(features.shaderFloat64));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Shader Image Gather Extended: ") + std::to_string(features.shaderImageGatherExtended));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Shader Int16: ") + std::to_string(features.shaderInt16));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Shader Int64: ") + std::to_string(features.shaderInt64));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Shader Resource Min Lod: ") + std::to_string(features.shaderResourceMinLod));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Shader Resource Residency: ") + std::to_string(features.shaderResourceResidency));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Shader Sampled Image Array Dynamic Indexing: ") + std::to_string(features.shaderSampledImageArrayDynamicIndexing));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Shader Storage Buffer Array Dynamic Indexing: ") + std::to_string(features.shaderStorageBufferArrayDynamicIndexing));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Shader Storage Image Array Dynamic Indexing: ") + std::to_string(features.shaderStorageImageArrayDynamicIndexing));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Shader Storage Image Extended Formats: ") + std::to_string(features.shaderStorageImageExtendedFormats));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Shader Storage Image Multisample: ") + std::to_string(features.shaderStorageImageMultisample));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Shader Storage Image Read Without Format: ") + std::to_string(features.shaderStorageImageReadWithoutFormat));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Shader Storage Image Write Without Format: ") + std::to_string(features.shaderStorageImageWriteWithoutFormat));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Shader Tessellation and Geometry Point Size: ") + std::to_string(features.shaderTessellationAndGeometryPointSize));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Shader Uniform Buffer Array Dynamic Indexing: ") + std::to_string(features.shaderUniformBufferArrayDynamicIndexing));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Sparse Binding: ") + std::to_string(features.sparseBinding));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Sparse Residency 16 Samples: ") + std::to_string(features.sparseResidency16Samples));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Sparse Residency 2 Samples: ") + std::to_string(features.sparseResidency2Samples));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Sparse Residency 4 Samples: ") + std::to_string(features.sparseResidency4Samples));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Sparse Residency 8 Samples: ") + std::to_string(features.sparseResidency8Samples));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Sparse Residency Aliased: ") + std::to_string(features.sparseResidencyAliased));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Sparse Residency Buffer: ") + std::to_string(features.sparseResidencyBuffer));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Sparse Residency Image 2D: ") + std::to_string(features.sparseResidencyImage2D));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Sparse Residency Image 3D: ") + std::to_string(features.sparseResidencyImage3D));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Tessellation Shader: ") + std::to_string(features.tessellationShader));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Texture Compression ASTC_LDR: ") + std::to_string(features.textureCompressionASTC_LDR));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Texture Compression BC: ") + std::to_string(features.textureCompressionBC));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Texture Compression ETC2: ") + std::to_string(features.textureCompressionETC2));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Variable Multisample Rate: ") + std::to_string(features.variableMultisampleRate));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Vertex Pipeline Stores and Atomics: ") + std::to_string(features.vertexPipelineStoresAndAtomics));
	vulkanPhysicalDeviceLog.append(whiteSpace + std::string("Wide Lines: ") + std::to_string(features.wideLines));
}

VKAPI_ATTR VkBool32 VKAPI_CALL Renderer::VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	Logger::Category logCategory;

	bool logToConsole = true;
	bool logToLogFile = true;

;	switch (messageSeverity)
	{
	case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		logCategory = Logger::Category::Error;
		break;
	case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		logCategory = Logger::Category::Info;
	case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		logCategory = Logger::Category::Info;
		break;
	case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		logCategory = Logger::Category::Warning;
		break;
	default:
		logCategory = Logger::Category::Info;
		break;
	}

	std::string additionalPrefix("");

	// Some event has happened that is unrelated to the specification or performance
	if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
	{
		additionalPrefix.append("General: ");
		logToConsole = false;
	}

	// Something has happened that violates the specification or indicates a possible mistake
	if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
	{
		additionalPrefix.append("Validation: ");
	}

	// Potential non-optimal use of Vulkan
	if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
	{
		additionalPrefix.append("Performance: ");
	}

	Logger::Log(additionalPrefix + std::string(pCallbackData->pMessage), logCategory, logToConsole, logToLogFile);

	return VK_FALSE;
}
