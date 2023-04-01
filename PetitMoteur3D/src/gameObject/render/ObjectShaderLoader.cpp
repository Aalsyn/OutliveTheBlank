#include "stdafx.h"
#include "ObjectShaderLoader.h"
#include "ShaderInstanceManager.h"
#include "MoteurWindows.h"
#include "ShaderBuffers.h"
#include "resource.h"
#include "util.h"

//template<class B>
void PM3D::ObjectShaderLoader::createUninitializedConstantBuffer(ID3D11Device* pD3DDevice, uint32_t ByteWidth, ID3D11Buffer*& pbuffer, const int code)
{
	assert(pD3DDevice);
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = ByteWidth; //the byte width must be a multiple of 16 for constant buffers
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	DXEssayer(pD3DDevice->CreateBuffer(&bd, nullptr, &pbuffer), code);
}


void PM3D::ObjectShaderLoader::initShader(const std::wstring& filename)
{
	assert(pD3DDevice);
	ShaderInstanceManager& shaderManager = CMoteurWindows::GetInstance().GetShaderInstanceManager();
	pShaderInstance = shaderManager.GetNewInstance(filename, pD3DDevice);
}


void PM3D::ObjectShaderLoader::setMeshBuffers(ID3D11Buffer* pVertexBuffer, ID3D11Buffer* pIndexBuffer, ID3D11DeviceContext* pContext)
{
	assert(pVertexBuffer);
	assert(pIndexBuffer);
	assert(pContext);

	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pContext->IASetInputLayout(pShaderInstance->pVertexLayout);
	pContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	UINT stride = sizeof(CSommetTerrain);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
}



void PM3D::ObjectShaderLoader::setSampleState(ID3D11SamplerState* pSampleState, const std::string& shaderVariableName, int index)
{
	ID3DX11EffectSamplerVariable* variableSampler;
	variableSampler = pShaderInstance->pEffet->GetVariableByName(shaderVariableName.c_str())->AsSampler();
	variableSampler->SetSampler(0, pSampleState);
}


bool PM3D::ObjectShaderLoader::loadTexture(ID3D11ShaderResourceView* pTexture, const std::string& shaderVariableName)
{
	ID3DX11EffectShaderResourceVariable* variableTexture;
	variableTexture = pShaderInstance->pEffet->GetVariableByName(shaderVariableName.c_str())->AsShaderResource();
	variableTexture->SetResource(pTexture);
	return (pTexture != nullptr);
}


