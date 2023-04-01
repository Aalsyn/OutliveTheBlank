#include "ObjectShaderInstance.h"
#include "util.h"
#include "resource.h"
#include "SommetTerrain.h"

using namespace PM3D;

ObjectShaderInstance::ObjectShaderInstance(const std::wstring& _filename, ID3D11Device* _pD3DDevice) : filename(_filename), pD3DDevice(_pD3DDevice)
{
	compileShader();
	createVertexLayout();
}

PM3D::ObjectShaderInstance::~ObjectShaderInstance()
{
	DXRelacher(pEffet);
	DXRelacher(pTechnique);
	DXRelacher(pPasse);
	DXRelacher(pVertexLayout);
}

void PM3D::ObjectShaderInstance::compileShader()
{
	// Pour l'effet
	ID3DBlob* pFXBlob = nullptr; // need to instanciated

	DXEssayer(D3DCompileFromFile(filename.c_str(), 0, 0, 0, // need to instanciated
		"fx_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
		&pFXBlob, nullptr),
		DXE_ERREURCREATION_FX);

	D3DX11CreateEffectFromMemory(pFXBlob->GetBufferPointer(), pFXBlob->GetBufferSize(), 0, pD3DDevice, &pEffet); // need to instanciated

	pFXBlob->Release();

	pTechnique = pEffet->GetTechniqueByIndex(0); // need to instanciated
	pPasse = pTechnique->GetPassByIndex(0); // need to instanciated
}


void ObjectShaderInstance::createVertexLayout()
{
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
}
