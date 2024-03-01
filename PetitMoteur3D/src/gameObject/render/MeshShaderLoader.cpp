#include <stdafx.h>
#include "MeshShaderLoader.h"
#include "ShaderInstanceManager.h"
#include "MoteurWindows.h"
#include "ShaderBuffers.h"
#include "resource.h"
#include "util.h"



void PM3D::MeshShaderLoader::initBuffers(GeometryInitData::Type type)
{
	createUninitializedConstantBuffer(pD3DDevice, sizeof(CameraParams), pCameraBuffer, DXE_CREATIONCONSTANTBUFFER);
	createUninitializedConstantBuffer(pD3DDevice, sizeof(LightParams), pLightBuffer, DXE_CREATIONCONSTANTBUFFER);
	uint32_t matParamsSize = sizeof(MaterialParams);
	switch (type) {
	case GeometryInitData::MESH: {
		matParamsSize = sizeof(MaterialParams);
		break;
	}
	case GeometryInitData::BILLBOARD: {
		matParamsSize = sizeof(MaterialParams);
		break;
	}
	case GeometryInitData::TERRAIN: {
		matParamsSize = sizeof(MaterialTabParams);
		break;
	}
	}
	createUninitializedConstantBuffer(pD3DDevice, matParamsSize, pMaterialBuffer, DXE_CREATIONCONSTANTBUFFER);
}

void PM3D::MeshShaderLoader::initSamplers()
{
	// Initialisation des paramètres de sampling de la texture
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 8;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	// Création de l’état de sampling
	pD3DDevice->CreateSamplerState(&samplerDesc, &pSampleState);
}



PM3D::MeshShaderLoader::~MeshShaderLoader()
{
	DXRelacher(pCameraBuffer);
	DXRelacher(pMaterialBuffer);
	DXRelacher(pLightBuffer);
}


void PM3D::MeshShaderLoader::setCameraBuffer(const DirectX::XMMATRIX& MatWorld, ID3D11DeviceContext* pContext, const std::string& shaderVariableName)
{
	assert(pCameraBuffer);
	assert(pContext);
	const XMMATRIX& viewProj = CMoteurWindows::GetInstance().GetMatViewProj();
	CameraParams cp;
	cp.cPosition = XMVectorSet(0.0f, 3.0f, -5.0f, 1.0f); //initial camera position
	cp.matWorldViewProj = XMMatrixTranspose(MatWorld * viewProj);
	cp.matWorld = XMMatrixTranspose(MatWorld);

	ID3DX11EffectConstantBuffer* pCameraParams = pShaderInstance->pEffet->GetConstantBufferByName(shaderVariableName.c_str());
	pCameraParams->SetConstantBuffer(pCameraBuffer);
	pContext->UpdateSubresource(pCameraBuffer, 0, nullptr, &cp, 0, 0);
}

void PM3D::MeshShaderLoader::setLightBuffer(const DirectX::XMMATRIX& MatWorld, std::vector<Light>& lights, ID3D11DeviceContext* pContext, const std::string& shaderVariableName)
{
	assert(pMaterialBuffer);
	assert(pContext);
	LightParams lp;
	for (size_t i = 0; i != std::min(lights.size(), LIGHT_NUMBER); ++i) {
		//TODO : retrieve the mesh position and set the viewMatrix of the light with it

		lp.lPosition[i] = lights[i].position;
		lp.lDirection[i] = lights[i].direction;
		lp.lAmbiant[i] = lights[i].ambiant;
		lp.lDiffuse[i] = lights[i].diffuse;
		lp.lSpecular[i] = lights[i].specular;
		lp.lAttenuationParam[i] = lights[i].attenuationParam;
		lp.lMatWVP[i] = XMMatrixTranspose(MatWorld * lights[i].getViewProjMatrix());
		lp.isPoint[i] = lights[i].lightType == LightType::POINTLIGHT ? XMVectorSet(1.0f ,1.0f, 1.0f, 1.0f) : XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		lp.lIsActive[i] = lights[i].isActive ? XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f) : XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	}


	ID3DX11EffectConstantBuffer* pLightParams = pShaderInstance->pEffet->GetConstantBufferByName(shaderVariableName.c_str());
	pLightParams->SetConstantBuffer(pLightBuffer);
	pContext->UpdateSubresource(pLightBuffer, 0, nullptr, &lp, 0, 0);
}

void PM3D::MeshShaderLoader::setMaterialBuffer(const CMaterialData& material, bool isTextureActive, ID3D11DeviceContext* pContext, const std::string& shaderVariableName)
{
	assert(pMaterialBuffer);
	assert(pContext);
	MaterialParams mp;
	mp.mAmbiant = XMLoadFloat4(&material.Ambient);
	mp.mDiffuse = XMLoadFloat4(&material.Diffuse);
	mp.mSpecular = XMLoadFloat4(&material.Specular);
	mp.specAttenuation = XMVectorSet(material.Puissance, material.Puissance, material.Puissance, 1.0f);
	mp.uvScaling = XMVectorSet(material.uScaling, material.vScaling, 0.0f, 0.0f);
	// Activation de la texture ou non
	mp.texIsActive = isTextureActive ? XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f) : XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);


	ID3DX11EffectConstantBuffer* pMaterialParams = pShaderInstance->pEffet->GetConstantBufferByName(shaderVariableName.c_str());
	pMaterialParams->SetConstantBuffer(pMaterialBuffer);
	pContext->UpdateSubresource(pMaterialBuffer, 0, nullptr, &mp, 0, 0);
}

void PM3D::MeshShaderLoader::setMaterialBuffer(const std::vector<CMaterialInstance>& materials, bool isTextureActive, ID3D11DeviceContext* pContext, const std::string& shaderVariableName)
{
	assert(pMaterialBuffer);
	assert(pContext);
	MaterialTabParams mp;
	for (size_t i = 0; i != std::min(materials.size(), MAT_NUMBER); ++i) {
		mp.mAmbiant[i] = XMLoadFloat4(&materials[i].matData.Ambient);
		mp.mDiffuse[i] = XMLoadFloat4(&materials[i].matData.Diffuse);
		mp.mSpecular[i] = XMLoadFloat4(&materials[i].matData.Specular);
		mp.specAttenuation[i] = XMVectorSet(materials[i].matData.Puissance, materials[i].matData.Puissance, materials[i].matData.Puissance, 1.0f);
		mp.uvScaling[i] = XMVectorSet(materials[i].matData.uScaling, materials[i].matData.vScaling, 0.0f, 0.0f);
		// Activation de la texture ou non
		mp.texIsActive[i] = isTextureActive ? XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f) : XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	}
	ID3DX11EffectConstantBuffer* pMaterialParams = pShaderInstance->pEffet->GetConstantBufferByName(shaderVariableName.c_str());
	pMaterialParams->SetConstantBuffer(pMaterialBuffer);
	pContext->UpdateSubresource(pMaterialBuffer, 0, nullptr, &mp, 0, 0);
}

void PM3D::MeshShaderLoader::setSamplers(const std::string& shaderVariableName)
{
	setSampleState(pSampleState, shaderVariableName, 0);
}
