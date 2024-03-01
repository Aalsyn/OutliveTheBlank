#include <stdafx.h>
#include "DepthShaderLoader.h"
#include "ShaderInstanceManager.h"
#include "MoteurWindows.h"
#include "ShaderBuffers.h"
#include "resource.h"
#include "util.h"



void PM3D::DepthShaderLoader::initBuffers(GeometryInitData::Type)
{
	createUninitializedConstantBuffer(pD3DDevice, sizeof(PovParams), pPovBuffer, DXE_CREATIONCONSTANTBUFFER);
	createUninitializedConstantBuffer(pD3DDevice, sizeof(MaterialParams), pMaterialBuffer, DXE_CREATIONCONSTANTBUFFER);
}



PM3D::DepthShaderLoader::~DepthShaderLoader()
{
	DXRelacher(pPovBuffer);
}


void PM3D::DepthShaderLoader::setPovBuffer(const DirectX::XMMATRIX& MatWorld, const XMMATRIX& matViewProj, ID3D11DeviceContext* pContext,
	const std::string& shaderVariableName)
{
	assert(pPovBuffer && pContext);
	PovParams pp;
	//auto& light = CMoteurWindows::GetInstance().getLights().front();
	//const XMMATRIX& matViewProj = light.getViewProjMatrix();
	//const XMMATRIX& matViewProj = CMoteurWindows::GetInstance().GetMatViewProj();
	pp.matWorldViewProj = XMMatrixTranspose(MatWorld * matViewProj);

	ID3DX11EffectConstantBuffer* pPovParams = pShaderInstance->pEffet->GetConstantBufferByName(shaderVariableName.c_str());
	pPovParams->SetConstantBuffer(pPovBuffer);
	pContext->UpdateSubresource(pPovBuffer, 0, nullptr, &pp, 0, 0);
}

void PM3D::DepthShaderLoader::setMaterialBuffer(const CMaterialData& material, bool isTextureActive, ID3D11DeviceContext* pContext, const std::string& shaderVariableName)
{
	assert(pMaterialBuffer);
	assert(pContext);
	MaterialParams mp;
	mp.mAmbiant = XMLoadFloat4(&material.Ambient);
	mp.mDiffuse = XMLoadFloat4(&material.Diffuse);
	mp.mSpecular = XMLoadFloat4(&material.Specular);
	mp.specAttenuation = XMVectorSet(material.Puissance, material.Puissance, material.Puissance, 1.0f);

	// Activation de la texture ou non
	mp.texIsActive = isTextureActive ? XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f) : XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	ID3DX11EffectConstantBuffer* pMaterialParams = pShaderInstance->pEffet->GetConstantBufferByName(shaderVariableName.c_str());
	pMaterialParams->SetConstantBuffer(pMaterialBuffer);
	pContext->UpdateSubresource(pMaterialBuffer, 0, nullptr, &mp, 0, 0);
}
