
#include "StdAfx.h"
#include "MeshInstance.h"
#include "util.h"
#include "DispositifD3D11.h"
#include "resource.h"
#include "MoteurWindows.h"
#include "GestionnaireDeTextures.h"
#include "chargeur.h"
#include "chargeurObj.h"
#include "GeometryInitData.h"
#include "ShaderBuffers.h"
#include "ShaderInstanceManager.h"
#include <commdlg.h>
#include <ChargeurTerrain.h>
#include <ChargeurBillBoard.h>


namespace PM3D
{
	CMeshInstance::~CMeshInstance()
	{
		DXRelacher(pVertexBuffer);
		DXRelacher(pIndexBuffer);
	}

	CMeshInstance::CMeshInstance(
		GeometryInitData::GeometryStruct data,
		CDispositifD3D11* pDispositif_)
		: pDispositif(pDispositif_) // Prendre en note le dispositif
		, matWorld(XMMatrixIdentity())
		, rotation(0.0f)
		, pVertexBuffer(nullptr)
		, pIndexBuffer(nullptr)
		, m_baseMeshName(data.name)
		, m_shouldBeUsedToBakeColision(data.shouldBeUsedToBakeColision)
		, meshType(data.type)
	{
		std::ifstream fichier;
		// try loading with binary if it exist :
		fichier.open(data.filePath + data.fileName + data.binaryExtension, std::ios::in | std::ios_base::binary);
		if (data.shouldForceObjLoad);
		else if (fichier.is_open() ) {
			fichier.close();
			LireFichierBinaire(data.filePath + data.fileName + data.binaryExtension);
			InitEffet();
			return;
		}

		switch (data.type) {
			case GeometryInitData::MESH: {
				LoadMeshFromObjFile(data);
				break;
				}
			case GeometryInitData::BILLBOARD: {
				LoadMeshAsBillBoard(data);
				break;
				}
			case GeometryInitData::TERRAIN: {
				LoadMeshFromHightMap(data);
				break;
				}
		}
		// load mesh with newly created binnary file
		LireFichierBinaire(data.filePath + data.fileName + data.binaryExtension);
		InitEffet();
		return;
	}

	void CMeshInstance::LoadMeshFromObjFile(GeometryInitData::GeometryStruct& data)
	{

		std::ifstream fichier;

		// try saving the obj as a binary for futur use and then load the binary :
		fichier.open(data.filePath + data.fileName + data.objectExtension, std::ios::in | std::ios_base::binary);
		if (fichier.is_open()) {

			fichier.close();
			CParametresChargement te;
			te.NomChemin = { data.filePath.begin(), data.filePath.end() };
			te.NomFichier = string({ data.fileName.begin(), data.fileName.end() }) + string({ data.objectExtension.begin(), data.objectExtension.end() });
			te.bInverserCulling = true;
			te.bMainGauche = false;
			CChargeurOBJ t;

			DXEssayer(t.Chargement(te), 0);

			//TransfertObjet(t);
			EcrireFichierBinaire(t, data.filePath + data.fileName + data.binaryExtension);
			return;
		}
		throw "Could not Load Mesh Because no file exist at desired path";
	}

	void CMeshInstance::LoadMeshAsBillBoard(GeometryInitData::GeometryStruct& data)
	{

		std::ifstream fichier;

		// try saving the obj as a binary for futur use and then load the binary :
		fichier.open(data.filePath + data.fileName + data.imageExtension, std::ios::in | std::ios_base::binary);
		if (fichier.is_open()) {

			fichier.close();

			CChargeurBillBoard t;
			wstring file = data.filePath + data.fileName + data.imageExtension;
			t.setMaterials(data.customMats);
			EcrireFichierBinaire(t, data.filePath + data.fileName + data.binaryExtension);
			return;
		}
		throw "Could not Load image as BillBoard and convert it to mesh Because no file exist at desired path";
	}

	void CMeshInstance::LoadMeshFromHightMap(GeometryInitData::GeometryStruct& data)
	{

		std::ifstream fichier;

		// try saving the obj as a binary for futur use and then load the binary :
		fichier.open(data.filePath + data.fileName + data.imageExtension, std::ios::in | std::ios_base::binary);
		if (fichier.is_open()) {

			fichier.close();
			
			CChargeurTerrain t;
			wstring file = data.filePath + data.fileName + data.imageExtension;
			t.generateTerrain({ file.begin(), file.end() });
			t.setMaterials(data.customMats);
			//DXEssayer(t.Chargement(te), 0);

			//TransfertObjet(t);
			EcrireFichierBinaire(t, data.filePath + data.fileName + data.binaryExtension);
			t.release();
			return;
		}
		throw "Could not Load HighMap and convert it to mesh Because no file exist at desired path";
	}

	void CMeshInstance::getVertices(CSommetTerrain*& verts)
	{
		verts = pVertex.get();
	}

	void CMeshInstance::getIndices(uint32_t*& indices)
	{
		indices = pIndex.get();
	}

	template<class T, class B>
	void CMeshInstance::createBuffer(ID3D11Device* pD3DDevice, uint32_t ByteWidth, D3D11_USAGE usage, D3D11_BIND_FLAG bindFlag, T* pData, B*& pbuffer, const int code) {

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = ByteWidth;
		bd.Usage = usage;
		bd.CPUAccessFlags = 0;
		bd.BindFlags =  bindFlag;
		/*bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bd.StructureByteStride = sizeof(pData);*/

		/*bd.Usage = usage;
		bd.ByteWidth = ByteWidth;
		bd.BindFlags = bindFlag;
		bd.CPUAccessFlags = D3D11_BIND_UNORDERED_ACCESS;*/
		//bd.CPUAccessFlags += D3D11_CPU_ACCESS_READ;
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = pData;
		pbuffer = nullptr;

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pbuffer), code);
	}



	void CMeshInstance::InitMaterialLocalInstance(const IChargeur& chargeur)
	{
		for (int32_t i = 0; i < chargeur.GetNombreMaterial(); ++i)
		{
			CMaterialData mat;

			chargeur.GetMaterial(i, mat.NomFichierTexture,
				mat.NomMateriau,
				mat.Ambient,
				mat.Diffuse,
				mat.Specular,
				mat.Puissance,
				mat.uScaling,
				mat.vScaling
			);

			Materials.push_back(CMaterialInstance(pDispositif->GetD3DDevice(), mat));
		}
	}

	void CMeshInstance::InitEffet()
	{
		ID3D11Device* pDevice = pDispositif->GetD3DDevice();
		for (auto& mat : Materials) {
			mat.setDevice(pDevice);
			std::wstring filename;
			switch (meshType) {
			case GeometryInitData::MESH: {
				filename = L"ShadowTextureShader.fx";
				break;
			}
			case GeometryInitData::BILLBOARD: {
				filename = L"BillboardTextureShader.fx";

				break;
			}
			case GeometryInitData::TERRAIN: {
				filename = L"HeightMapShader.fx";
				break;
			}
			}
			mat.initShaderLoader(filename, meshType);
			//mat.initShaderLoader(L"BasicTextureShader.fx");
		}
		depthShader.setDevice(pDevice);
		depthShader.initShader(L"depthShader.fx");
		depthShader.initBuffers(meshType);
	}


	std::unique_ptr<CSommetTerrain[]> CMeshInstance::loadSubMesh(const IChargeur& chargeur)
	{
		m_verticesSize = static_cast<uint32_t>(chargeur.GetNombreSommets());
		std::unique_ptr<CSommetTerrain[]> subMeshArr(new CSommetTerrain[m_verticesSize]);
		for (uint32_t i = 0; i < m_verticesSize; ++i)
		{
			subMeshArr[i].m_Position = chargeur.GetPosition(i);
			subMeshArr[i].m_Normal = chargeur.GetNormale(i);
			subMeshArr[i].m_CoordTex = chargeur.GetCoordTex(i);
		}
		return subMeshArr;
	}

	void CMeshInstance::Draw(const FXMVECTOR& scale, const FXMVECTOR& position, const FXMVECTOR& rotationQuaternion)
	{
		// Obtenir le contexte
		ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();

		const XMMATRIX MeshInstanceMatWorld = XMMatrixAffineTransformation(
			scale,
			{ 0,0,0 },
			rotationQuaternion,
			position
		);

		if (meshType == GeometryInitData::TERRAIN) {
			Materials.front().setBuffers(pVertexBuffer, pIndexBuffer, MeshInstanceMatWorld, Materials, pImmediateContext);
		}
		//meshType != GeometryInitData::BILLBOARD ? rotationQuaternion : XMLoadFloat4( CMoteurWindows::GetInstance().GetMatView().r[0])
		// Dessiner les subsets non-transparents
		if (meshType == GeometryInitData::BILLBOARD || meshType == GeometryInitData::TERRAIN) pDispositif->ActiverMelangeAlpha();
		for (int32_t i = 0; i < m_NombreSubset; ++i)
		{
			int32_t indexStart = SubsetIndex[i];
			int32_t indexDrawAmount = SubsetIndex[i + 1] - SubsetIndex[i];
			if (indexDrawAmount)
			{
				if (meshType != GeometryInitData::TERRAIN) {
					Materials[SubsetMaterialIndex[i]].setBuffers(pVertexBuffer, pIndexBuffer, MeshInstanceMatWorld, pImmediateContext);
				}
				pImmediateContext->DrawIndexed(indexDrawAmount, indexStart, 0);
			}
		}
		if (meshType == GeometryInitData::BILLBOARD || meshType == GeometryInitData::TERRAIN) pDispositif->DesactiverMelangeAlpha();

	}

	void CMeshInstance::BasicRender(const FXMVECTOR& scale, const FXMVECTOR& position, const FXMVECTOR& rotationQuaternion, const XMMATRIX& matViewProj)
	{
		// Obtenir le contexte
		ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();

		const XMMATRIX MeshInstanceMatWorld = XMMatrixAffineTransformation(
			scale,
			{ 0,0,0 },
			rotationQuaternion,
			position
		);
		// Dessiner les subsets non-transparents
		for (int32_t i = 0; i < m_NombreSubset; ++i)
		{
			int32_t indexStart = SubsetIndex[i];
			int32_t indexDrawAmount = SubsetIndex[i + 1] - SubsetIndex[i];
			if (indexDrawAmount)
			{
				depthShader.setMeshBuffers(pVertexBuffer, pIndexBuffer, pImmediateContext);
				depthShader.setPovBuffer(MeshInstanceMatWorld, matViewProj, pImmediateContext);
				depthShader.setMaterialBuffer(Materials[SubsetMaterialIndex[i]].matData, true, pImmediateContext);
				depthShader.Apply(pImmediateContext);
				//Materials[SubsetMaterialIndex[i]].setBuffers(pVertexBuffer, pIndexBuffer, MeshInstanceMatWorld, pImmediateContext);

				pImmediateContext->DrawIndexed(indexDrawAmount, indexStart, 0);
			}
		}
	}

	void CMeshInstance::TransfertObjet(const IChargeur& chargeur)
	{

		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();
		// 1. SOMMETS a) Créations des sommets dans un tableau temporaire
		{
			auto submesh = loadSubMesh(chargeur);

			// 1. SOMMETS b) Création du vertex buffer et copie des sommets
			//createBuffer(pD3DDevice,static_cast<uint32_t>(sizeof(CSommetTerrain) * chargeur.GetNombreSommets()), D3D11_BIND_VERTEX_BUFFER, submesh.get(), pVertexBuffer, DXE_CREATIONVERTEXBUFFER);
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));

			bd.Usage = D3D11_USAGE_IMMUTABLE;
			bd.ByteWidth = static_cast<UINT>(sizeof(CSommetTerrain) * chargeur.GetNombreSommets());
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = submesh.get();
			pVertexBuffer = nullptr;

			DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);
		}

		// 2. INDEX - Création de l'index buffer et copie des indices
		/* Les indices étant habituellement des entiers, j'ai
		//            pris directement ceux du chargeur mais attention au
					format si vous avez autre chose que DXGI_FORMAT_R32_UINT*/
		{
			//createBuffer(pD3DDevice,static_cast<uint32_t>(sizeof(uint32_t) * chargeur.GetNombreIndex()), D3D11_BIND_INDEX_BUFFER, chargeur.GetIndexData(), pIndexBuffer, DXE_CREATIONINDEXBUFFER);
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));

			bd.Usage = D3D11_USAGE_IMMUTABLE;
			bd.ByteWidth = static_cast<uint32_t>(sizeof(uint32_t) * chargeur.GetNombreIndex());
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = chargeur.GetIndexData();
			pIndexBuffer = nullptr;

			DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer),
				DXE_CREATIONINDEXBUFFER);
		}

		// 3. Les sous-objets
		m_NombreSubset = chargeur.GetNombreSubset();

		//    Début de chaque sous-objet et un pour la fin
		SubsetIndex.reserve(m_NombreSubset);
		chargeur.CopieSubsetIndex(SubsetIndex);

		// 4. MATERIAUX
		/*4a) Créer un matériau de défaut en index 0
			 Vous pourriez changer les valeurs, j'ai conservé
			 celles du constructeur*/
		Materials.reserve(chargeur.GetNombreMaterial() + 1);
		Materials.emplace_back(CMaterialInstance());

		// 4b) Copie des matériaux dans la version locale
		InitMaterialLocalInstance(chargeur);

		// 4c) Trouver l'index du materiau pour chaque sous-ensemble
		SubsetMaterialIndex.reserve(chargeur.GetNombreSubset());
		for (int32_t i = 0; i < chargeur.GetNombreSubset(); ++i)
		{
			int32_t index;
			for (index = 0; index < Materials.size(); ++index)
			{
				if (Materials[index].matData.NomMateriau == chargeur.GetMaterialName(i))
				{
					break;
				}
			}

			if (index >= Materials.size())
			{
				index = 0;  // valeur de défaut
			}

			SubsetMaterialIndex.push_back(index);
		}

		// 4d) Chargement des textures
		CGestionnaireDeTextures& TexturesManager = CMoteurWindows::GetInstance().GetTextureManager();

		for (uint32_t i = 0; i < Materials.size(); ++i)
		{
			if (Materials[i].matData.NomFichierTexture.length() > 0)
			{
				const std::wstring ws(Materials[i].matData.NomFichierTexture.begin(), Materials[i].matData.NomFichierTexture.end());
				Materials[i].pBaseTexture = TexturesManager.GetNewTexture(ws.c_str(), pDispositif)->GetD3DTexture();
			}
		}
	}


	void CMeshInstance::EcrireFichierBinaire(const IChargeur& chargeur, const std::wstring& nomFichier)
	{
		std::ofstream fichier;
		fichier.open(nomFichier, std::ios::out | std::ios_base::binary);
		// 1. SOMMETS a) Créations des sommets dans un tableau temporaire
		{
			int32_t nombreSommets = static_cast<int32_t>(chargeur.GetNombreSommets());
			std::unique_ptr<CSommetTerrain[]> ts(new CSommetTerrain[nombreSommets]);

			for (int32_t i = 0; i < nombreSommets; ++i)
			{
				ts[i].m_Position = chargeur.GetPosition(i);
				ts[i].m_Normal = chargeur.GetNormale(i);
				ts[i].m_CoordTex = chargeur.GetCoordTex(i);
			}

			// 1. SOMMETS b) Écriture des sommets dans un fichier binaire
			fichier.write((char*)&nombreSommets, sizeof(nombreSommets));
			fichier.write((char*)ts.get(), nombreSommets * sizeof(CSommetTerrain));

			m_verticesSize = nombreSommets;

		}

		// 2. INDEX 
		const int32_t nombreIndex = static_cast<int32_t>(chargeur.GetNombreIndex());

		fichier.write((char*)&nombreIndex, sizeof(nombreIndex));
		int bsize = nombreIndex * sizeof(int32_t);
		fichier.write((char*)chargeur.GetIndexData(), nombreIndex * sizeof(int32_t));

		// 3. Les sous-objets
		const int32_t NombreSubset = chargeur.GetNombreSubset();

		//    Début de chaque sous-objet et un pour la fin
		std::vector<int32_t> SI;
		SI.reserve(NombreSubset);
		chargeur.CopieSubsetIndex(SI);

		fichier.write((char*)&NombreSubset, sizeof(NombreSubset));
		fichier.write((char*)SI.data(), (NombreSubset + 1) * sizeof(int32_t));

		// 4. MATERIAUX
		// 4a) Créer un matériau de défaut en index 0
		//     Vous pourriez changer les valeurs, j'ai conservé 
		//     celles du constructeur
		int32_t NbMaterial = static_cast<int32_t>(chargeur.GetNombreMaterial());
		std::vector<CMaterialData> MatLoad;
		MatLoad.reserve(NbMaterial + 1);
		MatLoad.emplace_back(CMaterialData());

		// 4b) Copie des matériaux dans la version locale
		CMaterialData mat;
		for (int32_t i = 0; i < NbMaterial; ++i)
		{
			chargeur.GetMaterial(
				i,
				mat.NomFichierTexture,
				mat.NomMateriau,
				mat.Ambient,
				mat.Diffuse,
				mat.Specular,
				mat.Puissance,
				mat.uScaling,
				mat.vScaling
			);

			MatLoad.push_back(mat);
		}

		NbMaterial++;
		fichier.write((char*)&NbMaterial, sizeof(int32_t));

		MaterialBlock mb;
		for (int32_t i = 0; i < NbMaterial; ++i)
		{
			MatLoad[i].MatToBlock(mb);
			fichier.write((char*)&mb, sizeof(MaterialBlock));
		}

		// 4c) Trouver l'index du materiau pour chaque sous-ensemble
		std::vector<int32_t> SubsetMI;
		SubsetMI.reserve(NombreSubset);
		for (int32_t i = 0; i < NombreSubset; ++i)
		{
			int32_t index;
			for (index = 0; index < MatLoad.size(); ++index)
			{
				if (MatLoad[index].NomMateriau == chargeur.GetMaterialName(i)) break;
			}

			if (index >= MatLoad.size()) index = 0;  // valeur de défaut

			SubsetMI.push_back(index);
		}

		fichier.write((char*)SubsetMI.data(), (NombreSubset) * sizeof(int32_t));

		m_indicesSize = nombreIndex;

	}

	void CMeshInstance::LireFichierBinaire(const std::wstring& nomFichier)
	{
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

		std::ifstream fichier;
		fichier.open(nomFichier, std::ios::in | std::ios_base::binary);
		assert(fichier.is_open());

		// 1. SOMMETS a) Créations des sommets dans un tableau temporaire
		{
			int32_t nombreSommets;
			fichier.read((char*)&nombreSommets, sizeof(nombreSommets));

			std::unique_ptr<CSommetTerrain[]> ts(new CSommetTerrain[nombreSommets]);

			// 1. SOMMETS b) Lecture des sommets à partir d'un fichier binaire
			fichier.read((char*)ts.get(), nombreSommets * sizeof(CSommetTerrain));

			// 1. SOMMETS b) Création du vertex buffer et copie des sommets
			createBuffer(pD3DDevice, sizeof(CSommetTerrain) * nombreSommets, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER, ts.get(), pVertexBuffer, DXE_CREATIONVERTEXBUFFER);

			if (m_shouldBeUsedToBakeColision) {
				pVertex = move(ts);
			}

			m_verticesSize = nombreSommets;
		}

		// 2. INDEX 
		{
			int32_t nombreIndex;
			fichier.read((char*)&nombreIndex, sizeof(nombreIndex));

			std::unique_ptr<uint32_t[]> index(new uint32_t[nombreIndex]);
			fichier.read((char*)index.get(), nombreIndex * sizeof(uint32_t));

			createBuffer(pD3DDevice, sizeof(uint32_t) * nombreIndex, D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER, index.get(), pIndexBuffer, DXE_CREATIONINDEXBUFFER);
			/*D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));

			bd.Usage = D3D11_USAGE_IMMUTABLE;
			bd.ByteWidth = sizeof(uint32_t) * nombreIndex;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = index.get();
			pIndexBuffer = nullptr;

			DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer),
			DXE_CREATIONINDEXBUFFER);
			*/

			if (m_shouldBeUsedToBakeColision) {
				pIndex = move(index);
			}

			m_indicesSize = nombreIndex;
		}

		// 3. Les sous-objets
		fichier.read((char*)&m_NombreSubset, sizeof(m_NombreSubset));
		//    Début de chaque sous-objet et un pour la fin
		{
			std::unique_ptr<int32_t[]> si(new int32_t[m_NombreSubset + 1]);

			fichier.read((char*)si.get(), (m_NombreSubset + 1) * sizeof(int32_t));
			SubsetIndex.assign(si.get(), si.get() + (m_NombreSubset + 1));
		}

		// 4. MATERIAUX
		// 4a) Créer un matériau de défaut en index 0
		//     Vous pourriez changer les valeurs, j'ai conservé 
		//     celles du constructeur


		int32_t NbMaterial;
		fichier.read((char*)&NbMaterial, sizeof(int32_t));

		Materials.resize(NbMaterial);

		MaterialBlock mb;
		for (int32_t i = 0; i < NbMaterial; ++i)
		{
			fichier.read((char*)&mb, sizeof(MaterialBlock));
			Materials[i].matData.BlockToMat(mb);
		}

		// 4c) Trouver l'index du materiau pour chaque sous-ensemble
		{
			std::unique_ptr<int32_t[]> smi(new int32_t[m_NombreSubset]);

			fichier.read((char*)smi.get(), (m_NombreSubset) * sizeof(int32_t));
			SubsetMaterialIndex.assign(smi.get(), smi.get() + m_NombreSubset);
		}

		// 4d) Chargement des textures
		CGestionnaireDeTextures& TexturesManager = CMoteurWindows::GetInstance().GetTextureManager();

		for (uint32_t i = 0; i < Materials.size(); ++i)
		{
			if (Materials[i].matData.NomFichierTexture.length() > 0)
			{
				std::wstring ws(Materials[i].matData.NomFichierTexture.begin(), Materials[i].matData.NomFichierTexture.end());
				Materials[i].pBaseTexture = TexturesManager.GetNewTexture(ws.c_str(), pDispositif)->GetD3DTexture();
			}
		}
	}



} // namespace PM3D
