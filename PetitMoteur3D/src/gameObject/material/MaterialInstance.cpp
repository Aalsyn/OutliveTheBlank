#include "MaterialInstance.h"
#include "MoteurWindows.h"

void PM3D::CMaterialInstance::setBuffers(ID3D11Buffer* pVertexBuffer, ID3D11Buffer* pIndexBuffer, const XMMATRIX& matrixWorld, ID3D11DeviceContext* pContext)
{
	shaderLoader.setMeshBuffers(pVertexBuffer, pIndexBuffer, pContext);
	bool isTextureLoaded = shaderLoader.loadTexture(pBaseTexture);
	//if (!isTextureLoaded) {
	//	bool test = false;
	//}
	//load shadow map from the RenderTexture resource (depth buffer rendered on this texture)
	pShadowMapTexture = CMoteurWindows::GetInstance().GetRenderTexture().GetShaderResourceView();
	//bool isTextureLoaded = shaderLoader.loadTexture(pShadowMapTexture, MeshShaderVariables.find(BASE_TEXTURE)->second);
	shaderLoader.loadTexture(pShadowMapTexture, MeshShaderVariables.find(SHADOW_MAP_TEXTURE)->second);

	shaderLoader.setLightBuffer(matrixWorld, CMoteurWindows::GetInstance().getLights(), pContext);
	shaderLoader.setCameraBuffer(matrixWorld, pContext);
	shaderLoader.setMaterialBuffer(matData, isTextureLoaded, pContext);
	shaderLoader.setSamplers();
	shaderLoader.Apply(pContext);


}

void PM3D::CMaterialInstance::setBuffers(ID3D11Buffer* pVertexBuffer, ID3D11Buffer* pIndexBuffer, const XMMATRIX& matrixWorld, std::vector<CMaterialInstance>& matVec, ID3D11DeviceContext* pContext)
{
	shaderLoader.setMeshBuffers(pVertexBuffer, pIndexBuffer, pContext);
	shaderLoader.setLightBuffer(matrixWorld, CMoteurWindows::GetInstance().getLights(), pContext);
	shaderLoader.setCameraBuffer(matrixWorld, pContext);

	//THERE MUST BE 3 MATERIALS
	//The material number 0 is usually empty ?
	shaderLoader.loadTexture(matVec[1].pBaseTexture, "tex1");
	shaderLoader.loadTexture(matVec[2].pBaseTexture, "tex2");
	shaderLoader.loadTexture(matVec[3].pBaseTexture, "tex3");

	shaderLoader.setMaterialBuffer(matVec, true, pContext);
	shaderLoader.setSamplers();
	shaderLoader.Apply(pContext);
}
