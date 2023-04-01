#pragma once
#include "ObjectShaderInstance.h"

namespace PM3D {

class ShaderInstanceManager {
	std::vector<std::unique_ptr<ObjectShaderInstance>> shaderInstanceList;
public:
	//	CMeshInstance* const GetNewInstance(const wstring& filename, CDispositifD3D11* pDispositif);
	ObjectShaderInstance* const GetNewInstance(const std::wstring& filename, ID3D11Device* pD3DDevice);
	ObjectShaderInstance* const GetInstance(const std::wstring& filename);
};

}
