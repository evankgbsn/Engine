#include "Window.h"

#include <algorithm>
#include <unordered_set>
#include <stdexcept>
#include <chrono>

#include <GLFW/glfw3.h>

#include "../../Utils/Logger.h"
#include "../Renderer.h"
#include "../Vulkan/VulkanPhysicalDevice.h"
#include "../Vulkan/Utils/VulkanUtils.h"
#include "../Pipeline/GraphicsPipeline.h"
#include "../Pipeline/PipelineLayout.h"
#include "../Pipeline/RenderPass/RenderPass.h"
#include "../Pipeline/Viewport/ViewportPipelineState.h"
#include "../Pipeline/Shaders/ShaderPipelineStage.h"
#include "../Commands/CommandManager.h"
#include "../Memory/VertexBuffer.h"
#include "../Memory/StagingBuffer.h"
#include "../Memory/IndexBuffer.h"
#include "../Memory/UniformBuffer.h"
#include "../Memory/MemoryManager.h"
#include "../Pipeline/Shaders/DescriptorSet.h"
#include "../GraphicsObjects/GraphicsObjectManager.h"
#include "../GraphicsObjects/GraphicsObject.h"
#include "../GraphicsObjects/TexturedAnimatedGraphicsObject.h"
#include "../Model/Model.h"
#include "../Model/ModelManager.h"
#include "../Pipeline/Shaders/DescriptorSetManager.h"
#include "../Cameras/CameraManager.h"
#include "../Memory/MemoryManager.h"
#include "../../Time/TimeManager.h"
#include "../Images/TextureManager.h"

#include "glm/gtc/matrix_transform.hpp"

static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

Window::Window(uint32_t w, uint32_t h, std::string&& windowName) :
	name(windowName),
	width(w),
	height(h),
	framebuffers(std::vector<VkFramebuffer>()),
	framebufferResized(false),
	renderPass(nullptr)
{
	CameraManager::Initialize();
	Camera& cam = CameraManager::CreateCamera(Camera::Type::PERSPECTIVE, std::string("MainCamera"), this);
	cam.SetPosition(glm::vec3(0.0f, 2.5f, 10.0f));
	cam.SetTarget(glm::vec3(0.0f, 2.5f, 9.0f));
}

Window::~Window()
{
	CameraManager::Terminate();

	VkDevice& device = Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice();

	vkDeviceWaitIdle(device);

	vkDestroyImageView(device, depthImageView, nullptr);
	vmaDestroyImage(MemoryManager::GetAllocator(), depthImage, depthImageAllocation);

	GraphicsObjectManager::Terminate();
	TextureManager::Terminate();
	MemoryManager::Terminate();

	vkDestroyFence(device, inFlight, nullptr);
	vkDestroySemaphore(device, imageAvailable, nullptr);
	vkDestroySemaphore(device, renderFinished, nullptr);

	CleanupSwapchain();

	delete viewportPipelineState;
	delete renderPass;
	
	vkDestroySurfaceKHR(Renderer::GetVulkanInstance(), surface, nullptr);
}

void Window::Initialize()
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), name.c_str(), nullptr, nullptr);

	if (!window)
	{
		Logger::Log(std::string("Could not create GLFW window"), Logger::Category::Error);
		throw std::runtime_error("Could not create GLFW window");
	}

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);

	// Create the Vulkan Surface.
	VkResult result = glfwCreateWindowSurface(Renderer::GetVulkanInstance(), window, nullptr, &surface);
	if (result != VK_SUCCESS)
	{
		std::string errorStr = std::string("Could not create a surface for window ") + name + std::string(" Window::Window");
		Logger::Log(std::string(errorStr), Logger::Category::Error);
		throw std::runtime_error(errorStr.c_str());
	}

	static bool firstWindow = true;
	if (firstWindow)
	{
		// Choose a device that supports this window.
		Renderer::ChooseDevice(*this);

		MemoryManager::Initialize();

		// Needs to be called before we create the RenderPass in the pipeline for a reference to the depth format.
		CreateDepthBuffer();
		renderPass = new RenderPass(*this);
		viewportPipelineState = new ViewportPipelineState(*this);
		TextureManager::Initialize();
		GraphicsObjectManager::Initialize(*this);
		firstWindow = false;
	}

	// swapchainExtent isnt set until GetSurfaceInfo is called in ChooseDevice.
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapchainExtent.width;
	viewport.height = (float)swapchainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissor.offset = { 0, 0 };
	scissor.extent = swapchainExtent;

	CreateFramebuffers();
	CreateSyncObjects();

	Texture* womanTexture = TextureManager::LoadTexture("../Engine/Engine/Renderer/Images/Woman.png", "Woman");
	Texture* vikingRoomTexture = TextureManager::LoadTexture("../Engine/Engine/Renderer/Images/VikingRoom.png", "VikingRoom");

	Model* womanModel = ModelManager::LoadModel("Woman", "../Engine/Engine/Renderer/Model/Woman.gltf");
	Model* vikingRoom = ModelManager::LoadModel("VikingRoom", "../Engine/Engine/Renderer/Model/VikingRoom.gltf");

	gObj0 = GraphicsObjectManager::CreateTexturedAnimatedGraphicsObject(ModelManager::GetModel("Woman"), womanTexture);
	GraphicsObjectManager::CreateTexturedStaticGraphicsObject(vikingRoom, vikingRoomTexture);
}

bool Window::Update()
{
	if (glfwWindowShouldClose(window))
	{
		return false;
	}

	CheckInput();

	glfwPollEvents();

	Draw();

	return true;
}

const Window::SurfaceInfo& Window::GetSurfaceInfo(const VulkanPhysicalDevice& device)
{
	VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*device, surface, &surfaceInfo.surfaceInfo);

	if (result != VK_SUCCESS)
	{
		Logger::Log(std::string("Could not get surface info for window ") + name + std::string(" using device ") + std::string(device.GetProperties().deviceName), Logger::Category::Error);
	}

	uint32_t formatCount = 0;
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(*device, surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		surfaceInfo.surfaceFormats.resize(formatCount);
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(*device, surface, &formatCount, surfaceInfo.surfaceFormats.data());
	}

	if (result != VK_SUCCESS)
	{
		Logger::Log(std::string("Could not get surface formats for window ") + name + std::string(" using device ") + std::string(device.GetProperties().deviceName), Logger::Category::Error);
	}

	uint32_t presentModeCount = 0;
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(*device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		surfaceInfo.presentModes.resize(presentModeCount);
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(*device, surface, &presentModeCount, surfaceInfo.presentModes.data());
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

	VkSwapchainCreateInfoKHR createInfo{};
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
		VkImageViewCreateInfo imageViewCreateInfo{};
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

void Window::CreateFramebuffers()
{
	framebuffers.resize(swapchainImageViews.size());

	int i = 0;
	for (VkImageView imageView : swapchainImageViews)
	{
		std::vector<VkImageView> attachments = { imageView, depthImageView };

		VkFramebufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = **renderPass;
		createInfo.attachmentCount = static_cast<unsigned int>(attachments.size());
		createInfo.pAttachments = attachments.data();
		createInfo.width = swapchainExtent.width;
		createInfo.height = swapchainExtent.height;
		createInfo.layers = 1;

		VkResult result = vkCreateFramebuffer(Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice(), &createInfo, nullptr, &framebuffers[i]);

		if (result != VK_SUCCESS)
		{
			Logger::Log(std::string("Failed to create a framebuffer."), Logger::Category::Error);
			throw std::runtime_error("Failed to create a framebuffer.");
		}
		
		i++;
	}
}

void Window::RecreateSwapchain()
{
	// Minimization.
	int width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}
	//

	CleanupSwapchain();

	int w, h;
	glfwGetWindowSize(window, &w, &h);
	width = static_cast<unsigned int>(w);
	height = static_cast<unsigned int>(h);

	GetSurfaceInfo(*Renderer::GetVulkanPhysicalDevice());
	
	// swapchainExtent isnt set until GetSurfaceInfo is called in ChooseDevice.
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapchainExtent.width;
	viewport.height = (float)swapchainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissor.offset = { 0, 0 };
	scissor.extent = swapchainExtent;

	CreateSwapchain();

	vkDestroyImageView(Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice(), depthImageView, nullptr);
	vmaDestroyImage(MemoryManager::GetAllocator(), depthImage, depthImageAllocation);
	CreateDepthBuffer();
	CreateFramebuffers();


	framebufferResized = false;

}

void Window::CleanupSwapchain()
{
	const VkDevice& device = Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice();
	vkDeviceWaitIdle(device);

	for (VkFramebuffer framebuffer : framebuffers)
	{
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}

	for (const auto& imageView : swapchainImageViews)
	{
		vkDestroyImageView(device, imageView, nullptr);
	}

	vkDestroySwapchainKHR(device, swapchain, nullptr);
}

const VkFormat& Window::GetDepthFormat() const
{
	return depthFormat;
}

void Window::Draw()
{
	VkDevice& device = Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice();

	vkWaitForFences(device, 1, &inFlight, VK_TRUE, UINT64_MAX);

	uint32_t imageIndex = 0;
	VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailable, VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		RecreateSwapchain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		Logger::Log(std::string("Failed to aquire an image from the swapchain."), Logger::Category::Error);
		throw std::runtime_error("Failed to aquire an image from the swapchain.");
		return;
	}
	
	vkResetFences(device, 1, &inFlight);

	vkResetCommandBuffer(CommandManager::GetRenderCommandBuffer(), 0);

	RecordCommands(imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &imageAvailable;
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &CommandManager::GetRenderCommandBuffer();
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &renderFinished;

	result = vkQueueSubmit(Renderer::GetVulkanPhysicalDevice()->GetGraphicsQueue(), 1, &submitInfo, inFlight);

	if (result != VK_SUCCESS)
	{
		Logger::Log(std::string("Failed to submit command buffer to graphics queue."), Logger::Category::Error);
		throw std::runtime_error("Failed to submit command buffer to graphics queue.");
		return;
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderFinished;

	result = vkQueuePresentKHR(Renderer::GetVulkanPhysicalDevice()->GetPresentationQueue(), &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		RecreateSwapchain();
	}
	else if (result != VK_SUCCESS) {
		Logger::Log(std::string("Failed to present."), Logger::Category::Error);
		throw std::runtime_error("Failed to present.");
		return;
	}
}

void Window::CheckInput()
{
	Camera& cam = CameraManager::GetCamera("MainCamera");

	float speed = 4.0f * TimeManager::DeltaTime();
	float rotSpeed = 2.0f * TimeManager::DeltaTime();

	if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_W))
	{
		cam.Translate(cam.GetForwardVector() * speed);
	}
	
	if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_A))
	{
		cam.Translate(cam.GetRightVector() * -speed);
	}
	
	if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_S))
	{
		cam.Translate(cam.GetForwardVector() * -speed);
	}
	
	if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_D))
	{
		cam.Translate(cam.GetRightVector() * speed);
	}
	
	if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
	{
		cam.Translate(glm::vec3(0.0f, -speed, 0.0f));
	}

	if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_SPACE))
	{
		cam.Translate(glm::vec3(0.0f, speed, 0.0f));
	}

	if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_Q))
	{
		cam.Rotate(cam.GetUpVector(), rotSpeed);
	}

	if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_E))
	{
		cam.Rotate(cam.GetUpVector(), -rotSpeed);
	}

	static bool toggle = false;
	if (GLFW_RELEASE == glfwGetKey(window, GLFW_KEY_1))
	{
		if (toggle == true)
		{
			static unsigned int index = 1U;
			gObj0->SetClip(index++);
			if (index >= 10)
			{
				index = 0;
			}
		}
		toggle = false;
	}

	if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_1))
	{
		toggle = true;
	}

}

void Window::RecordCommands(int imageIndex)
{
	VkCommandBuffer& buffer = CommandManager::GetRenderCommandBuffer();

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	VkResult result = vkBeginCommandBuffer(buffer, &beginInfo);

	if (result != VK_SUCCESS)
	{
		Logger::Log(std::string("Failed to begin command buffer."), Logger::Category::Error);
		throw std::runtime_error("Failed to begin command buffer.");
		return;
	}

	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = **renderPass;
	renderPassBeginInfo.framebuffer = framebuffers[imageIndex];
	renderPassBeginInfo.renderArea.offset = { 0,0 };
	renderPassBeginInfo.renderArea.extent = swapchainExtent;
	VkClearValue clearColor{};
	clearColor.color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	VkClearValue clearDepth{};
	clearDepth.depthStencil = { 1.0f, 0 };
	std::vector<VkClearValue> clearValues = { clearColor, clearDepth };
	renderPassBeginInfo.clearValueCount = static_cast<unsigned int>(clearValues.size());
	renderPassBeginInfo.pClearValues = clearValues.data();

	vkCmdSetViewport(buffer, 0, 1, &viewport);
	vkCmdSetScissor(buffer, 0, 1, &scissor);

	vkCmdBeginRenderPass(buffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	GraphicsObjectManager::DrawObjects(buffer, imageIndex);
	
	vkCmdEndRenderPass(buffer);

	result = vkEndCommandBuffer(buffer);

	if (result != VK_SUCCESS)
	{
		Logger::Log(std::string("Failed to record command buffer."), Logger::Category::Error);
		throw std::runtime_error("Failed to record command buffer.");
	}
}

void Window::CreateSyncObjects()
{
	VkDevice& device = Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice();

	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	
	VkResult result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &imageAvailable);

	if (result != VK_SUCCESS)
	{
		Logger::Log(std::string("Failed to create image available semaphore."), Logger::Category::Error);
		throw std::runtime_error("Failed to create image available semaphore.");
		return;
	}

	result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderFinished);

	if (result != VK_SUCCESS)
	{
		Logger::Log(std::string("Failed to create render finished semaphore."), Logger::Category::Error);
		throw std::runtime_error("Failed to create render finished semaphore.");
		return;
	}

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	result = vkCreateFence(device, &fenceCreateInfo, nullptr, &inFlight);

	if (result != VK_SUCCESS)
	{
		Logger::Log(std::string("Failed to create in flight fence."), Logger::Category::Error);
		throw std::runtime_error("Failed to create in flight fence.");
		return;
	}
}

void Window::CreateDepthBuffer()
{
	depthFormat = FindSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);

	depthImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	depthImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	depthImageCreateInfo.extent.width = static_cast<uint32_t>(width);
	depthImageCreateInfo.extent.height = static_cast<uint32_t>(height);
	depthImageCreateInfo.extent.depth = 1;
	depthImageCreateInfo.mipLevels = 1;
	depthImageCreateInfo.arrayLayers = 1;
	depthImageCreateInfo.format = depthFormat;
	depthImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	depthImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthImageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	depthImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	depthImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	depthImageCreateInfo.flags = 0;

	depthImageAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	depthImageAllocInfo.flags = 0;

	VkResult result = vmaCreateImage(MemoryManager::GetAllocator(), &depthImageCreateInfo, &depthImageAllocInfo, &depthImage, &depthImageAllocation, nullptr);
	VulkanUtils::CheckResult(result, true, true, "Failed to create Depth Buffer image.");

	VkImageViewCreateInfo imageViewCreateInfo{};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = depthImage;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = depthFormat;
	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	result = vkCreateImageView(Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice(), &imageViewCreateInfo, nullptr, &depthImageView);
	VulkanUtils::CheckResult(result, true, true, "Failed to create image view in Window::CreateDepthBuffer().");

}

VkFormat Window::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(*(*Renderer::GetVulkanPhysicalDevice()), format, &props);
		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	Logger::LogAndThrow("Could not find supported format Window::FindSupportedFormat().");

	return VkFormat();
}

static void FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true;
}