#pragma once
#include <string>
#include "d3dx11effect.h"
#include "Objet3D.h"
#include "DispositifD3D11.h"
#include "Texture.h"
namespace PM3D
{
	class CSommetSprite
	{
	public:
		CSommetSprite() = default;
		CSommetSprite(const XMFLOAT3& position, const XMFLOAT2& coordTex)
			: m_Position(position)
			, m_CoordTex(coordTex)
		{
		}
	public:
		static UINT numElements;
		static D3D11_INPUT_ELEMENT_DESC layout[];
		XMFLOAT3 m_Position;
		XMFLOAT2 m_CoordTex;
	};


	class CSpriteTemp : public CObjet3D
	{
	private:
		static CSommetSprite sommets[6];
		ID3D11Buffer* pVertexBuffer;
		CDispositifD3D11* pDispositif;
		
		ID3D11Buffer* pConstantBuffer;
		ID3DX11Effect* pEffet;
		ID3DX11EffectTechnique* pTechnique;
		ID3DX11EffectPass* pPasse;
		ID3D11InputLayout* pVertexLayout;
		ID3D11ShaderResourceView* pTextureD3D;
		ID3D11SamplerState* pSampleState;

		virtual void InitEffet();
	public:
		CSpriteTemp(const std::string& NomTexture, CDispositifD3D11*
			pDispositif);
		virtual ~CSpriteTemp();
		virtual void Anime(float tempsEcoule) {};
		virtual void Draw();

	};
} // namespace PM3D