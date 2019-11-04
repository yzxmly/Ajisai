#pragma once
#include "Core.h"
#include "Mesh.h"
#include "Texture.h"

namespace Ajisai {


	class Model
	{
	private:
		Device *mDevice;
		void ProcessNode(aiNode *node, const aiScene *scene, std::vector<Mesh> &meshes);
		void ProcessMesh(aiMesh *aimesh, const aiScene *scene, Mesh &mesh);
		void ProcessTexture(aiMaterial *mat, aiTextureType type, std::string typeName);

		void InitializeTexture(aiMaterial *mat, aiTextureType type, std::string typeName);

		void LoadTexture();

		void BindTexture(aiMaterial *mat, Mesh& mesh, aiTextureType textureType, std::string typeName);

	public:
		void BindDevice(Device & device) {
			mDevice = &device;
		}

		void LoadObject(std::string filePath);
		void MakeScreen();
		void MakeGround(float xWidth, float y, float zWidth);
		std::vector<VkDescriptorImageInfo> TextureInfo(size_t meshIdx);

		void CleanUp();

		std::vector<Mesh> mMeshes;

		std::vector<Texture> mTextures;
		std::vector<aiString> mTexturePath;
		std::vector<std::string> mTextureType;

		std::string mDirectory;
	};

}

