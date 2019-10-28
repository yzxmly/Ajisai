#pragma once
#include "Core.h"

namespace Ajisai {
	class Device
	{
	public:
		// GLFW object
		int mWidth = 800;
		int mHeight = 600;
		GLFWwindow* mWindow;

		VkInstance mInstance;
		VkPhysicalDevice mPhysicalDevice;
		VkDevice mLogicalDevice;

		VkQueue mGraphicsQueue;

		VkDebugUtilsMessengerEXT mDebugMessenger;

		// window surface objects
		VkSurfaceKHR mSurface;
		VkQueue mPresentQueue;
		
		// swapchain objects
		VkSwapchainKHR mSwapChain;
		VkFormat mSwapChainFormat;
		VkExtent2D mSwapChainExtent;
		std::vector<VkImage> mSwapChainImages;
		std::vector<VkImageView> mSwapChainImageViews;

		// pipeline objects


		// command pool
		VkCommandPool mCommandPool;

		void CmdTransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void CmdCopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
		void CmdCopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void BeginSingleTimeCommands(VkCommandBuffer &commandBuffer);
		void EndSingleTimeCommands(VkCommandBuffer &commandBuffer);
		bool HasStencilComponent(VkFormat format) {
			return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
		}

		void Wait() {
			vkDeviceWaitIdle(mLogicalDevice);
		}
		void CleanUp();
	};
}

