#include <stdafx.h>
#include "dispositifD3D11.h"
#include "MeshInstanceManager.h"


CMeshInstance* const CMeshInstanceManager::GetNewInstance(const GeometryInitData::GeometryStruct& data, CDispositifD3D11* pDispositif)
{
	// On vérifie si la texture est déjà dans notre liste
	CMeshInstance* pMesh = GetInstance(data.name);
	// Si non, on la crée
	if (!pMesh)
	{
		auto mesh = std::make_unique<CMeshInstance>(data, pDispositif);
		pMesh = mesh.get();
		// Puis, il est ajouté à la scène
		meshInstanceList.push_back(std::move(mesh));
	}
	assert(pMesh);
	return pMesh;

}

CMeshInstance* const CMeshInstanceManager::GetInstance(const wstring& filename)
{
	CMeshInstance* pMesh = nullptr;
	for (auto& mesh : meshInstanceList)
	{
		if (mesh->GetFilename() == filename)
		{
			pMesh = mesh.get();
			break;
		}
	}
	return pMesh;
}


