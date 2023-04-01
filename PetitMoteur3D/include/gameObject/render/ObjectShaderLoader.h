#pragma once

#include "ObjectShaderInstance.h"
#include "ShaderBuffers.h"
#include "GeometryInitData.h"

namespace PM3D {
class ObjectShaderLoader {
protected:
	//gere la compilation du shader (+layout)
	ObjectShaderInstance* pShaderInstance = nullptr;
	ID3D11Device* pD3DDevice = nullptr;


public:
	ObjectShaderLoader() = default;
	ObjectShaderLoader(ID3D11Device* _pD3DDevice) noexcept : pD3DDevice(_pD3DDevice) {}
	virtual ~ObjectShaderLoader() = default;

	virtual void initBuffers(GeometryInitData::Type) = 0;
	void initShader(const std::wstring& filename);

	virtual void setMeshBuffers(ID3D11Buffer* pVertexBuffer, ID3D11Buffer* pIndexBuffer, ID3D11DeviceContext* pContext);
	virtual bool loadTexture(ID3D11ShaderResourceView* pTexture, const std::string& shaderVariableName = MeshShaderVariables.find(BASE_TEXTURE)->second);
	void Apply(ID3D11DeviceContext* pContext) { pShaderInstance->Apply(pContext); }

	void setDevice(ID3D11Device* _pDevice) noexcept {
		pD3DDevice = _pDevice;
	}

	void setSampleState(ID3D11SamplerState* pSampleState, const std::string& shaderVariableName = "SampleState", int index = 0);

	//template<class B>
	void createUninitializedConstantBuffer(ID3D11Device* pD3DDevice, uint32_t ByteWidth, ID3D11Buffer*& pbuffer, const int code);
};

}