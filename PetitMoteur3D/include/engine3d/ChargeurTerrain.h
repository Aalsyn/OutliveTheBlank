#pragma once

#ifndef CHARGEURTERRAIN_H
#define CHARGEURTERRAIN_H

#include "chargeur.h"
#include <string>
#include <map>
#include "opencv2/opencv.hpp"
#include <MaterialData.h>


using namespace cv;
using namespace std;


class CChargeurTerrain : public PM3D::IChargeur {
	

public:
	CChargeurTerrain() = default;

	~CChargeurTerrain() {
		delete[] pVertices;
		delete[] pIndices;
		delete[] verticesData;
	}

	virtual void release();
	virtual void releaseTempDataFromMemory();

public:

	CSommetTerrain* pVertices;
	int32_t* pIndices;

	XMFLOAT3* verticesData; // temp variable to store vertex location while waiting for the normals to build the CSommetTerrain Object

	int verticesSize = 0;
	int indicesSize = 0;

	Mat image;

	// main gauche
	int dx = 0;
	int dy = 0;
	int dz = 0;

	const float echelleXZ = 1.0f;
	const float echelleY = 1.0f;

	XMVECTOR obtenirPosition(int x, int z) const { return XMLoadFloat3(&verticesData[x + z * dx]); }
	XMFLOAT3 computeNormalAt(int x, int z);

	void lireFichierHeightmap(std::string imagePath);
	void construireTerrain(float echelleXZ, float echelleY);
	void calculerNormales();
	void construireIndex();

public:
	vector<CMaterialData> materials = { CMaterialData()};
	void generateTerrain(std::string imagePath) { /// C'EST l'equivalent du MAIN DU RUSH 1 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		lireFichierHeightmap(imagePath);
		//const float terrainscallingFactorXY = (dx > dy) ? 15.0f/dx : 15.0f/dy; // normalize terrain
		construireTerrain(echelleXZ, echelleY);
		calculerNormales();
		construireIndex();

	}

	HRESULT Chargement(const PM3D::CParametresChargement& param) { return 1; };

	size_t GetNombreSommets() const { return verticesSize; };//
	size_t GetNombreIndex() const { return indicesSize; };//
	const void* GetIndexData()  const { return static_cast<void*>(pIndices); };//
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