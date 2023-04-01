#pragma once
#include "Objet3D.h"
#include "Texture.h"

#include "d3dx11effect.h"
#ifndef TERRAIN_H
#define TERRAIN_H


#include "SommetTerrain.h"
namespace PM3D
{

	class CDispositifD3D11;

	class CTerrain : public CObjet3D
	{
	public:
		CTerrain(
			CSommetTerrain* pVertices,
			int* index_mesh,
			int vertexNb,
			int indicesSize,
			int dx,
			int dy,
			int dz,
			float echelleXZ,
			float echelleY,
			CDispositifD3D11* pDispositif);

		// Destructeur
		virtual ~CTerrain();

		virtual void Anime(float tempsEcoule) override;
		virtual void Draw() override;
		virtual void BasicRender(const XMMATRIX& matViewProj) override {};

		float getTerrainY(const float x, const float z) const;

		void setUscaling(float uScaling);
		void setVscaling(float vScaling);

		void SetTexture(CTexture* pTexture);
		void SetCliffTexture(CTexture* pTexture);
		void SetGrassTexture(CTexture* pTexture);
		void SetWaterTexture(CTexture* pTexture);

		void appendTextureToBlendSet(CTexture* pTexture);

	private:

		int* index_mesh;
		int indexSize;

		CDispositifD3D11* pDispositif;
		void InitEffet();


		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;

		// variables de textures

		ID3D11ShaderResourceView* pBaseTextureD3D;
		ID3D11ShaderResourceView* pCliffTextureD3D;
		ID3D11ShaderResourceView* pGrassTextureD3D;
		ID3D11ShaderResourceView* pWaterTextureD3D;


		ID3D11SamplerState* pSampleState;

		//specifique au terrain
		CSommetTerrain* pVertices;
		int dx = 0;
		int dy = 0;
		int dz = 0;

		const float echelleXZ = 1.0;
		const float echelleY = 0.05f;

		float uScaling = 1.0;
		float vScaling = 1.0;

		// pour les effets
		ID3DX11Effect* pEffet;
		ID3DX11EffectTechnique* pTechnique;
		ID3DX11EffectPass* pPasse;
		ID3D11InputLayout* pVertexLayout;

		// Définitions des valeurs d'animation
		ID3D11Buffer* pConstantBuffer;
		XMMATRIX matWorld;
		float rotation;
	};

	float computeDistance(const XMFLOAT3& v1, const XMFLOAT3& v2);

} // namespace PM3D
#endif