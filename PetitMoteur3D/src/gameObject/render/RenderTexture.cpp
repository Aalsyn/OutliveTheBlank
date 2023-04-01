#include "RenderTexture.h"
#include "util.h"
#include "Moteur.h"
#include "resource.h"

using namespace PM3D;

void RenderTexture::beginRender(CDispositifD3D11* pDispositif, ID3D11DeviceContext* pContext, ID3D11DepthStencilView* _pDepthStencilView)
{
	assert(pDispositif && pContext && pDepthStencilView);
	pContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);
	pContext->ClearRenderTargetView(pRenderTargetView, CLEARRED);
	pContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	pDispositif->SetViewPortDimension(width, height);
}

void RenderTexture::endRender(CDispositifD3D11* pDispositif, ID3D11DeviceContext* pContext)
{
	assert(pDispositif && pContext);
	/*ID3D11RenderTargetView* tabRTV[1];
	tabRTV[0] = pDispositif->GetRenderTargetView();*/
	ID3D11RenderTargetView* rtv = pDispositif->GetRenderTargetView();
	pContext->OMSetRenderTargets(1, &rtv, pDispositif->GetDepthStencilView());

	pDispositif->ResetViewPortDimension();

	//ID3D11ShaderResourceView* const tabSRV[1] = { NULL };
	//pContext->PSSetShaderResources(0, 1, tabSRV);
}

RenderTexture::~RenderTexture()
{
	DXRelacher(pRenderTargetTexture);
	DXRelacher(pRenderTargetView);
	DXRelacher(pShaderResourceView);
}

void RenderTexture::init(ID3D11Device* pDevice, int textureWidth, int textureHeight)
{
	width = static_cast<float>(textureWidth);
	height = static_cast<float>(textureHeight);
	assert(pDevice);
	//TODO : Add specific error codes to the string table for the DXEssayer calls
	
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	DXEssayer(pDevice->CreateTexture2D(&textureDesc, nullptr, &pRenderTargetTexture), DXE_ERREURCREATIONTEXTURE);
	


	
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	DXEssayer(pDevice->CreateRenderTargetView(pRenderTargetTexture, &renderTargetViewDesc, &pRenderTargetView));

	

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	DXEssayer(pDevice->CreateShaderResourceView(pRenderTargetTexture, &shaderResourceViewDesc, &pShaderResourceView));



	D3D11_TEXTURE2D_DESC depthTextureDesc;
	ZeroMemory(&depthTextureDesc, sizeof(depthTextureDesc));
	depthTextureDesc.Width = textureWidth;
	depthTextureDesc.Height = textureHeight;
	depthTextureDesc.MipLevels = 1;
	depthTextureDesc.ArraySize = 1;
	depthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthTextureDesc.SampleDesc.Count = 1;
	depthTextureDesc.SampleDesc.Quality = 0;
	depthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTextureDesc.CPUAccessFlags = 0;
	depthTextureDesc.MiscFlags = 0;

	DXEssayer(pDevice->CreateTexture2D(&depthTextureDesc, nullptr, &pDepthTexture),
		DXE_ERREURCREATIONTEXTURE);

	// Création de la vue du tampon de profondeur (ou de stencil)
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSView;
	ZeroMemory(&descDSView, sizeof(descDSView));
	descDSView.Format = depthTextureDesc.Format;
	descDSView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSView.Texture2D.MipSlice = 0;
	DXEssayer(pDevice->CreateDepthStencilView(pDepthTexture, &descDSView, &pDepthStencilView),
		DXE_ERREURCREATIONDEPTHSTENCILTARGET);
}
