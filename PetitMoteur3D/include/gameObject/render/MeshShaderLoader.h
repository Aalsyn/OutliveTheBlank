#pragma once

#include "ObjectShaderInstance.h"
#include "Light.h"
#include "MaterialData.h"
#include "ObjectShaderLoader.h"
#include "ShaderBuffers.h"

namespace PM3D {
	class CMaterialInstance;
	class MeshShaderLoader : public ObjectShaderLoader{
		//buffer constants
		ID3D11Buffer* pCameraBuffer = nullptr;
		ID3D11Buffer* pMaterialBuffer = nullptr;
		ID3D11Buffer* pLightBuffer = nullptr;

		ID3D11SamplerState* pSampleState = nullptr;


	public:
		MeshShaderLoader() = default;
		~MeshShaderLoader();
		MeshShaderLoader(ID3D11Device* _pD3DDevice) noexcept : ObjectShaderLoader(_pD3DDevice) {}

		void initBuffers(GeometryInitData::Type) override;
		void initSamplers();

		void setCameraBuffer(const DirectX::XMMATRIX& MatWorld, ID3D11DeviceContext* pContext, 
			const std::string& shaderVariableName = MeshShaderVariables.find(CAMERA_BUFFER)->second);
		void setLightBuffer(const DirectX::XMMATRIX& MatWorld, std::vector<Light>& lights, ID3D11DeviceContext* pContext,
			const std::string& shaderVariableName = MeshShaderVariables.find(LIGHT_BUFFER)->second);
		void setMaterialBuffer(const CMaterialData& material, bool isTextureActive, ID3D11DeviceContext* pContext, 
			const std::string& shaderVariableName = MeshShaderVariables.find(MATERIAL_BUFFER)->second);

		void setMaterialBuffer(const std::vector<CMaterialInstance>& materials, bool isTextureActive, ID3D11DeviceContext* pContext,
			const std::string& shaderVariableName = MeshShaderVariables.find(MATERIAL_TAB_BUFFER)->second);

		void setSamplers(const std::string& shaderVariableName = "SampleState");

	};

}