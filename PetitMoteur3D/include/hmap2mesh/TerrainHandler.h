#pragma once

#ifndef TERRAINHANDLER_H
#define TERRAINHANDLER_H


#include <map>
#include "opencv2/opencv.hpp"
#include "chargeur.h"

using namespace cv;
using namespace std;


class CTerrainHandler  {
	CTerrainHandler(){};

public:
	CTerrainHandler& operator=(const CTerrainHandler&) = delete;

	static CTerrainHandler& get() {
		static CTerrainHandler singleton;
		return singleton;
	}

	~CTerrainHandler() {
		delete[] pVertices;
		delete[] pIndices;
		delete[] verticesData;
	}

	virtual void release();
	virtual void releaseTempDataFromMemory();

public:

	CSommetTerrain* pVertices;
	int* pIndices;

	XMFLOAT3* verticesData; // temp variable to store vertex location while waiting for the normals to build the CSommetTerrain Object

	int verticesSize = 0;
	int indicesSize = 0;

	Mat image;

	// main gauche
	int dx = 0;
	int dy = 0;
	int dz = 0;

	const float echelleXZ = 1;
	const float echelleY = 0.05f;

	XMVECTOR obtenirPosition(int x, int z) const { return XMLoadFloat3(&verticesData[x + z * dx]); }
	XMFLOAT3 computeNormalAt(int x, int z);
	double getTerrainY(const float x, const float z) const;

	void lireFichierHeightmap(std::string imagePath);
	void construireTerrain(float echelleXZ, float echelleY);
	void calculerNormales();
	void construireIndex();
	int enregistrerTout();

public:
	vector<string> texturesPaths;
	void generateTerrain(std::string imagePath) { /// C'EST l'equivalent du MAIN DU RUSH 1 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		lireFichierHeightmap(imagePath);
		//const float terrainscallingFactorXY = (dx > dy) ? 15.0f/dx : 15.0f/dy; // normalize terrain
		construireTerrain(echelleXZ, echelleY);
		calculerNormales();
		construireIndex();
		//enregistrerTout();

	}

	HRESULT Chargement(const PM3D::CParametresChargement& param) { return 1; };

	size_t GetNombreSommets() const { return verticesSize; };//
	size_t GetNombreIndex() const { return indicesSize; };//
	const void* GetIndexData()  const { return static_cast<void*>(pVertices); };//
	int GetNombreSubset() const { return 1; };//
	size_t  GetNombreMaterial() const { return 3; };//
	void GetMaterial(int _i,
		std::string& _NomFichierTexture,
		std::string& _NomMateriau,
		XMFLOAT4& _Ambient,
		XMFLOAT4& _Diffuse,
		XMFLOAT4& _Specular,
		float& _Puissance) const {};//

	const std::string& GetMaterialName(int i) const { return texturesPaths[i];};//

	void CopieSubsetIndex(std::vector<int>& dest) const { dest.push_back(0); };//

	const XMFLOAT3& GetPosition(int NoSommet) const { return pVertices[NoSommet].m_Position; };//
	const XMFLOAT2& GetCoordTex(int NoSommet) const { return pVertices[NoSommet].m_CoordTex; };//
	const XMFLOAT3& GetNormale(int NoSommet) const { return pVertices[NoSommet].m_Normal; };//

};

#endif