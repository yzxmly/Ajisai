#include "Viewer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else 
const bool enableValidationLayers = true;
#endif

void Viewer::InitWindow() {
#ifndef AJISAI_NEW_IMPLEMENT

#else
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	mWindow = glfwCreateWindow(mWidth, mHeight, "Ajisai", nullptr, nullptr);
	glfwSetWindowUserPointer(mWindow, this);
	glfwSetFramebufferSizeCallback(mWindow, FramebufferResizeCallback);
#endif
}

void Viewer::FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<Viewer*>(glfwGetWindowUserPointer(window));
	app->m_windowResized = true;
	app->mWidth = width;
	app->mHeight = height;
	app->mCam.mAspect = (float)width / (float)height;
	app->mCam.Update();
}

void Viewer::InitVulkan() {
#ifndef AJISAI_NEW_IMPLEMENT
	
#else
	// vulkan device
	CreateInstance();
	SetupDebugMessenger();
	CreateSurface();
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateCommandPool();

	// swapchain
	CreateSwapChain();
	CreateSwapChainImageViews();
	
	// objects 
	InitObjects();

	// debug pipeline
	CreateDebugRenderPass();
	CreateDebugDescriptorSetLayout();
	CreateDebugPipeline();
	CreateDebugFrameBuffers();

	// offscreen pipeline
	CreateOffscreenFramebuffer();
	CreateOffscreenDescriptorSetLayout();
	CreateOffscreenPipeline();

	// shadowMap pipeline
	CreateShadowFramebuffer();
	CreateShadowDescriptorSetLayout();
	CreateShadowPipeline();

	// descriptors
	CreateDescriptorPool();
	CreateUniformBuffers();
	CreateOffscreenDiscriptorSets();
	CreateDebugDescriptorSets();
	CreateShadowDiscriptorSets();

	// commandbuffers
	CreateOffscreenCommandBuffers();
	CreateDebugCommandBuffers();
	CreateShadowCommandBuffers();

	CreateSyncObjects();
#endif
}



void Viewer::RecreateSwapChain() {
#ifndef AJISAI_NEW_IMPLEMENT
	
#else
	int width = 0, height = 0;
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(mWindow, &width, &height);
		glfwWaitEvents();
	}

	mDevice.Wait();

	CleanupSwapChain();

	// swapchain recreation
	CreateSwapChain();
	CreateSwapChainImageViews();

	// debug pipeline
	CreateDebugRenderPass();
	//CreateDebugDescriptorSetLayout();
	CreateDebugPipeline();
	CreateDebugFrameBuffers();

	// offscreen pipeline
	CreateOffscreenFramebuffer();
	//CreateOffscreenDescriptorSetLayout();
	CreateOffscreenPipeline();
	
	// shadowMap pipeline
	CreateShadowFramebuffer();
	CreateShadowPipeline();

	// descriptors
	CreateDescriptorPool();
	CreateUniformBuffers();
	CreateOffscreenDiscriptorSets();
	CreateDebugDescriptorSets();
	CreateShadowDiscriptorSets();

	// commandbuffers
	CreateOffscreenCommandBuffers();
	CreateDebugCommandBuffers();
	CreateShadowCommandBuffers();

#endif
}

void Viewer::CreateInstance() {
#ifndef AJISAI_NEW_IMPLEMENT

#else
	if (enableValidationLayers && !CheckValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available");
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Ajisai";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Yuuri";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	auto requiredExtensions = GetRequiredExtensions();

	createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t> (validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		GenerateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}
	if (vkCreateInstance(&createInfo, nullptr, &(mDevice.mInstance)) != VK_SUCCESS) {
		throw std::runtime_error("failed to crate vulkan instance!");
	}
#endif
}

bool Viewer::CheckValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availabelLayer(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availabelLayer.data());

	for (auto layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availabelLayer) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) return false;
	}

	return true;
}

std::vector<const char*> Viewer::GetRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensionNames;

	glfwExtensionNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensionNames, glfwExtensionNames + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void Viewer::SetupDebugMessenger() {
#ifndef AJISAI_NEW_IMPLEMENT

#else
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	GenerateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(mDevice.mInstance, &createInfo, nullptr, &(mDevice.mDebugMessenger)) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
#endif
}

void Viewer::GenerateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
	createInfo.pUserData = nullptr;
}

VKAPI_ATTR VkBool32 VKAPI_CALL Viewer::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

VkResult Viewer::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void Viewer::CreateSurface() {
#ifndef AJISAI_NEW_IMPLEMENT

#else
	if (glfwCreateWindowSurface(mDevice.mInstance, mWindow, nullptr, &m_surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface");
	}
#endif
}

void Viewer::PickPhysicalDevice() {
#ifndef AJISAI_NEW_IMPLEMENT
	
#else
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(mDevice.mInstance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan supports");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(mDevice.mInstance, &deviceCount, devices.data());

	for (auto device : devices) {
		if (IsDeviceSuitable(device)) {
			mDevice.mPhysicalDevice = device;
			break;
		}
	}

	if (mDevice.mPhysicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU");
	}
#endif
}

bool Viewer::IsDeviceSuitable(VkPhysicalDevice device) {
	QueueFamilyIndices indices = FindQueueFamilies(device);

	auto extensionsSupported = CheckDeviceExetnsionSupport(device);

	bool swapChainAdequate = false;

	if (extensionsSupported) {
		auto swapChainDetail = QuerySwapChainSupport(device);
		swapChainAdequate = !swapChainDetail.formats.empty() && !swapChainDetail.presentModes.empty();
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return indices.IsComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

int Viewer::RateDeviceSuitability(VkPhysicalDevice device) {
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;

	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	int score = 0;

	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		score += 1000;
	}
	std::cout << deviceProperties.limits.maxImageDimension2D << std::endl;
	score += deviceProperties.limits.maxImageDimension2D;

	if (!deviceFeatures.geometryShader) {
		return 0;
	}

	return score;
}

void Viewer::InitObjects() {
	mModels.nanoSuit.BindDevice(mDevice);
	mModels.nanoSuit.LoadObject("Models\\nanosuit\\nanosuit.obj");

	mModels.debugScreen.BindDevice(mDevice);
	mModels.debugScreen.MakeScreen();

	mModels.ground.BindDevice(mDevice);
	mModels.ground.MakeGround(1.0f, 0.0f, 1.0f);

	mCam.Initialize();

	mLight.mPosition = glm::vec3(5.0f, 16.0f, 5.0f);
	mLight.mDirection = glm::normalize(glm::vec3(-.8f, -1.0f, -.8f));

	mLight.GenerateMatToLight();
}

QueueFamilyIndices Viewer::FindQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueCount > 0 && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.IsComplete()) {
			break;
		}

		i++;
	}
	return indices;
}

bool Viewer::CheckDeviceExetnsionSupport(VkPhysicalDevice device) {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto & extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}
	return requiredExtensions.empty();
}

void Viewer::CreateLogicalDevice() {
#ifndef AJISAI_NEW_IMPLEMENT
	
#else
	auto indices = FindQueueFamilies(mDevice.mPhysicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;

	for (auto queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t> (validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(mDevice.mPhysicalDevice, &createInfo, nullptr, &(mDevice.mLogicalDevice)) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device");
	}

	vkGetDeviceQueue(mDevice.mLogicalDevice, indices.graphicsFamily.value(), 0, &mDevice.mGraphicsQueue);
	vkGetDeviceQueue(mDevice.mLogicalDevice, indices.presentFamily.value(), 0, &mDevice.mPresentQueue);
#endif
}

void Viewer::CreateSwapChain() {
#ifndef AJISAI_NEW_IMPLEMENT
	
#else
	auto swapChainSupport = QuerySwapChainSupport(mDevice.mPhysicalDevice);

	auto surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	auto presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	auto extent = ChooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 5;

	std::cout << "image count is " << imageCount << std::endl;

	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	auto indices = FindQueueFamilies(mDevice.mPhysicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(mDevice.mLogicalDevice, &createInfo, nullptr, &m_swapChain)) {
		throw std::runtime_error("failed to create swap chain");
	}

	vkGetSwapchainImagesKHR(mDevice.mLogicalDevice, m_swapChain, &imageCount, nullptr);
	m_swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(mDevice.mLogicalDevice, m_swapChain, &imageCount, m_swapChainImages.data());
	m_swapChainImageFormat = surfaceFormat.format;
	m_swapChainExtent = extent;
#endif
}

SwapChainSupportDetails Viewer::QuerySwapChainSupport(VkPhysicalDevice device) {
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());
	}
	return details;
}

VkSurfaceFormatKHR Viewer::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	return availableFormats[0];
}

VkPresentModeKHR Viewer::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	/*for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			std::cout << "mail box supported" << std::endl;
			return availablePresentMode;
		}
	}*/
	
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Viewer::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		//int width, height;
		glfwGetFramebufferSize(mWindow, &mWidth, &mHeight);
		VkExtent2D actualExtent = {
			static_cast<uint32_t>(mWidth), 
			static_cast<uint32_t>(mHeight) 
		};

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

void Viewer::CreateSwapChainImageViews() {
#ifndef AJISAI_NEW_IMPLEMENT
#else
	m_swapChainImageViews.resize(m_swapChainImages.size());

	for (uint32_t i = 0; i < m_swapChainImages.size(); ++i) {
		m_swapChainImageViews[i] = CreateImageView(m_swapChainImages[i], m_swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}
#endif
}

VkImageView Viewer::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
	
#ifndef AJISAI_NEW_IMPLEMENT	
	
#else
	VkImageViewCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = image;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = format;

	createInfo.subresourceRange.aspectMask = aspectFlags;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(mDevice.mLogicalDevice, &createInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image views!");
	}
	return imageView;
#endif
}

void Viewer::CreateDebugRenderPass() {
#ifndef AJISAI_NEW_IMPLEMENT
	
#else
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = m_swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = nullptr;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 1> attachments = { colorAttachment };
	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassCreateInfo.pAttachments = attachments.data();
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = 1;
	renderPassCreateInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(mDevice.mLogicalDevice, &renderPassCreateInfo, nullptr, &mRenderPasses.debug) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass");
	}
#endif
}

void Viewer::CreateDebugDescriptorSetLayout() {
#ifndef AJISAI_NEW_IMPLEMENT
#else
	std::array<VkDescriptorSetLayoutBinding, 4> bindings = {};
	for (int i = 0; i < 4; i++) {
		bindings[i].binding = i;
		bindings[i].descriptorCount = 1;
		bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		bindings[i].pImmutableSamplers = nullptr;
		bindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	}

	VkDescriptorSetLayoutCreateInfo samplerCreateInfo = {};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	samplerCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	samplerCreateInfo.pBindings = bindings.data();
	std::cout << "here" << std::endl;
	if (vkCreateDescriptorSetLayout(mDevice.mLogicalDevice, &samplerCreateInfo, nullptr, &mDescriptorSetLayouts.debugSampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout");
	}

	
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo uboCreateInfo = {};
	uboCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	uboCreateInfo.bindingCount = 1;
	uboCreateInfo.pBindings = &uboLayoutBinding;

	if (vkCreateDescriptorSetLayout(mDevice.mLogicalDevice, &uboCreateInfo, nullptr, &mDescriptorSetLayouts.debugUniform) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout");
	}
#endif
}

void Viewer::CreateShadowDescriptorSetLayout() {
#ifndef AJISAI_NEW_IMPLEMENT
#else
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo uboCreateInfo = {};
	uboCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	uboCreateInfo.bindingCount = 1;
	uboCreateInfo.pBindings = &uboLayoutBinding;

	if (vkCreateDescriptorSetLayout(mDevice.mLogicalDevice, &uboCreateInfo, nullptr, &mDescriptorSetLayouts.shadowMapUniform) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout");
	}
#endif
}

void Viewer::CreateOffscreenDescriptorSetLayout() {
#ifndef AJISAI_NEW_IMPLEMENT
#else
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo uniformCreateInfo = {};
	uniformCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	uniformCreateInfo.bindingCount = 1;
	uniformCreateInfo.pBindings = &uboLayoutBinding;

	if (vkCreateDescriptorSetLayout(mDevice.mLogicalDevice, &uniformCreateInfo, nullptr, &mDescriptorSetLayouts.offscreenUniform) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout");
	}

	VkDescriptorSetLayoutBinding samplerLayoutBinding_diffuse = {};
	samplerLayoutBinding_diffuse.binding = 0;
	samplerLayoutBinding_diffuse.descriptorCount = 1;
	samplerLayoutBinding_diffuse.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding_diffuse.pImmutableSamplers = nullptr;
	samplerLayoutBinding_diffuse.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding_specular = {};
	samplerLayoutBinding_specular.binding = 1;
	samplerLayoutBinding_specular.descriptorCount = 1;
	samplerLayoutBinding_specular.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding_specular.pImmutableSamplers = nullptr;
	samplerLayoutBinding_specular.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding_normal = {};
	samplerLayoutBinding_normal.binding = 2;
	samplerLayoutBinding_normal.descriptorCount = 1;
	samplerLayoutBinding_normal.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding_normal.pImmutableSamplers = nullptr;
	samplerLayoutBinding_normal.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 3> bindings = { samplerLayoutBinding_diffuse, samplerLayoutBinding_specular, samplerLayoutBinding_normal };

	VkDescriptorSetLayoutCreateInfo samplerCreateInfo = {};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	samplerCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	samplerCreateInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(mDevice.mLogicalDevice, &samplerCreateInfo, nullptr, &mDescriptorSetLayouts.offscreenSampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout");
	}
#endif
}

void Viewer::CreateShadowPipeline() {
#ifndef AJISAI_NEW_IMPLEMENT

#else
	auto vertShaderCode = ReadFile("shadow.vert.spv");

	auto vertShaderModule = CreateShaderModule(vertShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo = {};
	vertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageCreateInfo.module = vertShaderModule;
	vertShaderStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageCreateInfo };

	auto bindingDescription = Ajisai::Vertex::GetBindingDescription();
	auto attributeDescriptions = Ajisai::Vertex::GetAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputStageCreateInfo = {};
	vertexInputStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputStageCreateInfo.vertexBindingDescriptionCount = 1;
	vertexInputStageCreateInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputStageCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputStageCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStageCreateInfo = {};
	inputAssemblyStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyStageCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyStageCreateInfo.primitiveRestartEnable = VK_FALSE;

	VkPipelineRasterizationStateCreateInfo rasterizationStageCreateInfo = {};
	rasterizationStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationStageCreateInfo.depthClampEnable = VK_FALSE;
	rasterizationStageCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationStageCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationStageCreateInfo.lineWidth = 1.0f;
	rasterizationStageCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizationStageCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizationStageCreateInfo.depthBiasEnable = VK_FALSE;
	rasterizationStageCreateInfo.depthBiasConstantFactor = 0.0f;
	rasterizationStageCreateInfo.depthBiasClamp = 0.0f;
	rasterizationStageCreateInfo.depthBiasSlopeFactor = 0.0f;

	VkPipelineDepthStencilStateCreateInfo dsCreateInfo = {};
	dsCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	dsCreateInfo.depthTestEnable = VK_TRUE;
	dsCreateInfo.depthWriteEnable = VK_TRUE;
	dsCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	dsCreateInfo.depthBoundsTestEnable = VK_FALSE;
	dsCreateInfo.minDepthBounds = 0.0f;
	dsCreateInfo.maxDepthBounds = 1.0f;
	dsCreateInfo.stencilTestEnable = VK_FALSE;
	dsCreateInfo.front = {};
	dsCreateInfo.back = {};

	VkPipelineViewportStateCreateInfo viewportStageCreateInfo = {};
	viewportStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStageCreateInfo.viewportCount = 1;
	viewportStageCreateInfo.pViewports = nullptr;
	viewportStageCreateInfo.scissorCount = 1;
	viewportStageCreateInfo.pScissors = nullptr;

	VkPipelineMultisampleStateCreateInfo multisamplingStageCreateInfo = {};
	multisamplingStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisamplingStageCreateInfo.sampleShadingEnable = VK_FALSE;
	multisamplingStageCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	
	VkPipelineColorBlendStateCreateInfo colorBlendStageCreateInfo = {};
	colorBlendStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendStageCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendStageCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendStageCreateInfo.attachmentCount = 0;
	colorBlendStageCreateInfo.pAttachments = nullptr;

	std::array<VkDynamicState, 2> dynamicStages = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
	};

	VkPipelineDynamicStateCreateInfo dynamicStageCreateInfo = {};
	dynamicStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStageCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStages.size());
	dynamicStageCreateInfo.pDynamicStates = dynamicStages.data();

	std::array<VkDescriptorSetLayout, 1> setLayouts = { mDescriptorSetLayouts.shadowMapUniform };
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
	pipelineLayoutCreateInfo.pSetLayouts = setLayouts.data();
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(mDevice.mLogicalDevice, &pipelineLayoutCreateInfo, nullptr, &mPipelineLayouts.shadowMap) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout");
	}

	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stageCount = 1;
	pipelineCreateInfo.pStages = shaderStages;

	pipelineCreateInfo.pVertexInputState = &vertexInputStageCreateInfo;
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStageCreateInfo;
	pipelineCreateInfo.pRasterizationState = &rasterizationStageCreateInfo;
	pipelineCreateInfo.pViewportState = &viewportStageCreateInfo;
	pipelineCreateInfo.pMultisampleState = &multisamplingStageCreateInfo;
	pipelineCreateInfo.pDepthStencilState = &dsCreateInfo;
	pipelineCreateInfo.pColorBlendState = &colorBlendStageCreateInfo;
	pipelineCreateInfo.pDynamicState = &dynamicStageCreateInfo;

	pipelineCreateInfo.layout = mPipelineLayouts.shadowMap;
	pipelineCreateInfo.renderPass = mShadowMapFrameBuffer.renderPass;
	pipelineCreateInfo.subpass = 0;

	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineCreateInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(mDevice.mLogicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &mPipelines.shadowMap) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline");
	}

	vkDestroyShaderModule(mDevice.mLogicalDevice, vertShaderModule, nullptr);
#endif
}

void Viewer::CreateOffscreenPipeline() {
#ifndef AJISAI_NEW_IMPLEMENT

#else
	auto vertShaderCode = ReadFile("shader.vert.spv");
	auto fragShaderCode = ReadFile("shader.frag.spv");

	auto vertShaderModule = CreateShaderModule(vertShaderCode);
	auto fragShaderModule = CreateShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo = {};
	vertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageCreateInfo.module = vertShaderModule;
	vertShaderStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo = {};
	fragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageCreateInfo.module = fragShaderModule;
	fragShaderStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageCreateInfo, fragShaderStageCreateInfo };

	auto bindingDescription = Ajisai::Vertex::GetBindingDescription();
	auto attributeDescriptions = Ajisai::Vertex::GetAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputStageCreateInfo = {};
	vertexInputStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputStageCreateInfo.vertexBindingDescriptionCount = 1;
	vertexInputStageCreateInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputStageCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputStageCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStageCreateInfo = {};
	inputAssemblyStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyStageCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyStageCreateInfo.primitiveRestartEnable = VK_FALSE;

	VkPipelineRasterizationStateCreateInfo rasterizationStageCreateInfo = {};
	rasterizationStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationStageCreateInfo.depthClampEnable = VK_FALSE;
	rasterizationStageCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationStageCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationStageCreateInfo.lineWidth = 1.0f;
	rasterizationStageCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizationStageCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizationStageCreateInfo.depthBiasEnable = VK_FALSE;
	rasterizationStageCreateInfo.depthBiasConstantFactor = 0.0f;
	rasterizationStageCreateInfo.depthBiasClamp = 0.0f;
	rasterizationStageCreateInfo.depthBiasSlopeFactor = 0.0f;

	VkPipelineDepthStencilStateCreateInfo dsCreateInfo = {};
	dsCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	dsCreateInfo.depthTestEnable = VK_TRUE;
	dsCreateInfo.depthWriteEnable = VK_TRUE;
	dsCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	dsCreateInfo.depthBoundsTestEnable = VK_FALSE;
	dsCreateInfo.minDepthBounds = 0.0f;
	dsCreateInfo.maxDepthBounds = 1.0f;
	dsCreateInfo.stencilTestEnable = VK_FALSE;
	dsCreateInfo.front = {};
	dsCreateInfo.back = {};

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)(m_swapChainExtent.width);
	viewport.height = (float)(m_swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0,0 };
	scissor.extent = m_swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportStageCreateInfo = {};
	viewportStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStageCreateInfo.viewportCount = 1;
	viewportStageCreateInfo.pViewports = &viewport;
	viewportStageCreateInfo.scissorCount = 1;
	viewportStageCreateInfo.pScissors = &scissor;

	VkPipelineMultisampleStateCreateInfo multisamplingStageCreateInfo = {};
	multisamplingStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisamplingStageCreateInfo.sampleShadingEnable = VK_FALSE;
	multisamplingStageCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	std::array<VkPipelineColorBlendAttachmentState, 3> colorBlendAttachments = {};
	colorBlendAttachments[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachments[0].blendEnable = VK_FALSE;
	colorBlendAttachments[1].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachments[1].blendEnable = VK_FALSE;
	colorBlendAttachments[2].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachments[2].blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlendStageCreateInfo = {};
	colorBlendStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendStageCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendStageCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendStageCreateInfo.attachmentCount = 3;
	colorBlendStageCreateInfo.pAttachments = colorBlendAttachments.data();

	VkDynamicState dynamicStages[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineDynamicStateCreateInfo dynamicStageCreateInfo = {};
	dynamicStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStageCreateInfo.dynamicStateCount = 2;
	dynamicStageCreateInfo.pDynamicStates = dynamicStages;

	std::array<VkDescriptorSetLayout, 2> setLayouts = { mDescriptorSetLayouts.offscreenUniform, mDescriptorSetLayouts.offscreenSampler };
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
	pipelineLayoutCreateInfo.pSetLayouts = setLayouts.data();
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(mDevice.mLogicalDevice, &pipelineLayoutCreateInfo, nullptr, &mPipelineLayouts.offscreen) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout");
	}

	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stageCount = 2;
	pipelineCreateInfo.pStages = shaderStages;

	pipelineCreateInfo.pVertexInputState = &vertexInputStageCreateInfo;
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStageCreateInfo;
	pipelineCreateInfo.pRasterizationState = &rasterizationStageCreateInfo;
	pipelineCreateInfo.pViewportState = &viewportStageCreateInfo;
	pipelineCreateInfo.pMultisampleState = &multisamplingStageCreateInfo;
	pipelineCreateInfo.pDepthStencilState = &dsCreateInfo;
	pipelineCreateInfo.pColorBlendState = &colorBlendStageCreateInfo;
	pipelineCreateInfo.pDynamicState = nullptr;

	pipelineCreateInfo.layout = mPipelineLayouts.offscreen;
	pipelineCreateInfo.renderPass = mOffscreenFrameBuffer.renderPass;
	pipelineCreateInfo.subpass = 0;

	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineCreateInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(mDevice.mLogicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &mPipelines.offscreen) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline");
	}

	vkDestroyShaderModule(mDevice.mLogicalDevice, vertShaderModule, nullptr);
	vkDestroyShaderModule(mDevice.mLogicalDevice, fragShaderModule, nullptr);
#endif
}

void Viewer::CreateDebugPipeline() {
#ifndef AJISAI_NEW_IMPLEMENT
#else
	auto vertShaderCode = ReadFile("debug.vert.spv");
	auto fragShaderCode = ReadFile("debug.frag.spv");

	auto vertShaderModule = CreateShaderModule(vertShaderCode);
	auto fragShaderModule = CreateShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo = {};
	vertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageCreateInfo.module = vertShaderModule;
	vertShaderStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo = {};
	fragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageCreateInfo.module = fragShaderModule;
	fragShaderStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageCreateInfo, fragShaderStageCreateInfo };

	auto bindingDescription = Ajisai::Vertex::GetBindingDescription();
	auto attributeDescriptions = Ajisai::Vertex::GetAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputStageCreateInfo = {};
	vertexInputStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputStageCreateInfo.vertexBindingDescriptionCount = 1;
	vertexInputStageCreateInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputStageCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputStageCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStageCreateInfo = {};
	inputAssemblyStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyStageCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyStageCreateInfo.primitiveRestartEnable = VK_FALSE;

	VkPipelineRasterizationStateCreateInfo rasterizationStageCreateInfo = {};
	rasterizationStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationStageCreateInfo.depthClampEnable = VK_FALSE;
	rasterizationStageCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationStageCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationStageCreateInfo.lineWidth = 1.0f;
	rasterizationStageCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizationStageCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizationStageCreateInfo.depthBiasEnable = VK_FALSE;
	rasterizationStageCreateInfo.depthBiasConstantFactor = 0.0f;
	rasterizationStageCreateInfo.depthBiasClamp = 0.0f;
	rasterizationStageCreateInfo.depthBiasSlopeFactor = 0.0f;

	VkPipelineDepthStencilStateCreateInfo dsCreateInfo = {};
	dsCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	dsCreateInfo.depthTestEnable = VK_FALSE;
	dsCreateInfo.depthWriteEnable = VK_FALSE;
	dsCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	dsCreateInfo.depthBoundsTestEnable = VK_FALSE;
	dsCreateInfo.minDepthBounds = 0.0f;
	dsCreateInfo.maxDepthBounds = 1.0f;
	dsCreateInfo.stencilTestEnable = VK_FALSE;
	dsCreateInfo.front = {};
	dsCreateInfo.back = {};

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)(m_swapChainExtent.width);
	viewport.height = (float)(m_swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0,0 };
	scissor.extent = m_swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportStageCreateInfo = {};
	viewportStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStageCreateInfo.viewportCount = 1;
	viewportStageCreateInfo.pViewports = &viewport;
	viewportStageCreateInfo.scissorCount = 1;
	viewportStageCreateInfo.pScissors = &scissor;

	VkPipelineMultisampleStateCreateInfo multisamplingStageCreateInfo = {};
	multisamplingStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisamplingStageCreateInfo.sampleShadingEnable = VK_FALSE;
	multisamplingStageCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlendStageCreateInfo = {};
	colorBlendStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendStageCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendStageCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendStageCreateInfo.attachmentCount = 1;
	colorBlendStageCreateInfo.pAttachments = &colorBlendAttachment;

	VkDynamicState dynamicStages[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineDynamicStateCreateInfo dynamicStageCreateInfo = {};
	dynamicStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStageCreateInfo.dynamicStateCount = 2;
	dynamicStageCreateInfo.pDynamicStates = dynamicStages;

	std::array<VkDescriptorSetLayout, 2> setLayouts = { mDescriptorSetLayouts.debugUniform, mDescriptorSetLayouts.debugSampler };
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
	pipelineLayoutCreateInfo.pSetLayouts = setLayouts.data();
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(mDevice.mLogicalDevice, &pipelineLayoutCreateInfo, nullptr, &mPipelineLayouts.debug) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout");
	}

	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stageCount = 2;
	pipelineCreateInfo.pStages = shaderStages;

	pipelineCreateInfo.pVertexInputState = &vertexInputStageCreateInfo;
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStageCreateInfo;
	pipelineCreateInfo.pRasterizationState = &rasterizationStageCreateInfo;
	pipelineCreateInfo.pViewportState = &viewportStageCreateInfo;
	pipelineCreateInfo.pMultisampleState = &multisamplingStageCreateInfo;
	pipelineCreateInfo.pDepthStencilState = &dsCreateInfo;
	pipelineCreateInfo.pColorBlendState = &colorBlendStageCreateInfo;
	pipelineCreateInfo.pDynamicState = nullptr;

	pipelineCreateInfo.layout = mPipelineLayouts.debug;
	pipelineCreateInfo.renderPass = mRenderPasses.debug;
	pipelineCreateInfo.subpass = 0;

	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineCreateInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(mDevice.mLogicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &mPipelines.debug) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline");
	}

	vkDestroyShaderModule(mDevice.mLogicalDevice, vertShaderModule, nullptr);
	vkDestroyShaderModule(mDevice.mLogicalDevice, fragShaderModule, nullptr);
#endif
}


VkShaderModule Viewer::CreateShaderModule(const std::vector<char>& code) {
#ifndef AJISAI_NEW_IMPLEMENT
	
#else
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(mDevice.mLogicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module");
	}
	return shaderModule;
#endif
}

std::vector<char> Viewer::ReadFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer;
}

void Viewer::CreateShadowFramebuffer() {
	mShadowMapFrameBuffer.width = 1000;
	mShadowMapFrameBuffer.height = 1000;

	// depth attachment
	mShadowMapFrameBuffer.depth.CreateImage(&mDevice, mShadowMapFrameBuffer.width, mShadowMapFrameBuffer.height, VK_FORMAT_D16_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	mShadowMapFrameBuffer.depth.CreateImageView(&mDevice, VK_FORMAT_D16_UNORM, VK_IMAGE_ASPECT_DEPTH_BIT);
	
	VkAttachmentDescription attachmentDesc = {};
	attachmentDesc.format = VK_FORMAT_D16_UNORM;
	attachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

	VkAttachmentReference depthReference;
	depthReference.attachment = 0;
	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.pColorAttachments = nullptr;
	subpass.colorAttachmentCount = 0;
	subpass.pDepthStencilAttachment = &depthReference;

	std::array<VkSubpassDependency, 2> dependencies;
	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.pAttachments = &attachmentDesc;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = 2;
	renderPassCreateInfo.pDependencies = dependencies.data();

	if (vkCreateRenderPass(mDevice.mLogicalDevice, &renderPassCreateInfo, nullptr, &mShadowMapFrameBuffer.renderPass) != VK_SUCCESS) {
		throw(std::runtime_error("failed to create offscreen renderPass"));
	}

	VkImageView attachment = mShadowMapFrameBuffer.depth.mImageView;

	VkFramebufferCreateInfo fbufCreateInfo = {};
	fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fbufCreateInfo.pNext = nullptr;
	fbufCreateInfo.renderPass = mShadowMapFrameBuffer.renderPass;
	fbufCreateInfo.attachmentCount = 1;
	fbufCreateInfo.pAttachments = &attachment;
	fbufCreateInfo.width = mShadowMapFrameBuffer.width;
	fbufCreateInfo.height = mShadowMapFrameBuffer.height;
	fbufCreateInfo.layers = 1;

	if (vkCreateFramebuffer(mDevice.mLogicalDevice, &fbufCreateInfo, nullptr, &mShadowMapFrameBuffer.frameBuffer) != VK_SUCCESS) {
		throw(std::runtime_error("failed to create offscreen framebuffer"));
	}

	VkSamplerCreateInfo samplerCreateInfo = {};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.anisotropyEnable = VK_TRUE;
	samplerCreateInfo.maxAnisotropy = 1.0f;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_WHITE;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	samplerCreateInfo.compareEnable = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 1.0f;

	if (vkCreateSampler(mDevice.mLogicalDevice, &samplerCreateInfo, nullptr, &mShadowMapFrameBuffer.depthSampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create sampler");
	}
}

void Viewer::CreateDeferredFramebuffer() {
	mDeferredFrameBuffer.width = mWidth;
	mDeferredFrameBuffer.height = mHeight;

	// all image attachment 
	mDeferredFrameBuffer.position.CreateImage(&mDevice, mWidth, mHeight, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	mDeferredFrameBuffer.position.CreateImageView(&mDevice, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT);

	mDeferredFrameBuffer.normal.CreateImage(&mDevice, mWidth, mHeight, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	mDeferredFrameBuffer.normal.CreateImageView(&mDevice, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT);

	mDeferredFrameBuffer.diffuse.CreateImage(&mDevice, mWidth, mHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	mDeferredFrameBuffer.diffuse.CreateImageView(&mDevice, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

	// depth attachment
	VkFormat depthFormat = FindDepthFormat();
	mDeferredFrameBuffer.depth.CreateImage(&mDevice, mWidth, mHeight, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	mDeferredFrameBuffer.depth.CreateImageView(&mDevice, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

	std::array<VkAttachmentDescription, 5> attachmentDescs = {};
	attachmentDescs[0].format = m_swapChainImageFormat;

	for (uint32_t i = 0; i < 5; ++i) {
		attachmentDescs[i].samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescs[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescs[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescs[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescs[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		if (i == 4) {
			attachmentDescs[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachmentDescs[i].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}
		else if (i == 0) {
			attachmentDescs[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachmentDescs[i].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		}
		else {
			attachmentDescs[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachmentDescs[i].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
	}

	attachmentDescs[0].format = m_swapChainImageFormat;
	attachmentDescs[1].format = mDeferredFrameBuffer.position.mFormat;
	attachmentDescs[2].format = mDeferredFrameBuffer.normal.mFormat;
	attachmentDescs[3].format = mDeferredFrameBuffer.diffuse.mFormat;
	attachmentDescs[4].format = mDeferredFrameBuffer.depth.mFormat;

	std::vector<VkAttachmentReference> firstColorReferences;
	firstColorReferences.push_back({ 1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
	firstColorReferences.push_back({ 2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
	firstColorReferences.push_back({ 3, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });

	VkAttachmentReference depthReference;
	depthReference.attachment = 3;
	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	
	VkAttachmentReference secondColorAttachmentRef = {};
	secondColorAttachmentRef.attachment = 0;
	secondColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	std::array<VkSubpassDescription, 2> subpasses = {};
	subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpasses[0].pColorAttachments = firstColorReferences.data();
	subpasses[0].colorAttachmentCount = static_cast<uint32_t>(firstColorReferences.size());
	subpasses[0].pDepthStencilAttachment = &depthReference;

	subpasses[1].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpasses[1].pColorAttachments = &secondColorAttachmentRef;
	subpasses[1].colorAttachmentCount = 1;
	subpasses[1].pDepthStencilAttachment = &depthReference;

	std::array<VkSubpassDependency, 3> dependencies;
	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = 1;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.pAttachments = attachmentDescs.data();
	renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentDescs.size());
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpasses[0];
	renderPassCreateInfo.dependencyCount = 2;
	renderPassCreateInfo.pDependencies = dependencies.data();

	if (vkCreateRenderPass(mDevice.mLogicalDevice, &renderPassCreateInfo, nullptr, &mOffscreenFrameBuffer.renderPass) != VK_SUCCESS) {
		throw(std::runtime_error("failed to create offscreen renderPass"));
	}

}

void Viewer::CreateOffscreenFramebuffer() {
	mOffscreenFrameBuffer.width = mWidth;
	mOffscreenFrameBuffer.height = mHeight;

	// create all images

	mOffscreenFrameBuffer.position.CreateImage(&mDevice, mWidth, mHeight, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	mOffscreenFrameBuffer.position.CreateImageView(&mDevice, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT);

	mOffscreenFrameBuffer.normal.CreateImage(&mDevice, mWidth, mHeight, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	mOffscreenFrameBuffer.normal.CreateImageView(&mDevice, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT);

	mOffscreenFrameBuffer.diffuse.CreateImage(&mDevice, mWidth, mHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	mOffscreenFrameBuffer.diffuse.CreateImageView(&mDevice, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

	// depth attachment

	VkFormat depthFormat = FindDepthFormat();
	mOffscreenFrameBuffer.depth.CreateImage(&mDevice, mWidth, mHeight, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	mOffscreenFrameBuffer.depth.CreateImageView(&mDevice, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

	std::array<VkAttachmentDescription, 4> attachmentDescs = {};
	for (uint32_t i = 0; i < 4; ++i) {
		attachmentDescs[i].samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescs[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescs[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescs[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescs[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		if (i == 3) {
			attachmentDescs[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachmentDescs[i].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}
		else {
			attachmentDescs[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachmentDescs[i].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
	}

	attachmentDescs[0].format = mOffscreenFrameBuffer.position.mFormat;
	attachmentDescs[1].format = mOffscreenFrameBuffer.normal.mFormat;
	attachmentDescs[2].format = mOffscreenFrameBuffer.diffuse.mFormat;
	attachmentDescs[3].format = mOffscreenFrameBuffer.depth.mFormat;

	std::vector<VkAttachmentReference> colorReferences;
	colorReferences.push_back({ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
	colorReferences.push_back({ 1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
	colorReferences.push_back({ 2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });

	VkAttachmentReference depthReference;
	depthReference.attachment = 3;
	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.pColorAttachments = colorReferences.data();
	subpass.colorAttachmentCount = static_cast<uint32_t>(colorReferences.size());
	subpass.pDepthStencilAttachment = &depthReference;

	std::array<VkSubpassDependency, 2> dependencies;
	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.pAttachments = attachmentDescs.data();
	renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentDescs.size());
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = 2;
	renderPassCreateInfo.pDependencies = dependencies.data();

	if (vkCreateRenderPass(mDevice.mLogicalDevice, &renderPassCreateInfo, nullptr, &mOffscreenFrameBuffer.renderPass) != VK_SUCCESS) {
		throw(std::runtime_error("failed to create offscreen renderPass"));
	}

	std::array<VkImageView, 4> attachments;
	attachments[0] = mOffscreenFrameBuffer.position.mImageView;
	attachments[1] = mOffscreenFrameBuffer.normal.mImageView;
	attachments[2] = mOffscreenFrameBuffer.diffuse.mImageView;
	attachments[3] = mOffscreenFrameBuffer.depth.mImageView;

	VkFramebufferCreateInfo fbufCreateInfo = {};
	fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fbufCreateInfo.pNext = nullptr;
	fbufCreateInfo.renderPass = mOffscreenFrameBuffer.renderPass;
	fbufCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	fbufCreateInfo.pAttachments = attachments.data();
	fbufCreateInfo.width = mWidth;
	fbufCreateInfo.height = mHeight;
	fbufCreateInfo.layers = 1;

	if (vkCreateFramebuffer(mDevice.mLogicalDevice, &fbufCreateInfo, nullptr, &mOffscreenFrameBuffer.frameBuffer) != VK_SUCCESS) {
		throw(std::runtime_error("failed to create offscreen framebuffer"));
	}

	VkSamplerCreateInfo samplerCreateInfo = {};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.anisotropyEnable = VK_TRUE;
	samplerCreateInfo.maxAnisotropy = 16;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	samplerCreateInfo.compareEnable = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 0.0f;

	if (vkCreateSampler(mDevice.mLogicalDevice, &samplerCreateInfo, nullptr, &mSampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create sampler");
	}
}

void Viewer::CreateDebugFrameBuffers() {
#ifndef AJISAI_NEW_IMPLEMENT
#else
	mFrameBuffers.debug.resize(m_swapChainImageViews.size());
	
	for (size_t i = 0; i < m_swapChainImageViews.size(); ++i) {
		std::array<VkImageView, 1> attachments = {
			m_swapChainImageViews[i],
		};

		VkFramebufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = mRenderPasses.debug;
		createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		createInfo.pAttachments = attachments.data();
		createInfo.width = m_swapChainExtent.width;
		createInfo.height = m_swapChainExtent.height;
		createInfo.layers = 1;

		if (vkCreateFramebuffer(mDevice.mLogicalDevice, &createInfo, nullptr, &mFrameBuffers.debug[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer");
		}

	}
#endif
}

void Viewer::CreateCommandPool() {
#ifndef AJISAI_NEW_IMPLEMENT

#else
	QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(mDevice.mPhysicalDevice);

	VkCommandPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	createInfo.flags = 0;

	if (vkCreateCommandPool(mDevice.mLogicalDevice, &createInfo, nullptr, &(mDevice.mCommandPool)) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool");
	}
#endif
}

void Viewer::CreateShadowCommandBuffers() {
#ifndef AJISAI_NEW_IMPLEMENT

#else
	mCommandBuffers.shadowMap.resize(m_swapChainImages.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = mDevice.mCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)mCommandBuffers.shadowMap.size();

	if (vkAllocateCommandBuffers(mDevice.mLogicalDevice, &allocInfo, mCommandBuffers.shadowMap.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffer");
	}

	for (size_t i = 0; i < mCommandBuffers.shadowMap.size(); ++i) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(mCommandBuffers.shadowMap[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = mShadowMapFrameBuffer.renderPass;
		renderPassBeginInfo.framebuffer = mShadowMapFrameBuffer.frameBuffer;
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent.width = mShadowMapFrameBuffer.width;
		renderPassBeginInfo.renderArea.extent.height = mShadowMapFrameBuffer.height;

		std::array<VkClearValue, 1> clearValues = {};
		clearValues[0].depthStencil = { 1.0f, 0 };

		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(mCommandBuffers.shadowMap[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport = {};
		viewport.width = mShadowMapFrameBuffer.width;
		viewport.height = mShadowMapFrameBuffer.height;
		viewport.maxDepth = 1.0f;
		viewport.minDepth = 0.0f;

		vkCmdSetViewport(mCommandBuffers.shadowMap[i], 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.extent.width = mShadowMapFrameBuffer.width;
		scissor.extent.height = mShadowMapFrameBuffer.height;
		scissor.offset.x = 0;
		scissor.offset.y = 0;

		vkCmdSetScissor(mCommandBuffers.shadowMap[i], 0, 1, &scissor);

		vkCmdBindPipeline(mCommandBuffers.shadowMap[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelines.shadowMap);

		for (unsigned int j = 0; j < mModels.nanoSuit.mMeshes.size(); j++) {
			VkBuffer vertexBuffers[] = { mModels.nanoSuit.mMeshes[j].mVertexBuffer.mBuffer };
			std::array<VkDescriptorSet, 1> decriptorSets = { mDescriptorSets.shadowMapUniform[i] };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(mCommandBuffers.shadowMap[i], 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(mCommandBuffers.shadowMap[i], mModels.nanoSuit.mMeshes[j].mIndexBuffer.mBuffer, 0, VK_INDEX_TYPE_UINT16);
			vkCmdBindDescriptorSets(mCommandBuffers.shadowMap[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayouts.shadowMap, 0, decriptorSets.size(), decriptorSets.data(), 0, nullptr);
			vkCmdDrawIndexed(mCommandBuffers.shadowMap[i], static_cast<uint32_t>(mModels.nanoSuit.mMeshes[j].mIndices.size()), 1, 0, 0, 0);
		}

		vkCmdEndRenderPass(mCommandBuffers.shadowMap[i]);

		if (vkEndCommandBuffer(mCommandBuffers.shadowMap[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer");
		}
	}
#endif
}

void Viewer::CreateOffscreenCommandBuffers() {
#ifndef AJISAI_NEW_IMPLEMENT
	
#else
	mCommandBuffers.offscreen.resize(m_swapChainImages.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = mDevice.mCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)mCommandBuffers.offscreen.size();

	if (vkAllocateCommandBuffers(mDevice.mLogicalDevice, &allocInfo, mCommandBuffers.offscreen.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffer");
	}

	for (size_t i = 0; i < mCommandBuffers.offscreen.size(); ++i) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(mCommandBuffers.offscreen[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = mOffscreenFrameBuffer.renderPass;
		renderPassBeginInfo.framebuffer = mOffscreenFrameBuffer.frameBuffer;
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = m_swapChainExtent;

		std::array<VkClearValue, 4> clearValues = {};
		clearValues[0].color = { 0.0f,0.0f,0.0f,1.0f };
		clearValues[1].color = { 0.0f,0.0f,0.0f,1.0f };
		clearValues[2].color = { 0.0f,0.0f,0.0f,1.0f };
		clearValues[3].depthStencil = { 1.0f, 0 };

		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(mCommandBuffers.offscreen[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(mCommandBuffers.offscreen[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelines.offscreen);

		for (unsigned int j = 0; j < mModels.nanoSuit.mMeshes.size(); j++) {
			VkBuffer vertexBuffers[] = { mModels.nanoSuit.mMeshes[j].mVertexBuffer.mBuffer };
			std::array<VkDescriptorSet, 2> decriptorSets = { mDescriptorSets.offscreenUniform[i], mDescriptorSets.offscreenSampler[j] };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(mCommandBuffers.offscreen[i], 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(mCommandBuffers.offscreen[i], mModels.nanoSuit.mMeshes[j].mIndexBuffer.mBuffer, 0, VK_INDEX_TYPE_UINT16);
			vkCmdBindDescriptorSets(mCommandBuffers.offscreen[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayouts.offscreen, 0, decriptorSets.size(), decriptorSets.data(), 0, nullptr);
			vkCmdDrawIndexed(mCommandBuffers.offscreen[i], static_cast<uint32_t>(mModels.nanoSuit.mMeshes[j].mIndices.size()), 1, 0, 0, 0);
		}

		vkCmdEndRenderPass(mCommandBuffers.offscreen[i]);

		if (vkEndCommandBuffer(mCommandBuffers.offscreen[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer");
		}
	}
#endif
}
void Viewer::CreateDebugCommandBuffers() {
#ifndef AJISAI_NEW_IMPLEMENT
	
#else
	mCommandBuffers.debug.resize(mFrameBuffers.debug.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = mDevice.mCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)mCommandBuffers.debug.size();

	if (vkAllocateCommandBuffers(mDevice.mLogicalDevice, &allocInfo, mCommandBuffers.debug.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffer");
	}

	for (size_t i = 0; i < mCommandBuffers.debug.size(); i++) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(mCommandBuffers.debug[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = mRenderPasses.debug;
		renderPassBeginInfo.framebuffer = mFrameBuffers.debug[i];
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = m_swapChainExtent;

		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = { 0.0f,0.0f,0.0f,1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(mCommandBuffers.debug[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(mCommandBuffers.debug[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelines.debug);

		VkBuffer vertexBuffers[] = {	mModels.debugScreen.mMeshes[0].mVertexBuffer.mBuffer };
		std::array<VkDescriptorSet, 2> decriptorSets = { mDescriptorSets.debugUniform[0], mDescriptorSets.debugSampler[0] };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(mCommandBuffers.debug[i], 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(mCommandBuffers.debug[i], mModels.debugScreen.mMeshes[0].mIndexBuffer.mBuffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdBindDescriptorSets(mCommandBuffers.debug[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayouts.debug, 0, decriptorSets.size(), decriptorSets.data(), 0, nullptr);
		vkCmdDrawIndexed(mCommandBuffers.debug[i], static_cast<uint32_t>(mModels.debugScreen.mMeshes[0].mIndices.size()), 1, 0, 0, 0);

		
		vkCmdEndRenderPass(mCommandBuffers.debug[i]);

		if (vkEndCommandBuffer(mCommandBuffers.debug[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer");
		}
	}
#endif
}

void Viewer::CreateSyncObjects() {
#ifndef AJISAI_NEW_IMPLEMENT
	
#else
	m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	mSemaphores.imageAvailable.resize(MAX_FRAMES_IN_FLIGHT);
	mSemaphores.offscreenFinished.resize(MAX_FRAMES_IN_FLIGHT);
	mSemaphores.renderingFinished.resize(MAX_FRAMES_IN_FLIGHT);
	mSemaphores.shadowMapFinished.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		if (vkCreateSemaphore(mDevice.mLogicalDevice, &semaphoreCreateInfo, nullptr, &mSemaphores.imageAvailable[i])
			|| vkCreateSemaphore(mDevice.mLogicalDevice, &semaphoreCreateInfo, nullptr, &mSemaphores.offscreenFinished[i])
			|| vkCreateSemaphore(mDevice.mLogicalDevice, &semaphoreCreateInfo, nullptr, &mSemaphores.renderingFinished[i])
			|| vkCreateSemaphore(mDevice.mLogicalDevice, &semaphoreCreateInfo, nullptr, &mSemaphores.shadowMapFinished[i])
			|| vkCreateFence(mDevice.mLogicalDevice, &fenceCreateInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create synchronization objects for a frame");
		}
	}
#endif
}



void Viewer::CreateUniformBuffers() {
#ifndef AJISAI_NEW_IMPLEMENT
#else
	mCam.Update();

	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	m_uniformBuffers.resize(m_swapChainImages.size());
	m_uniformBuffersMemory.resize(m_swapChainImages.size());

	for (size_t i = 0; i < m_uniformBuffers.size(); ++i) {
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_uniformBuffers[i], m_uniformBuffersMemory[i]);
	}

	VkDeviceSize shadowMapVSBufferSize = sizeof(LightSpaceUniformBufferObject);
	mUniformBuffers.shadowMapVS.resize(m_swapChainImages.size());
	for (size_t i = 0; i < m_swapChainImages.size(); i++) {
		mUniformBuffers.shadowMapVS[i].CreateBuffer(&mDevice, shadowMapVSBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}

	VkDeviceSize deferredLightFSBufferSize = sizeof(DeferredLightSpaceUniformBufferObject);
	mUniformBuffers.deferredLightFS.CreateBuffer(&mDevice, deferredLightFSBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
#endif
}

void Viewer::UpdateUniformBuffer(uint32_t imageIndex) {
#ifndef  AJISAI_NEW_IMPLEMENT
	
#else
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	//time = time + 0.001f;
	UniformBufferObject ubo = {};
	ubo.mat_toWorld = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f)/10.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//ubo.mat_toWorld = glm::mat4(1.0f);
	ubo.mat_toCamera = mCam.mMatToCam;
	//ubo.mat_toCamera = glm::lookAt(glm::vec3(4.0f, 4.0f, 4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.mat_toFrustum = mCam.mMatToFrustum;
	ubo.mat_toFrustum[1][1] *= -1;
	ubo.camPosition = glm::vec4(mCam.mCamPos, 1.0f);

	void* data;
	vkMapMemory(mDevice.mLogicalDevice, m_uniformBuffersMemory[imageIndex], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(mDevice.mLogicalDevice, m_uniformBuffersMemory[imageIndex]);

	LightSpaceUniformBufferObject lightUbo = {};
	lightUbo.matToWorld = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f) / 10.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	lightUbo.matToLight = mLight.mMatToLight;
	lightUbo.matToFrustum = mLight.mMatToFrustrum;
	lightUbo.matToFrustum[1][1] *= -1;

	void* lightdata;
	vkMapMemory(mDevice.mLogicalDevice, mUniformBuffers.shadowMapVS[imageIndex].mBufferMemory, 0, sizeof(lightUbo), 0, &lightdata);
	memcpy(lightdata, &lightUbo, sizeof(lightUbo));
	vkUnmapMemory(mDevice.mLogicalDevice, mUniformBuffers.shadowMapVS[imageIndex].mBufferMemory);

	DeferredLightSpaceUniformBufferObject deferredLightUbo = {};
	deferredLightUbo.matToLight = mLight.mMatToLight;
	deferredLightUbo.matToFrustum = mLight.mMatToFrustrum;
	deferredLightUbo.lightPosition = glm::vec4(mLight.mPosition, 1.0f);
	deferredLightUbo.lightDirection = glm::vec4(mLight.mDirection, 0.0f);
	deferredLightUbo.matToFrustum[1][1] *= -1;

	void* deferredLightData;
	vkMapMemory(mDevice.mLogicalDevice, mUniformBuffers.deferredLightFS.mBufferMemory, 0, sizeof(deferredLightUbo), 0, &deferredLightData);
	memcpy(deferredLightData, &deferredLightUbo, sizeof(deferredLightUbo));
	vkUnmapMemory(mDevice.mLogicalDevice, mUniformBuffers.deferredLightFS.mBufferMemory);
#endif // ! AJISAI_NEW_IMPLEMENT
}

void Viewer::CreateDescriptorPool() {
#ifndef AJISAI_NEW_IMPLEMENT

#else
	std::array<VkDescriptorPoolSize, 2> poolSizes = {};

	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(m_swapChainImages.size() * 2 + 1);

	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(mModels.nanoSuit.mMeshes.size() * 3 + 4);

	VkDescriptorPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	createInfo.pPoolSizes = poolSizes.data();
	createInfo.maxSets = static_cast<uint32_t>(m_swapChainImages.size() * 2 + mModels.nanoSuit.mMeshes.size() + 2);

	if (vkCreateDescriptorPool(mDevice.mLogicalDevice, &createInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool");
	}
#endif
}

void Viewer::CreateOffscreenDiscriptorSets() {
	// uniform buffer
	std::vector<VkDescriptorSetLayout> uniformLayouts(m_swapChainImages.size(), mDescriptorSetLayouts.offscreenUniform);
	VkDescriptorSetAllocateInfo uniformAllocInfo = {};
	uniformAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	uniformAllocInfo.descriptorPool = m_descriptorPool;
	uniformAllocInfo.descriptorSetCount = static_cast<uint32_t>(m_swapChainImages.size());
	uniformAllocInfo.pSetLayouts = uniformLayouts.data();

	mDescriptorSets.offscreenUniform.resize(m_swapChainImages.size());
	if (vkAllocateDescriptorSets(mDevice.mLogicalDevice, &uniformAllocInfo, mDescriptorSets.offscreenUniform.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets");
	}

	for (size_t i = 0; i < m_swapChainImages.size(); i++) {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = m_uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		std::vector<VkWriteDescriptorSet> descriptorWrites;
		descriptorWrites.resize(1);
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = mDescriptorSets.offscreenUniform[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(mDevice.mLogicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	// texture sampler
	std::vector<VkDescriptorSetLayout> samplerLayouts(mModels.nanoSuit.mMeshes.size(), mDescriptorSetLayouts.offscreenSampler);

	VkDescriptorSetAllocateInfo samplerAllocInfo = {};
	samplerAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	samplerAllocInfo.descriptorPool = m_descriptorPool;
	samplerAllocInfo.descriptorSetCount = static_cast<uint32_t>(mModels.nanoSuit.mMeshes.size());
	samplerAllocInfo.pSetLayouts = samplerLayouts.data();

	mDescriptorSets.offscreenSampler.resize(mModels.nanoSuit.mMeshes.size());

	if (vkAllocateDescriptorSets(mDevice.mLogicalDevice, &samplerAllocInfo, mDescriptorSets.offscreenSampler.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets");
	}
	for (size_t i = 0; i < mModels.nanoSuit.mMeshes.size(); i++) {

		std::vector<VkDescriptorImageInfo> imagesInfo = mModels.nanoSuit.TextureInfo(i);

		std::vector<VkWriteDescriptorSet> descriptorWrites;
		descriptorWrites.resize(imagesInfo.size());

		for (unsigned int j = 0; j < imagesInfo.size(); j++) {
			descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[j].dstSet = mDescriptorSets.offscreenSampler[i];
			descriptorWrites[j].dstBinding = j;
			descriptorWrites[j].dstArrayElement = 0;
			descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[j].descriptorCount = 1;
			descriptorWrites[j].pImageInfo = &imagesInfo[j];
		}

		vkUpdateDescriptorSets(mDevice.mLogicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void Viewer::CreateShadowDiscriptorSets() {
	// uniform buffer
	std::vector<VkDescriptorSetLayout> uniformLayouts(m_swapChainImages.size(), mDescriptorSetLayouts.shadowMapUniform);
	VkDescriptorSetAllocateInfo uniformAllocInfo = {};
	uniformAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	uniformAllocInfo.descriptorPool = m_descriptorPool;
	uniformAllocInfo.descriptorSetCount = static_cast<uint32_t>(m_swapChainImages.size());
	uniformAllocInfo.pSetLayouts = uniformLayouts.data();

	mDescriptorSets.shadowMapUniform.resize(m_swapChainImages.size());
	if (vkAllocateDescriptorSets(mDevice.mLogicalDevice, &uniformAllocInfo, mDescriptorSets.shadowMapUniform.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets");
	}

	for (size_t i = 0; i < m_swapChainImages.size(); i++) {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = mUniformBuffers.shadowMapVS[i].mBuffer;
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(LightSpaceUniformBufferObject);

		std::vector<VkWriteDescriptorSet> descriptorWrites;
		descriptorWrites.resize(1);
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = mDescriptorSets.shadowMapUniform[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(mDevice.mLogicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void Viewer::CreateDebugDescriptorSets() {
#ifndef AJISAI_NEW_IMPLEMENT

#else
	
	std::vector<VkDescriptorSetLayout> uniformLayouts(1, mDescriptorSetLayouts.debugUniform);
	VkDescriptorSetAllocateInfo uniformAllocInfo = {};
	uniformAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	uniformAllocInfo.descriptorPool = m_descriptorPool;
	uniformAllocInfo.descriptorSetCount = static_cast<uint32_t>(1);
	uniformAllocInfo.pSetLayouts = uniformLayouts.data();

	mDescriptorSets.debugUniform.resize(1);
	if (vkAllocateDescriptorSets(mDevice.mLogicalDevice, &uniformAllocInfo, mDescriptorSets.debugUniform.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets");
	}

	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = mUniformBuffers.deferredLightFS.mBuffer;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(DeferredLightSpaceUniformBufferObject);

	VkWriteDescriptorSet descriptorWrite = {};
	
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = mDescriptorSets.debugUniform[0];
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &bufferInfo;
	std::cout << "a" << std::endl;
	vkUpdateDescriptorSets(mDevice.mLogicalDevice, static_cast<uint32_t>(1), &descriptorWrite, 0, nullptr);
	std::cout << "b" << std::endl;
	std::vector<VkDescriptorSetLayout> samplerLayouts(1, mDescriptorSetLayouts.debugSampler);
	VkDescriptorSetAllocateInfo samplerAllocInfo = {};
	samplerAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	samplerAllocInfo.descriptorPool = m_descriptorPool;
	samplerAllocInfo.descriptorSetCount = static_cast<uint32_t>(1);
	samplerAllocInfo.pSetLayouts = samplerLayouts.data();

	mDescriptorSets.debugSampler.resize(1);

	if (vkAllocateDescriptorSets(mDevice.mLogicalDevice, &samplerAllocInfo, mDescriptorSets.debugSampler.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets");
	}


	// final buffer should contains position, normal, color from offscreen rendering
	// and shadow map from shadow map rendering
	std::array<VkDescriptorImageInfo, 4> imageInfos = {};
	imageInfos[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfos[0].imageView = mOffscreenFrameBuffer.position.mImageView;
	imageInfos[0].sampler = mSampler;

	imageInfos[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfos[1].imageView = mOffscreenFrameBuffer.normal.mImageView;
	imageInfos[1].sampler = mSampler;

	imageInfos[2].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfos[2].imageView = mOffscreenFrameBuffer.diffuse.mImageView;
	imageInfos[2].sampler = mSampler;

	imageInfos[3].imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
	imageInfos[3].imageView = mShadowMapFrameBuffer.depth.mImageView;
	imageInfos[3].sampler = mShadowMapFrameBuffer.depthSampler;

	std::array<VkWriteDescriptorSet, 4> descriptorWrites = {};

	for (int i = 0; i < 4; i++) {
		descriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[i].dstSet = mDescriptorSets.debugSampler[0];
		descriptorWrites[i].dstBinding = i;
		descriptorWrites[i].dstArrayElement = 0;
		descriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[i].descriptorCount = 1;

		descriptorWrites[i].pImageInfo = &imageInfos[i];

	}

	/*
	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = mOffscreenFrameBuffer.diffuse.mImageView;
	imageInfo.sampler = mSampler;
	//imageInfo.imageView = mShadowMapFrameBuffer.depth.mImageView;
	//imageInfo.sampler = mShadowMapFrameBuffer.depthSampler;

	VkWriteDescriptorSet descriptorWrite = {};
	
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = mDescriptorSets.debugSampler[0];
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrite.descriptorCount = 1;

	descriptorWrite.pImageInfo = &imageInfo;
	*/

	vkUpdateDescriptorSets(mDevice.mLogicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

#endif
}



void Viewer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
#ifndef AJISAI_NEW_IMPLEMENT

#else
	VkBufferCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.size = size;
	createInfo.usage = usage;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(mDevice.mLogicalDevice, &createInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(mDevice.mLogicalDevice, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(mDevice.mLogicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory");
	}

	vkBindBufferMemory(mDevice.mLogicalDevice, buffer, bufferMemory, 0);
#endif
}
uint32_t Viewer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(mDevice.mPhysicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void Viewer::CreateTextureSampler() {
#ifndef AJISAI_NEW_IMPLEMENT
	VkSamplerCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	createInfo.magFilter = VK_FILTER_LINEAR;
	createInfo.minFilter = VK_FILTER_LINEAR;
	createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	createInfo.anisotropyEnable = VK_TRUE;
	createInfo.maxAnisotropy = 16;
	createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	createInfo.unnormalizedCoordinates = VK_FALSE;
	createInfo.compareEnable = VK_FALSE;
	createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	createInfo.mipLodBias = 0.0f;
	createInfo.minLod = 0.0f;
	createInfo.maxLod = 0.0f;

	if (vkCreateSampler(mDevice.mLogicalDevice, &createInfo, nullptr, &m_textureSampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create sampler");
	}
#endif
}

void Viewer::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
#ifndef AJISAI_NEW_IMPLEMENT
	
#else
	VkImage t_image;
	VkImageCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.imageType = VK_IMAGE_TYPE_2D;
	createInfo.extent.width = static_cast<uint32_t>(width);
	createInfo.extent.height = static_cast<uint32_t>(height);
	createInfo.extent.depth = 1;
	createInfo.mipLevels = 1;
	createInfo.arrayLayers = 1;
	createInfo.format = format;
	createInfo.tiling = tiling;
	createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	createInfo.usage = usage;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.flags = 0;

	if (vkCreateImage(mDevice.mLogicalDevice, &createInfo, nullptr, &t_image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image");
	}

	image = t_image;

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(mDevice.mLogicalDevice, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(mDevice.mLogicalDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate texture image memory");
	}

	vkBindImageMemory(mDevice.mLogicalDevice, image, imageMemory, 0);
#endif
}

VkFormat Viewer::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(mDevice.mPhysicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

bool Viewer::HasStencilComponent(VkFormat format) {
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}


VkFormat Viewer::FindDepthFormat() {
	return FindSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}


void Viewer::MainLoop() {
#ifndef AJISAI_NEW_IMPLEMENT

#else
	while (!glfwWindowShouldClose(mWindow)) {
		glfwPollEvents();
		DrawFrame();
	}

	mDevice.Wait();
#endif
}

void Viewer::DrawFrame() {
#ifndef AJISAI_NEW_IMPLEMENT

#else
	
	vkWaitForFences(mDevice.mLogicalDevice, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(mDevice.mLogicalDevice, m_swapChain, UINT64_MAX, mSemaphores.imageAvailable[m_currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		RecreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image");
	}

	UpdateUniformBuffer(imageIndex);

	VkSubmitInfo offscreenSubmitInfo = {};
	offscreenSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	offscreenSubmitInfo.waitSemaphoreCount = 0;
	offscreenSubmitInfo.pWaitSemaphores = nullptr;
	offscreenSubmitInfo.pWaitDstStageMask = nullptr;
	offscreenSubmitInfo.commandBufferCount = 1;
	offscreenSubmitInfo.pCommandBuffers = &mCommandBuffers.offscreen[imageIndex];
	offscreenSubmitInfo.signalSemaphoreCount = 1;
	offscreenSubmitInfo.pSignalSemaphores = &mSemaphores.offscreenFinished[m_currentFrame];

	if (vkQueueSubmit(mDevice.mGraphicsQueue, 1, &offscreenSubmitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
		throw std::runtime_error("failed to sumbit offscreen draw command buufer");
	}

	VkSubmitInfo shadowMapSubmitInfo = {};
	shadowMapSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	shadowMapSubmitInfo.waitSemaphoreCount = 0;
	shadowMapSubmitInfo.pWaitSemaphores = nullptr;
	shadowMapSubmitInfo.pWaitDstStageMask = nullptr;
	shadowMapSubmitInfo.commandBufferCount = 1;
	shadowMapSubmitInfo.pCommandBuffers = &mCommandBuffers.shadowMap[imageIndex];
	shadowMapSubmitInfo.signalSemaphoreCount = 1;
	shadowMapSubmitInfo.pSignalSemaphores = &mSemaphores.shadowMapFinished[m_currentFrame];

	if (vkQueueSubmit(mDevice.mGraphicsQueue, 1, &shadowMapSubmitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
		throw std::runtime_error("failed to sumbit offscreen draw command buufer");
	}

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {
		mSemaphores.imageAvailable[m_currentFrame],
		mSemaphores.offscreenFinished[m_currentFrame],
		mSemaphores.shadowMapFinished[m_currentFrame]
	};

	VkPipelineStageFlags waitStages[] = { 
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
	};

	submitInfo.waitSemaphoreCount = 3;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &mCommandBuffers.debug[imageIndex];

	VkSemaphore signalSemaphores[] = {
		mSemaphores.renderingFinished[m_currentFrame]
	};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(mDevice.mLogicalDevice, 1, &m_inFlightFences[m_currentFrame]);

	if (vkQueueSubmit(mDevice.mGraphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to sumbit draw command buufer");
	}


	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { m_swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(mDevice.mPresentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_windowResized) {
		m_windowResized = false;
		RecreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
#endif
}
void Viewer::CleanUp() {
#ifndef AJISAI_NEW_IMPLEMENT
	
#else
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		vkDestroySemaphore(mDevice.mLogicalDevice, mSemaphores.imageAvailable[i], nullptr);
		vkDestroySemaphore(mDevice.mLogicalDevice, mSemaphores.offscreenFinished[i], nullptr);
		vkDestroySemaphore(mDevice.mLogicalDevice, mSemaphores.renderingFinished[i], nullptr);
		vkDestroySemaphore(mDevice.mLogicalDevice, mSemaphores.shadowMapFinished[i], nullptr);
		vkDestroyFence(mDevice.mLogicalDevice, m_inFlightFences[i], nullptr);
	}

	CleanupSwapChain();

	{
		mModels.nanoSuit.CleanUp();
		mModels.debugScreen.CleanUp();
		mModels.ground.CleanUp();
	}

	{
		vkDestroyDescriptorSetLayout(mDevice.mLogicalDevice, mDescriptorSetLayouts.debugSampler, nullptr);
		vkDestroyDescriptorSetLayout(mDevice.mLogicalDevice, mDescriptorSetLayouts.offscreenUniform, nullptr);
		vkDestroyDescriptorSetLayout(mDevice.mLogicalDevice, mDescriptorSetLayouts.offscreenSampler, nullptr);
		vkDestroyDescriptorSetLayout(mDevice.mLogicalDevice, mDescriptorSetLayouts.shadowMapUniform, nullptr);
	}

	vkDestroySurfaceKHR(mDevice.mInstance, m_surface, nullptr);
	if (enableValidationLayers) {
		mDevice.DestroyDebugUtilsMessengerEXT();
	}
	mDevice.CleanUp();
	std::cout << "Here" << std::endl;
	glfwDestroyWindow(mWindow);
	glfwTerminate();
#endif
}

void Viewer::CleanupSwapChain() {
#ifndef AJISAI_NEW_IMPLEMENT

#else
	{
		for (size_t i = 0; i < mFrameBuffers.debug.size(); ++i) {
			vkDestroyFramebuffer(mDevice.mLogicalDevice, mFrameBuffers.debug[i], nullptr);
		}

		vkDestroyFramebuffer(mDevice.mLogicalDevice, mOffscreenFrameBuffer.frameBuffer, nullptr);
		vkDestroyFramebuffer(mDevice.mLogicalDevice, mShadowMapFrameBuffer.frameBuffer, nullptr);
	}

	{
		vkFreeCommandBuffers(mDevice.mLogicalDevice, mDevice.mCommandPool, static_cast<uint32_t>(mCommandBuffers.debug.size()), mCommandBuffers.debug.data());
		vkFreeCommandBuffers(mDevice.mLogicalDevice, mDevice.mCommandPool, static_cast<uint32_t>(mCommandBuffers.offscreen.size()), mCommandBuffers.offscreen.data());
		vkFreeCommandBuffers(mDevice.mLogicalDevice, mDevice.mCommandPool, static_cast<uint32_t>(mCommandBuffers.shadowMap.size()), mCommandBuffers.shadowMap.data());
	}

	{
		vkDestroyPipeline(mDevice.mLogicalDevice, mPipelines.debug, nullptr);
		vkDestroyPipeline(mDevice.mLogicalDevice, mPipelines.offscreen, nullptr);
		vkDestroyPipeline(mDevice.mLogicalDevice, mPipelines.shadowMap, nullptr);
	}

	{
		vkDestroyPipelineLayout(mDevice.mLogicalDevice, mPipelineLayouts.debug, nullptr);
		vkDestroyPipelineLayout(mDevice.mLogicalDevice, mPipelineLayouts.offscreen, nullptr);
		vkDestroyPipelineLayout(mDevice.mLogicalDevice, mPipelineLayouts.shadowMap, nullptr);
	}

	{
		vkDestroyRenderPass(mDevice.mLogicalDevice, mRenderPasses.debug, nullptr);
		vkDestroyRenderPass(mDevice.mLogicalDevice, mOffscreenFrameBuffer.renderPass, nullptr);
		vkDestroyRenderPass(mDevice.mLogicalDevice, mShadowMapFrameBuffer.renderPass, nullptr);
	}

	{
		mOffscreenFrameBuffer.position.CleanUp(&mDevice);
		mOffscreenFrameBuffer.normal.CleanUp(&mDevice);
		mOffscreenFrameBuffer.diffuse.CleanUp(&mDevice);
		mOffscreenFrameBuffer.depth.CleanUp(&mDevice);

		mShadowMapFrameBuffer.depth.CleanUp(&mDevice);
	}

	{
		vkDestroySampler(mDevice.mLogicalDevice, mSampler, nullptr);
		vkDestroySampler(mDevice.mLogicalDevice, mShadowMapFrameBuffer.depthSampler, nullptr);
	}

	for (size_t i = 0; i < m_swapChainImageViews.size(); ++i) {
		vkDestroyImageView(mDevice.mLogicalDevice, m_swapChainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(mDevice.mLogicalDevice, m_swapChain, nullptr);

	for (size_t i = 0; i < m_uniformBuffers.size(); i++) {
		vkDestroyBuffer(mDevice.mLogicalDevice, m_uniformBuffers[i], nullptr);
		vkFreeMemory(mDevice.mLogicalDevice, m_uniformBuffersMemory[i], nullptr);
	}

	for (size_t i = 0; i < mUniformBuffers.shadowMapVS.size(); i++) {
		vkDestroyBuffer(mDevice.mLogicalDevice, mUniformBuffers.shadowMapVS[i].mBuffer, nullptr);
		vkFreeMemory(mDevice.mLogicalDevice, mUniformBuffers.shadowMapVS[i].mBufferMemory, nullptr);
	}

	vkDestroyDescriptorPool(mDevice.mLogicalDevice, m_descriptorPool, nullptr);
#endif
}

