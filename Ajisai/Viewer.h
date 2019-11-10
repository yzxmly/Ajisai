#pragma once
#include "Core.h"
#include "Model.h"
#include "Device.h"
#include "Object.h"
#include "Camera.h"

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
	glm::vec4 camPosition;
};

struct LightSpaceUniformBufferObject {
	glm::mat4 matToWorld;
	glm::mat4 matToLight;
	glm::mat4 matToFrustum;
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
	bool m_windowResized = false;

	// Vulkan objects
	Ajisai::Device mDevice;

	// Window surface objects
	VkSurfaceKHR m_surface;
	VkSwapchainKHR m_swapChain;
	std::vector<VkImage> m_swapChainImages;
	VkFormat m_swapChainImageFormat;
	VkExtent2D m_swapChainExtent;
	std::vector<VkImageView> m_swapChainImageViews;

	// models
	struct {
		Ajisai::Model debugScreen;
		Ajisai::Model ground;
		Ajisai::Model nanoSuit;
	} mModels;

	// Pipeline Object
	VkDescriptorPool m_descriptorPool;

	struct {
		VkRenderPass debug;
	} mRenderPasses;

	struct {
		std::vector<VkFramebuffer> debug;
	} mFrameBuffers;

	struct {
		VkPipeline shadowMap;
		VkPipeline offscreen;
		VkPipeline deferred;
		VkPipeline debug;
	} mPipelines;

	struct {
		VkPipelineLayout shadowMap;
		VkPipelineLayout offscreen;
		VkPipelineLayout deferred;
		VkPipelineLayout debug;
	} mPipelineLayouts;

	struct{
		int32_t width, height;
		VkFramebuffer frameBuffer;
		Ajisai::Image position, normal, diffuse;
		Ajisai::Image depth;
		VkRenderPass renderPass;
	} mOffscreenFrameBuffer;

	struct {
		int32_t width, height;
		VkFramebuffer frameBuffer;
		Ajisai::Image depth;
		VkRenderPass renderPass;
		VkSampler depthSampler;
	} mShadowMapFrameBuffer;

	struct {
		VkDescriptorSetLayout debugSampler;
		VkDescriptorSetLayout shadowMapUniform;
		VkDescriptorSetLayout offscreenUniform;
		VkDescriptorSetLayout offscreenSampler;
		VkDescriptorSetLayout deferredSampler;
	} mDescriptorSetLayouts;

	VkSampler mSampler;

	// descriptorsets 
	struct {
		std::vector<VkDescriptorSet> debugSampler;
		std::vector<VkDescriptorSet> shadowMapUniform;
		std::vector<VkDescriptorSet> offscreenUniform;
		std::vector<VkDescriptorSet> offscreenSampler;
	} mDescriptorSets;

	struct {
		Ajisai::Buffer shadowLightVS;
		Ajisai::Buffer deferredLightFS;
		std::vector<Ajisai::Buffer> offscreenVS;
		std::vector<Ajisai::Buffer> shadowMapVS;
	} mUniformBuffers;

	// commandbuffer
	struct {
		std::vector<VkCommandBuffer> debug;
		std::vector<VkCommandBuffer> deferred;
		std::vector<VkCommandBuffer> offscreen;
		std::vector<VkCommandBuffer> shadowMap;
	} mCommandBuffers;
	
	// Synchronization
	static const int MAX_FRAMES_IN_FLIGHT = 3;

	struct {
		std::vector<VkSemaphore> imageAvailable;
		std::vector<VkSemaphore> offscreenFinished;
		std::vector<VkSemaphore> shadowMapFinished;
		std::vector<VkSemaphore> renderingFinished;
	} mSemaphores;

	std::vector<VkFence> m_inFlightFences;
	size_t m_currentFrame = 0;

	Ajisai::Camera mCam;

	std::vector<VkBuffer> m_uniformBuffers;
	std::vector<VkDeviceMemory> m_uniformBuffersMemory;

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

	void CreateCommandPool();

	// Initialize Window Surface
	void CreateSurface();
	
	void CreateSwapChain();
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void CreateSwapChainImageViews();

	// models
	void InitObjects();

	// Renderpass Pipeline and Framebuffer
	void CreateDebugPipeline();
	void CreateDebugRenderPass();
	void CreateDebugFrameBuffers();
	void CreateDebugDescriptorSetLayout();
	void CreateDebugDescriptorSets();
	void CreateDebugCommandBuffers();

	void CreateOffscreenFramebuffer();
	void CreateOffscreenDescriptorSetLayout();
	void CreateOffscreenPipeline();
	void CreateOffscreenDiscriptorSets();
	void CreateOffscreenCommandBuffers();

	void CreateShadowFramebuffer();
	void CreateShadowDescriptorSetLayout();
	void CreateShadowPipeline();
	void CreateShadowDiscriptorSets();
	void CreateShadowCommandBuffers();

	VkShaderModule CreateShaderModule(const std::vector<char>& code);

	// Synchronization
	void CreateSyncObjects();

	// Buffers
	void CreateUniformBuffers();
	void UpdateUniformBuffer(uint32_t imageIndex);
	void CreateDescriptorPool();

	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat FindDepthFormat();
	bool HasStencilComponent(VkFormat format);

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	static std::vector<char> ReadFile(const std::string& filename);

	void CreateTextureSampler();

	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

	void MainLoop();
	void DrawFrame();

	void CleanUp();
	void CleanupSwapChain();
};

