#pragma once
#include "MeshInstance.h"
#include "MeshInstanceManager.h"
#include "GeometryInitData.h"


class CObjectRenderInstance : public CObjet3D
{
private:
	std::vector<CMeshInstance*> m_MeshLods;
	size_t m_NumberOfLods;

	// need to be implemented in the GameObject
	void virtual Draw() = 0;

private :
	/*
	lookup at what level of detail the mesh should be drawn, 
	we might want to considere using screen space size instead of distance if we have the time.
	*/
	int lodsLookup(int distance) {
		for (auto& levelIt : lodsLookUpTable){
			if (levelIt.first > distance) {
				return levelIt.second;
			};
		}
		return -1;
	};



protected:

	/*
	compute distance between two point
	*/
	float computeDistance(const XMVECTOR& v1, const XMVECTOR& v2)
	{
		XMVECTOR vectorSub = XMVectorSubtract(v1, v2);
		XMVECTOR length = XMVector3Length(vectorSub);

		float distance = 0.0f;
		XMStoreFloat(&distance, length);
		return distance;
	}
	CObjectRenderInstance();
	CObjectRenderInstance(GeometryInitData& geometryInitData, string materialInstanceName, CDispositifD3D11* pDispositif);

	/*
	retrieve camera to get distance from camera
	*/
	XMVECTOR getCameraPosition();
	

	map<int, int> lodsLookUpTable = {};

	/*
	
	*/
	void Draw(const int& cameraDistance, const FXMVECTOR& scale, const FXMVECTOR& position, const FXMVECTOR& rotationQuaternion) {
		int lod = lodsLookup(cameraDistance);
		if (lod == -1 || lod >= m_NumberOfLods) return;
		m_MeshLods.at(lod)->Draw(scale, position, rotationQuaternion);
	};

	void BasicRender(const int& cameraDistance, const FXMVECTOR& scale, const FXMVECTOR& position, const FXMVECTOR& rotationQuaternion, const XMMATRIX& matViewProj) {
		int lod = lodsLookup(cameraDistance);
		if (lod == -1 || lod >= m_NumberOfLods) return;
		m_MeshLods.at(lod)->BasicRender(scale, position, rotationQuaternion, matViewProj);
	}
};

