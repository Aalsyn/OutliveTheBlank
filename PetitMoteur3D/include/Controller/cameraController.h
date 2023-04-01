#include "Camera.h"

#ifndef cameraController_h
#define cameraController_h


class CameraController {
private:
	PM3D::CCamera thirdPerson; //camera0
	PM3D::CCamera firstPerson; //camera1
	PM3D::CCamera menuCamera; //camera2
	PM3D::CCamera* activeCamera;
	int activeCameraID;
	static constexpr float baseFOV = XM_PI / 4.0f;
	static constexpr float fastFOV = 0.93f * baseFOV;
	float FOVSpeedMin;
	float FOVSpeedMax;
	DirectX::XMVECTOR CameraController::getNewPosition(const XMVECTOR& pos, const XMVECTOR& dir);
	DirectX::XMVECTOR getNewDirection(const XMVECTOR& pos,const DirectX::XMVECTOR& dir);
	void FastFOV();
	void BaseFOV();
	void updateFOV();

public:
	CameraController() {
		setActiveCamera(2);
	}
	void Init(XMMATRIX* pMatView, XMMATRIX* pMatProj, XMMATRIX* pMatViewProj);
	PM3D::CCamera* getActiveCamera();
	int getActiveCameraID();
	void setActiveCamera(int cameraID);
	void updateCamera();
	void updateFOVSpeeds();
	~CameraController() = default;
};


#endif
