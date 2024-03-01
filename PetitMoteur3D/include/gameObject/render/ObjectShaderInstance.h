#pragma once
#include "dispositifD3D11.h"


namespace PM3D {
class ObjectShaderInstance {

	std::wstring filename;

	ID3D11Device* pD3DDevice = nullptr;

	void compileShader();
	void createVertexLayout();
public:
	ID3DX11Effect* pEffet = nullptr;
	ID3DX11EffectTechnique* pTechnique = nullptr;
	ID3DX11EffectPass* pPasse = nullptr; //if multiple pass is used, this should be an array
	ID3D11InputLayout* pVertexLayout = nullptr;

	ObjectShaderInstance() = delete;
	ObjectShaderInstance(const std::wstring& _filename, ID3D11Device* _pD3DDevice);
	~ObjectShaderInstance();

	void Apply(ID3D11DeviceContext* pContext) { pPasse->Apply(0, pContext); }
	const std::wstring& getFileName() const noexcept { return filename; }
	
};
}