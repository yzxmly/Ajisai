#include "CommandPool.h"

namespace Ajisai {
	VkCommandBuffer CommandPool::BeginSingleTimeCmds() {
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = mCmdPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer cmdBuffer;
		vkAllocateCommandBuffers(mDevice->mLogicalDevice, &allocInfo, &cmdBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(cmdBuffer, &beginInfo);

		return cmdBuffer;
	}

	void CommandPool::EndSingleTimeCmds(VkCommandBuffer cmdBuffer) {

	}
}