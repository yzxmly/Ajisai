#pragma once
#include "Core.h"
#include "Device.h"
namespace Ajisai {
	class CommandPool
	{
	public:
		Device* mDevice;
		VkCommandPool mCmdPool;

		void CreateCmdPool();

		VkCommandBuffer BeginSingleTimeCmds();
		void EndSingleTimeCmds(VkCommandBuffer cmdBuffer);
	};
}


