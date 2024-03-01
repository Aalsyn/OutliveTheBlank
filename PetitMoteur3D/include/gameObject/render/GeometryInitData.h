#pragma once
#include "MaterialData.h"
using namespace std;

struct GeometryInitData
{
	enum Type {
		MESH,
		BILLBOARD,
		TERRAIN
	};

	struct GeometryStruct {
				
		wstring name;
		wstring fileName;
		wstring filePath;
		wstring objectExtension = L".obj";
		wstring imageExtension = L".png";
		wstring binaryExtension = L".OMB";

		vector<CMaterialData> customMats;
		Type type = MESH;
		bool shouldForceObjLoad = false;
		bool shouldBeUsedToBakeColision = false;

		int lodActivationThresHold;
		GeometryStruct(Type type, wstring name, wstring fileName, wstring filePath, int lodActivationThresHold = MAXINT32, bool shouldBeUsedToBakeColision = false, bool shouldForceObjLoad = false, vector<CMaterialData> customMats = {}, wstring imageExtension = L".png") :
			type(type) , name(name), fileName(fileName), filePath(filePath) ,  lodActivationThresHold(lodActivationThresHold),  shouldBeUsedToBakeColision(shouldBeUsedToBakeColision), shouldForceObjLoad(shouldForceObjLoad),customMats(customMats),imageExtension(imageExtension) {}
	};

	vector<GeometryStruct> lods;

	GeometryInitData(vector<GeometryStruct> lods) :lods(lods) {};

};

