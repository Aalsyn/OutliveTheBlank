#include "ShaderInstanceManager.h"

using namespace PM3D;
ObjectShaderInstance* const ShaderInstanceManager::GetNewInstance(const std::wstring& filename, ID3D11Device* pD3DDevice)
{
	// On vérifie si la texture est déjà dans notre liste
	ObjectShaderInstance* pShader = GetInstance(filename);
	// Si non, on la crée
	if (!pShader)
	{
		auto shader = std::make_unique<ObjectShaderInstance>(filename, pD3DDevice);
		pShader = shader.get();
		// Puis, il est ajouté à la scène
		shaderInstanceList.push_back(std::move(shader));
	}
	assert(pShader);
	return pShader;
}

ObjectShaderInstance* const PM3D::ShaderInstanceManager::GetInstance(const std::wstring& filename)
{
	ObjectShaderInstance* pShader = nullptr;
	for (auto& shader : shaderInstanceList)
	{
		if (shader->getFileName() == filename)
		{
			pShader = shader.get();
			break;
		}
	}
	return pShader;
}


