#pragma once
#include <string>
#include <ctype.h>
#include "PxPhysicsAPI.h"
#include "PxSimulationEventCallback.h"

using namespace physx;
using namespace std;

enum TypeGeo {
	eSPHERE, // 0 RAYON
	ePLANE, // 1 RIEN
	eCAPSULE, // 2 RAYON HALFHEIGHT
	eBOX, // 3 HALFHEIGHT CHAQUE COTE 3 , peut faire PxVec3
	eTRIANGLEMESH, // 4 NOM DU MESH
	eCONVEXMESH, // 5 VEHICULE
	eDEADZONE, // ALL TRIGERBOX
	eMONSTRE, // MONSTER
};

struct PhysicalInitData
{
	wstring meshInstanceName;
	PxReal matStaticFriction;
	PxReal matDynamicFriction;
	PxReal matRestitution;
	TypeGeo typeGeo;
	PxVec3 attributesGeo;
	bool isDynamic;
	PxTransform transform;
	float density;
	float mass;
	PxVec4 attributesRota;

	PhysicalInitData(TypeGeo _typeGeo, bool _isDynamic, float x, float y, float z, 
		PxVec3 _attributesGeo = PxVec3(1,1,1), wstring _meshInstanceName = L"", float _matStaticFriction = 0, float _matDynamicFriction = 0,
		float _matRestitution = 0, float _density = 0, float _mass = 0, PxVec4 _attributesRota = PxVec4(0,0,0,1)) : 
		typeGeo(_typeGeo), isDynamic(_isDynamic), matStaticFriction(_matStaticFriction), matDynamicFriction(_matDynamicFriction),
		matRestitution(_matRestitution), density(_density),mass(_mass),meshInstanceName(_meshInstanceName), attributesGeo(_attributesGeo), attributesRota(_attributesRota)
	{
		
		transform = PxTransform(x, y, z,PxQuat(_attributesRota.x, _attributesRota.y, _attributesRota.z, _attributesRota.w));
	}
};

