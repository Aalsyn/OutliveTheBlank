#pragma once

#ifndef MESH_H
#define MESH_H


#include "Objet3D.h"
#include "Texture.h"
#include "MaterialInstance.h"
#include "SommetTerrain.h"
#include "chargeur.h"
#include "GeometryInitData.h"
#include "ObjectShaderInstance.h"
#include "MeshShaderLoader.h"
#include "DepthShaderLoader.h"

namespace PM3D {
	class CMeshInstance : public CObjet3D
	{
	public:
		CMeshInstance(GeometryInitData::GeometryStruct data,CDispositifD3D11* pDispositif);

		// Destructeur
		virtual ~CMeshInstance();

		virtual void Draw() override { throw "can not use Draw on Instance Directly"; };
		virtual void BasicRender(const XMMATRIX& matViewProj) override { throw "can not use BasicRender on Instance Directly"; };
		void Draw(const FXMVECTOR& scale, const FXMVECTOR& position, const FXMVECTOR& rotationQuaternion);
		void BasicRender(const FXMVECTOR& scale, const FXMVECTOR& position, const FXMVECTOR& rotationQuaternion, const XMMATRIX& matViewProj);

		wstring GetFilename() { return m_baseMeshName; };
		 
		//specifique a la mesh
		GeometryInitData::Type meshType;
		uint32_t m_verticesSize;
		uint32_t m_indicesSize;
		
		void getVertices(CSommetTerrain*& verts);
		void getIndices(uint32_t*& indices);
		uint32_t getVerticesSize() { return static_cast<uint32_t>(m_verticesSize); };
		uint32_t getIndicesSize() { return static_cast<uint32_t>(m_indicesSize); };

	private:

		XMMATRIX matWorld;
		float rotation;

		//Mesh Instance Name "represented by the path of the mesh"
		wstring m_baseMeshName;
		bool m_shouldBeUsedToBakeColision;

		// binarary data
		int m_NombreSubset;						// Nombre de sous-objets dans le mesh
		std::vector<int> SubsetMaterialIndex;	// Index des matériaux
		std::vector<int> SubsetIndex;			// Index des sous-objets
		vector <CMaterialInstance> Materials;			// default material , instance need to be set during drawing phase

		
		CDispositifD3D11* pDispositif;
		void LoadMeshFromObjFile(GeometryInitData::GeometryStruct& data);
		void LoadMeshAsBillBoard(GeometryInitData::GeometryStruct& data);
		void LoadMeshFromHightMap(GeometryInitData::GeometryStruct& data);
		void InitEffet();
		void TransfertObjet(const IChargeur& chargeur);
		void EcrireFichierBinaire(const IChargeur& chargeur, const std::wstring& nomFichier);
		void LireFichierBinaire(const std::wstring& nomFichier);

		// Créations des sommets dans un tableau temporaire
		std::unique_ptr<CSommetTerrain[]> loadSubMesh(const IChargeur& chargeur);
		void InitMaterialLocalInstance(const IChargeur& chargeur);
		template<class T,class B>
		void createBuffer(ID3D11Device* pD3DDevice, uint32_t ByteWidth, D3D11_USAGE usage, D3D11_BIND_FLAG bindFlag, T* pData, B*& pbuffer, const int code);

		bool isBillboard;
		bool isTwoFaced;

		std::unique_ptr<CSommetTerrain[]> pVertex;
		std::unique_ptr <uint32_t[]> pIndex;

		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;

		//pour les ombres
		DepthShaderLoader depthShader;
	};
};
#endif