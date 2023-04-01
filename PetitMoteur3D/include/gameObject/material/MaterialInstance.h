#pragma once
#include <stdafx.h>
#include <string>
#include "MaterialData.h"
#include "MeshShaderLoader.h"
#include "GeometryInitData.h"
#include <vector>
namespace PM3D {
class CMaterialInstance
{
	MeshShaderLoader shaderLoader;

	//std::vector<ID3D11ShaderResourceView*> pShadowMapTextures{LIGHT_NUMBER};
	ID3D11ShaderResourceView* pShadowMapTexture;
public:
	ID3D11ShaderResourceView* pNormalMapTexture = nullptr;
	ID3D11ShaderResourceView* pBaseTexture = nullptr;
	CMaterialData matData; //cannot be private for now because of the loading of binary files (see LireFichierBinaire)

	CMaterialInstance() = default;
	CMaterialInstance(ID3D11Device* _pD3DDevice) : shaderLoader{ _pD3DDevice } {}
	CMaterialInstance(ID3D11Device* _pD3DDevice, const CMaterialData& _matData) : shaderLoader{ _pD3DDevice }, matData{ _matData } {}
	CMaterialInstance(ID3D11Device* _pD3DDevice, CMaterialData&& _matData) : shaderLoader{ _pD3DDevice }, matData{ _matData } {}

	void initShaderLoader(const std::wstring& filename, GeometryInitData::Type type) {
		shaderLoader.initShader(filename);
		shaderLoader.initBuffers(type);
		shaderLoader.initSamplers();
	}

	void setBuffers(ID3D11Buffer* pVertexBuffer, ID3D11Buffer* pIndexBuffer, const XMMATRIX& matrixWorld, ID3D11DeviceContext* pContext);
	void setBuffers(ID3D11Buffer* pVertexBuffer, ID3D11Buffer* pIndexBuffer, const XMMATRIX& matrixWorld, std::vector<CMaterialInstance>& matVec, ID3D11DeviceContext* pContext);


	void setDevice(ID3D11Device* pDevice) {
		shaderLoader.setDevice(pDevice);
	}

	void Apply(ID3D11DeviceContext* pContext) { shaderLoader.Apply(pContext); }
};
}

