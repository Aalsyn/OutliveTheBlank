#pragma once

#ifndef CHARGEURBILLBOARD_H
#define CHARGEURBILLBOARD_H

#include "chargeur.h"
#include <string>
#include <map>
#include "opencv2/opencv.hpp"
#include <MaterialData.h>
#include <SommetTerrain.h>


using namespace cv;
using namespace std;


class CChargeurBillBoard : public PM3D::IChargeur {
	


public:
	CChargeurBillBoard() = default;
	~CChargeurBillBoard() {
		delete pIndices;
		delete pVertices;
	};

	// bill board offset, should be set using gameobjet rather than dx and dy
	float dx = 1;
	float dy = 1;
	float dz = 0;
public:

	CSommetTerrain* pVertices = new CSommetTerrain[4]{
		// Le devant du bloc
		CSommetTerrain(XMFLOAT3(-dx / 2, dy / 2, -dz / 2), XMFLOAT3(0.0f, 0.0f, -1.0f),XMFLOAT2(0.0f, 0.0f)),
		CSommetTerrain(XMFLOAT3(dx / 2, dy / 2, -dz / 2), XMFLOAT3(0.0f, 0.0f, -1.0f),XMFLOAT2(1.0f, 0.0f)),
		CSommetTerrain(XMFLOAT3(dx / 2, -dy / 2, -dz / 2), XMFLOAT3(0.0f, 0.0f, -1.0f),XMFLOAT2(1.0f, 1.0f)),
		CSommetTerrain(XMFLOAT3(-dx / 2, -dy / 2, -dz / 2), XMFLOAT3(0.0f, 0.0f, -1.0f),XMFLOAT2(0.0f, 1.0f))
	};

	int32_t* pIndices = new int32_t[6]{
		0,1,2,			// devant
		0,2,3			// devant
	};
	


	int verticesSize = 4;
	int indicesSize = 6;

public:
	vector<CMaterialData> materials = { CMaterialData()};
	void generateTerrain(std::string imagePath) { /// C'EST l'equivalent du MAIN DU RUSH 1 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}

	HRESULT Chargement(const PM3D::CParametresChargement& param) { return 1; };

	size_t GetNombreSommets() const { return verticesSize; };//
	size_t GetNombreIndex() const { return indicesSize; };//
	int32_t* getIndices() { static_cast<void*>(pIndices); }
	const void* GetIndexData() const { return pIndices; };//
	int GetNombreSubset() const { return 1; };//
	size_t  GetNombreMaterial() const { return materials.size(); };//
	void GetMaterial(int _i,
		std::string& _NomFichierTexture,
		std::string& _NomMateriau,
		XMFLOAT4& _Ambient,
		XMFLOAT4& _Diffuse,
		XMFLOAT4& _Specular,
		float& _Puissance,
		float& _uScaling,
		float& _vScaling
	)
		const {
		_NomFichierTexture = materials[_i].NomFichierTexture;
		_NomMateriau = materials[_i].NomMateriau;
		_Ambient = materials[_i].Ambient;
		_Diffuse = materials[_i].Diffuse;
		_Specular = materials[_i].Specular;
		_Puissance = materials[_i].Puissance;
		_uScaling = materials[_i].uScaling;
		_vScaling = materials[_i].vScaling;
	};//

	const std::string& GetMaterialName(int i) const { return materials[i].NomMateriau;};//

	void CopieSubsetIndex(std::vector<int>& dest) const { dest.push_back(0);  dest.push_back(indicesSize);
	};//

	const XMFLOAT3& GetPosition(int NoSommet) const { return pVertices[NoSommet].m_Position; };//
	const XMFLOAT2& GetCoordTex(int NoSommet) const { return pVertices[NoSommet].m_CoordTex; };//
	const XMFLOAT3& GetNormale(int NoSommet) const { return pVertices[NoSommet].m_Normal; };//
	const void setMaterials(vector< CMaterialData> mats) {
		materials = mats;
	};
};

#endif