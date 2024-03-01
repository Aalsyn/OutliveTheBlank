#pragma once

using namespace DirectX;

enum LightType {
	DIRECTIONAL,
	POINTLIGHT,
	SPOT //spot not supported for now
};

class Light
{
//private:
	bool isVPMatrixComputed = false;
	XMMATRIX viewProjMatrix;
public:
	XMMATRIX viewMatrix;
	XMMATRIX projMatrix;

	LightType lightType;
	XMVECTOR ambiant;
	XMVECTOR diffuse;
	XMVECTOR specular;
	XMVECTOR position; // used for point light only
	XMVECTOR direction; // used for directionnal light only
	XMVECTOR attenuationParam;
	//float attenuation; //1/Range (because the gpu handles multiplications better than division)
	bool isSun = false;
	//float attenuation; //1/Range (because the gpu handles multiplications better than division)
	int isActive = 1;

	std::string name;


	/*
	* default point light with arbitary values
	*/
	Light()
		: lightType(LightType::POINTLIGHT)
	{
		ambiant = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
		diffuse = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		specular = XMVectorSet(0.6f, 0.6f, 0.6f, 1.0f);
		position = XMVectorSet(-10.0f, 10.0f, -15.0f, 1.0f);
		direction = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		//attenuation = 1.f / 200.f;
		name = "testLight";
		attenuationParam = XMVectorSet(100.0f,0.0f,200.0f,1.0f);
		isSun = false;
	}

	/*
	* Default light wit arbitrary values
	if it's a point light the default ctor is called
	if it's a directionnal light it's directed along the x axis
	*/
	Light(LightType type)
		: Light()
	{
		lightType = type;
		if (lightType == DIRECTIONAL) {
			position = XMVectorSet(-100000.0f, -100000.0f, -100000.0f, 1.0f); //no position, should not be used but arbitrary far position is set just in case
			direction = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f); //directed along x axis
			isSun = false;
		}
	}

	Light(std::string _name, XMVECTOR ambiant, XMVECTOR diffuse, XMVECTOR specular,
		LightType type = LightType::POINTLIGHT,
		XMVECTOR position = XMVectorSet(-10.0f, 10.0f, -15.0f, 1.0f),
		XMVECTOR attenuationParam = XMVectorSet(100.0f, 0.0f, 200.0f, 1.0f),
		XMVECTOR direction = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
		bool _isSun = false)
		:name(_name), ambiant(ambiant), diffuse(diffuse), specular(specular), attenuationParam(attenuationParam)
		, lightType(type), position(position), direction(direction), isSun(_isSun)
	{
		if (lightType == DIRECTIONAL) {
			//position = XMVectorSet(-100000.0f, -100000.0f, -100000.0f, 1.0f); //no position, should not be used but arbitrary far position is set just in case
			attenuationParam = XMVectorSet(100.0f, 0.0f, 200000.0f, 0.0f);
		}
		else if(lightType == POINTLIGHT) {
			direction = XMVectorSetY(position, -10000.0f);

		}
	}


	void computeViewMatrix() noexcept {
		viewMatrix = XMMatrixLookToLH(position,
			direction,
				XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));
		isVPMatrixComputed = false;
	}

	//I do not know which FOV to choose for point light
	void computeProjMatrix(float fov = XM_PI / 4, float aspectRatio = 1.f, float nearPlane = 0.1f, float farPlane  = 2000000.f, float width = 1024.f, float height = 1024.f) noexcept{
		if (lightType == POINTLIGHT) {
			projMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);
		}
		else {
			projMatrix = XMMatrixOrthographicLH(width, height, nearPlane, farPlane);
		}
		isVPMatrixComputed = false;
	}

	const XMMATRIX& getViewProjMatrix() {
		if (!isVPMatrixComputed) {
			viewProjMatrix = viewMatrix * projMatrix;
			isVPMatrixComputed = true;
		}
		return viewProjMatrix;
	}

};
