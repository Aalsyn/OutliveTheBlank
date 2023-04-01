#pragma once
#include "MeshInstance.h"
#include "DispositifD3D11.h"
#include "GeometryInitData.h"
using namespace PM3D;

class CMeshInstanceManager
{

public:
	CMeshInstance* const GetNewInstance(const GeometryInitData::GeometryStruct& initData, CDispositifD3D11* pDispositif);
	CMeshInstance* const GetInstance(const wstring& filename);
private:
	// Le tableau de textures
	std::vector<std::unique_ptr<CMeshInstance>> meshInstanceList;
};

