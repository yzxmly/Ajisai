#pragma once
#include "Core.h"
#include "Device.h"
namespace Ajisai {
	class Buffer
	{
	public:
		VkBuffer mBuffer;
		VkDeviceMemory mBufferMemory;

		void CreateBuffer(Device* device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

		static uint32_t FindMemoryType(Device* device, uint32_t typeFilter, VkMemoryPropertyFlags properties);
		//static void CopyBuffer(Buffer srcBuffer, Buffer dstBuffer, VkDeviceSize size);
		void CleanUp(Device* device);
	};

}

