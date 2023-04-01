#include "StdAfx.h"
#include "Terrain.h"
#include "util.h"
#include "DispositifD3D11.h"
#include "resource.h"
#include "MoteurWindows.h"
#include "GestionnaireDeTextures.h"
#include "PxPhysicsAPI.h"
#include "PxPhysics.h"
#include <vector>
#include "ObjectPhysicalInitData.h"
#include "ObjectPhysicalInstance.h"

using namespace physx;

namespace PM3D
{

	struct ShadersParams
	{
		XMMATRIX matWorldViewProj;	// la matrice totale 
		XMMATRIX matWorld;			// matrice de transformation dans le monde 
		XMVECTOR vLumiere; 			// la position de la source d'éclairage (Point)
		XMVECTOR vCamera; 			// la position de la caméra
		XMVECTOR vAEcl; 			// la valeur ambiante de l'éclairage
		XMVECTOR vAMat; 			// la valeur ambiante du matériau
		XMVECTOR vDEcl; 			// la valeur diffuse de l'éclairage 
		XMVECTOR vDMat; 			// la valeur diffuse du matériau 
		XMVECTOR uvScaling;
	};

	CTerrain::CTerrain(
		CSommetTerrain* pVertices ,
		int* index_mesh,
		int vertexNb,
		int indicesSize,
		int dx_,
		int dy_,
		int dz_,
		float echelleXZ_,
		float echelleY_,
		CDispositifD3D11* pDispositif_)
		: pDispositif(pDispositif_) // Prendre en note le dispositif
		, matWorld(XMMatrixIdentity())
		, rotation(0.0f)
		, pVertices(pVertices)
		, pVertexBuffer(nullptr)
		, pIndexBuffer(nullptr)
		, pVertexLayout(nullptr)
		, pConstantBuffer(nullptr)
		, pEffet(nullptr)
		, pTechnique(nullptr)
		, pBaseTextureD3D(0)
		, pCliffTextureD3D(0)
		, pGrassTextureD3D(0)
		, pWaterTextureD3D(0)
		, pSampleState(0)
		, index_mesh(index_mesh)
		, indexSize(indicesSize)
		, dx(dx_), dy(dy_), dz(dz_)
		, echelleXZ(echelleXZ_), echelleY(echelleY_)
	{

		// Création du vertex buffer et copie des sommets
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(CSommetTerrain) * vertexNb;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = pVertices;

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);

		// Création de l'index buffer et copie des indices
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(int)* indicesSize;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = index_mesh;
		pIndexBuffer = nullptr;

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer),
			DXE_CREATIONINDEXBUFFER);
		// initialisation de l'effet
		InitEffet();

		// Setting base UVScaling (can be redefined at runtime)
		const float M = static_cast<float>((dx > dz) ? dx : dz);
		setUscaling(M/2);
		setVscaling(M/2);

		//TexturesManager
		SetTexture(
			CMoteurWindows::GetInstance().
			GetTextureManager().
			GetNewTexture(L"sand.dds", pDispositif)
		);

		SetCliffTexture(
			CMoteurWindows::GetInstance().
			GetTextureManager().
			GetNewTexture(L"cliff.dds", pDispositif)
		);

		SetGrassTexture(
			CMoteurWindows::GetInstance().
			GetTextureManager().
			GetNewTexture(L"grass.dds", pDispositif)
		);

		SetWaterTexture(
			CMoteurWindows::GetInstance().
			GetTextureManager().
			GetNewTexture(L"water.dds", pDispositif)
		);

		
	}

	void CTerrain::Anime(float){}

	void CTerrain::Draw()
	{

		// Obtenir le contexte
		ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();

		// Choisir la topologie des primitives
		pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Source des sommets
		const UINT stride = sizeof(CSommetTerrain);
		const UINT offset = 0;
		pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
		// Source des index
		pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// input layout des sommets
		pImmediateContext->IASetInputLayout(pVertexLayout);

		// Activer le VS
		//pImmediateContext->VSSetShader(pVertexShader, nullptr, 0);

		// Initialiser et sélectionner les «constantes» de l'effet
		ShadersParams sp;
		XMMATRIX viewProj = CMoteurWindows::GetInstance().GetMatViewProj();

		sp.matWorldViewProj = XMMatrixTranspose(matWorld * viewProj);
		sp.matWorld = XMMatrixTranspose(matWorld);

		sp.vCamera = CMoteurWindows::GetInstance().GetCamera()->getPosition();
		sp.vLumiere =  XMVectorSet(0.0f, 300.0f, 0.0f, 0.0f);
		sp.vAEcl = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
		sp.vAMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.vDEcl = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.vDMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.uvScaling = { uScaling, vScaling };


		pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);

		ID3DX11EffectConstantBuffer* pCB = pEffet->GetConstantBufferByName("param");  // Nous n'avons qu'un seul CBuffer
		pCB->SetConstantBuffer(pConstantBuffer);


		// Activation de la texture
		ID3DX11EffectShaderResourceVariable* variableTexture;
		variableTexture = pEffet->GetVariableByName("texture_Base") -> AsShaderResource();
		variableTexture->SetResource(pBaseTextureD3D);

		variableTexture = pEffet->GetVariableByName("texture_Cliff")->AsShaderResource();
		variableTexture->SetResource(pCliffTextureD3D);

		variableTexture = pEffet->GetVariableByName("texture_Grass")->AsShaderResource();
		variableTexture->SetResource(pGrassTextureD3D);

		variableTexture = pEffet->GetVariableByName("texture_Water")->AsShaderResource();
		variableTexture->SetResource(pWaterTextureD3D);
	
		// Le sampler state (texture)
		ID3DX11EffectSamplerVariable* variableSampler;
		variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
		variableSampler->SetSampler(0, pSampleState);

		// **** Rendu de l'objet
		pPasse->Apply(0, pImmediateContext);

		pImmediateContext->DrawIndexed(indexSize, 0, 0);
	}

	CTerrain::~CTerrain()
	{
		DXRelacher(pVertexBuffer);
		DXRelacher(pIndexBuffer);
		DXRelacher(pConstantBuffer);
		DXRelacher(pEffet);
		DXRelacher(pVertexLayout);
	}

	void CTerrain::InitEffet()
	{
		// Compilation et chargement du vertex shader
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

		// Création d'un tampon pour les constantes du VS
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ShadersParams);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		pD3DDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer);

		// Pour l'effet
		ID3DBlob* pFXBlob = nullptr;

		DXEssayer(D3DCompileFromFile(L"MiniPhong.fx", 0, 0, 0,
			"fx_5_0", 0, 0,
			&pFXBlob, nullptr),
			DXE_ERREURCREATION_FX);

		D3DX11CreateEffectFromMemory(pFXBlob->GetBufferPointer(), pFXBlob->GetBufferSize(), 0, pD3DDevice, &pEffet);

		pFXBlob->Release();

		pTechnique = pEffet->GetTechniqueByIndex(0);
		pPasse = pTechnique->GetPassByIndex(0);

		// Créer l'organisation des sommets pour le VS de notre effet
		D3DX11_PASS_SHADER_DESC effectVSDesc;
		pPasse->GetVertexShaderDesc(&effectVSDesc);

		D3DX11_EFFECT_SHADER_DESC effectVSDesc2;
		effectVSDesc.pShaderVariable->GetShaderDesc(effectVSDesc.ShaderIndex, &effectVSDesc2);

		const void* vsCodePtr = effectVSDesc2.pBytecode;
		const unsigned vsCodeLen = effectVSDesc2.BytecodeLength;

		pVertexLayout = nullptr;
		DXEssayer(pD3DDevice->CreateInputLayout(CSommetTerrain::layout,
			CSommetTerrain::numElements,
			vsCodePtr,
			vsCodeLen,
			&pVertexLayout),
			DXE_CREATIONLAYOUT);

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

	float computeDistance(const XMFLOAT3& v1, const XMFLOAT3& v2)
	{
		XMVECTOR vector1 = XMLoadFloat3(&v1);
		XMVECTOR vector2 = XMLoadFloat3(&v2);

		XMVECTOR vectorSub = XMVectorSubtract(vector1, vector2);
		XMVECTOR length = XMVector3Length(vectorSub);

		float distance = 0.0f;
		XMStoreFloat(&distance, length);
		return distance;
	}

	float CTerrain::getTerrainY(const float x, const float z) const
	{
		auto vShift = [](double val) {return (0.0 < val) - (val < 0.0); };
		double xScaled = x / echelleXZ + (dx / 2.0);
		double zScaled = z / echelleXZ + (dz / 2.0);
		int xIndex = static_cast<int>(round(xScaled));
		int zIndex = static_cast<int>(round(zScaled));
		

		if (xIndex > 0 && xIndex < dx && zIndex > 0 && zIndex < dz) {

			const XMFLOAT3& v1 = pVertices[xIndex + vShift(xScaled - xIndex) + zIndex * dx].getPosition();
			const XMFLOAT3& v2 = pVertices[xIndex + ((zIndex + vShift(zScaled - zIndex)) * dx)].getPosition();
			const XMFLOAT3& v3 = pVertices[xIndex + zIndex * dx].getPosition();

			// compute Barycentric weights
			const float u =
				((v2.z - v3.z) * (x - v3.x) + (v3.x - v2.x) * (z - v3.z))
				/
				((v2.z - v3.z) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.z - v3.z))
				;
			const float v =
				((v3.z - v1.z) * (x - v3.x) + (v1.x - v3.x) * (z - v3.z))
				/
				((v2.z - v3.z) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.z - v3.z))
				;

			float y = (1 - u - v) * v3.y + u * v1.y + v * v2.y;
			return y;
		}
		return -1000;
	}

	void CTerrain::setUscaling(float _uScaling) {
		uScaling = _uScaling;
	}

	void CTerrain::setVscaling(float _vScaling) {
		vScaling = _vScaling;
	}

	void CTerrain::SetTexture(CTexture* pTexture)
	{
		pBaseTextureD3D = pTexture->GetD3DTexture();
	}

	void CTerrain::SetCliffTexture(CTexture* pTexture)
	{
		pCliffTextureD3D = pTexture->GetD3DTexture();
	}

	void CTerrain::SetGrassTexture(CTexture* pTexture)
	{
		pGrassTextureD3D = pTexture->GetD3DTexture();
	}

	void CTerrain::SetWaterTexture(CTexture* pTexture)
	{
		pWaterTextureD3D = pTexture->GetD3DTexture();
	}

} // namespace PM3D
