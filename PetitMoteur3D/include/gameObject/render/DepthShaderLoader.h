#pragma once

#include "ObjectShaderLoader.h"
#include "ShaderBuffers.h"
#include "MaterialData.h"
namespace PM3D {
	class DepthShaderLoader : public ObjectShaderLoader{
		//buffer constants
		ID3D11Buffer* pPovBuffer = nullptr;
		ID3D11Buffer* pMaterialBuffer = nullptr;

	public:
		DepthShaderLoader() = default;
		~DepthShaderLoader();
		DepthShaderLoader(ID3D11Device* _pD3DDevice) noexcept : ObjectShaderLoader(_pD3DDevice) {}

		void initBuffers(GeometryInitData::Type) override;


		void setPovBuffer(const DirectX::XMMATRIX& MatWorld, const XMMATRIX& matViewProj, ID3D11DeviceContext* pContext,
			const std::string& shaderVariableName = DepthShaderVariables.find(POV_BUFFER)->second);

		void setMaterialBuffer(const CMaterialData& material, bool isTextureActive, ID3D11DeviceContext* pContext,
			const std::string& shaderVariableName = MeshShaderVariables.find(MATERIAL_BUFFER)->second);
	};

}