#pragma once
#include "Core.h"
#include "Mesh.h"
#include "Texture.h"

namespace Ajisai {


	class Object
	{
	private:
		Device *mDevice;
		void ProcessNode(aiNode *node, const aiScene *scene, std::vector<Mesh> &meshes);
		void ProcessMesh(aiMesh *aimesh, const aiScene *scene, Mesh &mesh);
		void ProcessTexture(aiMaterial *mat, aiTextureType type, std::string typeName);

		void InitializeTexture(aiMaterial *mat, aiTextureType type, std::string typeName);

		void LoadTexture();
		
	public:
		void BindDevice(Device & device) {
			mDevice = &device;
		}

		void LoadObject(std::string filePath);
		VkDescriptorImageInfo TextureInfo(unsigned int meshIdx);

		void CleanUp();

		std::vector<Mesh> mMeshes;
		
		std::vector<Texture> mTextures;
		std::vector<VkSampler> mTexSamplers;
		std::vector<aiString> mTexturePath;
		std::vector<std::string> mTextureType;

		std::string mDirectory;
	};

}

