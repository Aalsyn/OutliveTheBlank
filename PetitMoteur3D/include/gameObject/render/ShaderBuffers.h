#pragma once
#include <DirectXMath.h>
#include <Texture.h>
#include <unordered_map>
#include <string>
using namespace DirectX;


//WARNING : Always make sure the struct bytesize if a multiple of 16 (add padding if necessary)

constexpr const size_t LIGHT_NUMBER = 2; //this constant should be replicated in the shaders using multiple lights
struct LightParams {
	XMVECTOR lAmbiant[LIGHT_NUMBER];
	XMVECTOR lDiffuse[LIGHT_NUMBER];
	XMVECTOR lSpecular[LIGHT_NUMBER];
	XMVECTOR lPosition[LIGHT_NUMBER]; // used for point light only
	XMVECTOR lDirection[LIGHT_NUMBER]; // used for directionnal light only
	XMMATRIX lMatWVP[LIGHT_NUMBER];
	XMVECTOR lAttenuationParam[LIGHT_NUMBER];
	XMVECTOR lIsActive[LIGHT_NUMBER];
	XMVECTOR isPoint[LIGHT_NUMBER]; //if false, the light is directionnal (temporary, should be changed if spotlight is added)
	//XMFLOAT2 padding[LIGHT_NUMBER];
};

struct MaterialParams {
	XMVECTOR mAmbiant;
	XMVECTOR mDiffuse;
	XMVECTOR mSpecular;
	XMVECTOR uvScaling;
	XMVECTOR specAttenuation;
	XMVECTOR texIsActive;
};

constexpr const size_t MAT_NUMBER = 4; //this constant should be replicated in the shaders using multiple materials
struct MaterialTabParams {
	XMVECTOR mAmbiant[MAT_NUMBER];
	XMVECTOR mDiffuse[MAT_NUMBER];
	XMVECTOR mSpecular[MAT_NUMBER];
	XMVECTOR uvScaling[MAT_NUMBER];
	XMVECTOR specAttenuation[MAT_NUMBER];
	XMVECTOR texIsActive[MAT_NUMBER];
};

struct CameraParams {
	XMMATRIX matWorldViewProj;	// la matrice totale 
	XMMATRIX matWorld;			// matrice de transformation dans le monde 
	XMVECTOR cPosition; 			// la position de la caméra
};

struct PovParams {
	XMMATRIX matWorldViewProj;	// la matrice totale 
};

enum ShaderVariables {
	CAMERA_BUFFER,
	MATERIAL_BUFFER,
	MATERIAL_TAB_BUFFER,
	LIGHT_BUFFER,
	BASE_TEXTURE,
	NORMAL_MAP_TEXTURE,
	SHADOW_MAP_TEXTURE,
	BASE_SAMPLE_STATE,

	//for depthbuffer
	POV_BUFFER
};

const std::unordered_map<ShaderVariables, std::string> MeshShaderVariables = {
	{CAMERA_BUFFER, "CameraParams"},
	{MATERIAL_BUFFER, "MaterialParams"},
	{LIGHT_BUFFER, "LightParams"},
	{BASE_TEXTURE, "textureEntree"},
	{SHADOW_MAP_TEXTURE, "shadowTexture"},
	{BASE_SAMPLE_STATE, "SampleState"},
	{MATERIAL_TAB_BUFFER, "MaterialTabParams"}
};

const std::unordered_map<ShaderVariables, std::string> DepthShaderVariables = {
	{POV_BUFFER, "PovParams"}
};


