#pragma once
#include "Core.h"
#include "Model.h"
#include "Device.h"
#include "Object.h"

#define AJISAI_NEW_IMPLEMENT

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool IsComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct UniformBufferObject {
	glm::mat4 mat_toWorld;
	glm::mat4 mat_toCamera;
	glm::mat4 mat_toFrustum;
};

class Viewer
{
public:
	void Run() {
		InitWindow();
		InitVulkan();
		MainLoop();
		CleanUp();
	}
private:
	// GLFW objects
	int mWidth = 800, mHeight = 600;
	GLFWwindow* mWindow;


	// GLFW objects
	int m_width = 800;
	int m_height = 600;
	//float time = 0.0f;
	GLFWwindow* m_window;

	// Vulkan objects
	VkInstance m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkDevice m_device;
	VkQueue m_graphicsQueue;

	Ajisai::Device mDevice;

	// Window surface objects
	VkSurfaceKHR m_surface;
	VkQueue m_presentQueue;
	VkSwapchainKHR m_swapChain;
	std::vector<VkImage> m_swapChainImages;
	VkFormat m_swapChainImageFormat;
	VkExtent2D m_swapChainExtent;
	std::vector<VkImageView> m_swapChainImageViews;

	// Pipeline Object
	VkRenderPass m_renderPass;
	VkPipeline m_pipeline;
	VkDescriptorSetLayout m_descriptorSetLayout;
	VkDescriptorSetLayout mUniformBufferLayout;
	VkDescriptorSetLayout mSamplerLayout;
	VkDescriptorPool m_descriptorPool;
	std::vector<VkDescriptorSet> m_descriptorSets;
	std::vector<VkDescriptorSet> mUniformDescriptorSets;
	std::vector<VkDescriptorSet> mSamplerDescriptorSets;
	VkPipelineLayout m_pipelineLayout;
	std::vector<VkFramebuffer> m_framebuffers;
	bool m_windowResized = false;

	// Command Pool
	VkCommandPool m_commandPool;
	std::vector<VkCommandBuffer> m_commandBuffers;
	
	// Synchronization
	static const int MAX_FRAMES_IN_FLIGHT = 3;
	VkSemaphore m_imageAvailableSemaphore, m_renderFinishedSemaphore;
	std::vector<VkSemaphore> m_imageAvailableSemaphores;
	std::vector<VkSemaphore> m_renderFinishedSemaphores;
	std::vector<VkFence> m_inFlightFences;
	size_t m_currentFrame = 0;

	Ajisai::Object mObject;
	Ajisai::Model mModelNanoSuit;
	void InitObjects();
	// Buffers
	//Model m_model;
	VkBuffer m_vertexBuffer;
	VkDeviceMemory m_vertexBufferMemory;

	VkBuffer m_indexBuffer;
	VkDeviceMemory m_indexBufferMemory;

	std::vector<VkBuffer> m_uniformBuffers;
	std::vector<VkDeviceMemory> m_uniformBuffersMemory;

	VkImage m_textureImage;
	VkDeviceMemory m_textureImageMemory;
	VkImageView m_textureImageView;
	VkSampler m_textureSampler;

	VkImage m_depthImage;
	VkDeviceMemory m_depthImageMemory;
	VkImageView m_depthImageView;

	// Initialize GLFW
	void InitWindow();
	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

	// Initialize Vulkan and recreate
	void InitVulkan();
	void RecreateSwapChain();

	void CreateInstance();
	bool CheckValidationLayerSupport();
	std::vector<const char*> GetRequiredExtensions();
	
	void SetupDebugMessenger();
	void GenerateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	
	void PickPhysicalDevice();
	bool IsDeviceSuitable(VkPhysicalDevice device);
	int RateDeviceSuitability(VkPhysicalDevice device);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	bool CheckDeviceExetnsionSupport(VkPhysicalDevice device);

	void CreateLogicalDevice();

	// Initialize Window Surface
	void CreateSurface();
	
	void CreateSwapChain();
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void CreateSwapChainImageViews();

	void CleanupSwapChain();

	// Renderpass Pipeline and Framebuffer
	void CreateRenderPass();
	void CreateDescriptorSetLayout();
	void CreateGraphicsPipeline();
	VkShaderModule CreateShaderModule(const std::vector<char>& code);
	void CreateFramebuffers();

	// Command Pool and buffer
	void CreateCommandPool();
	void CreateCommandBuffers();

	VkCommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	// Synchronization
	void CreateSyncObjects();

	// Buffers
	void CreateVertexBuffer();
	void CreateIndexBuffer();
	void CreateUniformBuffers();
	void UpdateUniformBuffer(uint32_t imageIndex);
	void CreateDescriptorPool();
	void CreateDescriptorSets();
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void CreateDepthResources();
	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat FindDepthFormat();
	bool HasStencilComponent(VkFormat format);

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	static std::vector<char> ReadFile(const std::string& filename);

	void CreateTextureImage();
	void CreateTextureImageView();
	void CreateTextureSampler();

	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

	void MainLoop();
	void DrawFrame();

	void CleanUp();
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
};

