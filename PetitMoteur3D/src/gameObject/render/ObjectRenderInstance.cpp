#include <stdafx.h>
#include "ObjectRenderInstance.h"
#include "MoteurWindows.h"

CObjectRenderInstance::CObjectRenderInstance() {
	m_NumberOfLods = 0;
	m_MeshLods = {};
}

CObjectRenderInstance::CObjectRenderInstance(GeometryInitData& geometryInitData, string materialInstanceName , CDispositifD3D11* pDispositif) :
	m_NumberOfLods(geometryInitData.lods.size())
{
	CMeshInstanceManager& MeshManager = PM3D::CMoteurWindows::GetInstance().GetMeshInstanceManager();
	m_MeshLods.reserve(m_NumberOfLods);
	int lodIndex = -1;
	for (auto data : geometryInitData.lods) {
		lodIndex++;
		m_MeshLods.push_back(MeshManager.GetNewInstance(data,pDispositif));
		lodsLookUpTable[data.lodActivationThresHold] = lodIndex;
	}
}

XMVECTOR CObjectRenderInstance::getCameraPosition()
{
	return PM3D::CMoteurWindows::GetInstance().GetCamera()->getPosition();
}