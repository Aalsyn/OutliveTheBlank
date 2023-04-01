#pragma once
#include "stdafx.h"
#include "dispositifD3D11.h"

class RenderTexture {
	float height = 512;
	float width = 512;
	ID3D11Texture2D* pRenderTargetTexture = nullptr;
	ID3D11Texture2D* pDepthTexture = nullptr;
	ID3D11RenderTargetView* pRenderTargetView = nullptr;
	ID3D11ShaderResourceView* pShaderResourceView = nullptr;
	ID3D11DepthStencilView* pDepthStencilView = nullptr;

public:
	RenderTexture() = default;
	RenderTexture(const RenderTexture&) = default;
	RenderTexture(float _height, float _width) noexcept : height(_height), width(_width){}
	~RenderTexture();

	void init(ID3D11Device* pDevice, int textureWidth, int textureHeight);
	void beginRender(PM3D::CDispositifD3D11* pDispositif, ID3D11DeviceContext* pContext, ID3D11DepthStencilView* _pDepthStencilView);
	void endRender(PM3D::CDispositifD3D11* pDispositif, ID3D11DeviceContext* pContext);

	ID3D11ShaderResourceView* GetShaderResourceView() noexcept { return pShaderResourceView; }
	void setHeight(float _height)  noexcept { height = _height; }
	void setWidth(float _width)  noexcept { height = _width; }
};