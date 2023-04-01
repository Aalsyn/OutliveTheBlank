#include "stdafx.h"
#include "cameraController.h"
#include "MoteurWindows.h"
#include "Converter.h"

using namespace DirectX;
using namespace PM3D;
using namespace MyConverter;

void CameraController::Init(XMMATRIX* pMatView, XMMATRIX* pMatProj, XMMATRIX* pMatViewProj) {
	CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();

	*pMatProj = XMMatrixPerspectiveFovLH(
		baseFOV,
		rMoteur.GetDispositif()->getRatioDAspect(),
		NEARPLANE,
		FARPLANE);

	*pMatViewProj = *pMatView * *pMatProj;

	//Initialisation de la camera 1ere personne
	firstPerson.Init(XMVectorSet(0.0f, 0.0f, -20.0f, 1.0f),
		XMVectorSet(0.0f, 0.0f, 10.0f, 1.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f),
		pMatView, pMatProj, pMatViewProj);
	firstPerson.Update();

	//Initialisation de la camera 3eme personne
	thirdPerson.Init(XMVectorSet(0.0f, 0.0f, -20.0f, 1.0f),
		XMVectorSet(0.0f, 0.0f, 10.0f, 1.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f),
		pMatView, pMatProj, pMatViewProj);
	thirdPerson.Update();

	menuCamera.Init(XMVectorSet(0.0f, 0.0f, -20.0f, 1.0f),
		XMVectorSet(0.0f, 0.0f, 10.0f, 1.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f),
		pMatView, pMatProj, pMatViewProj);
	menuCamera.Update();


	setActiveCamera(2);
}

CCamera* CameraController::getActiveCamera() {
	return activeCamera;
}

int CameraController::getActiveCameraID() {
	return activeCameraID;
}


void CameraController::setActiveCamera(int cameraID) {
	switch (cameraID) {
	case 0:
		activeCameraID = 0;
		activeCamera = &thirdPerson;
		break;
	case 1:
		activeCameraID = 1;
		activeCamera = &firstPerson;
		break;
	case 2:
		activeCameraID = 2;
		activeCamera = &menuCamera;
		break;
	default:
		activeCameraID = 0;
		activeCamera = &thirdPerson;
		break;
	}
}

void CameraController::updateCamera() {
	CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
	Vehicule* _vehicule = rMoteur.vehicule;
	updateFOV();
	const XMVECTOR vehiculePos = PxtoXM(_vehicule->getPosition());
	const XMVECTOR vehiculeDir = PxtoXM(_vehicule->getDirection());
	activeCamera->SetPosition(getNewPosition(vehiculePos, vehiculeDir));
	activeCamera->SetDirection(getNewDirection(vehiculePos,vehiculeDir));
	activeCamera->Update();
}

void CameraController::updateFOVSpeeds() {
	Vehicule* pVehicule = CMoteurWindows::GetInstance().vehicule;
	if (pVehicule) {
		FOVSpeedMin = pVehicule->getVitMax() / 2;
		FOVSpeedMax = max(pVehicule->getVitMax() * 0.8f, FOVSpeedMax);
	}
}

void CameraController::updateFOV() {
	Vehicule* pVehicule = CMoteurWindows::GetInstance().vehicule;
	if (pVehicule) {
		const float vitesse = pVehicule->getMeanSpeed();
		float newFOV;
		if (vitesse <= FOVSpeedMin) {
			newFOV = baseFOV;
		}
		else {
			newFOV = baseFOV + (fastFOV - baseFOV) * (min(vitesse, FOVSpeedMax) - FOVSpeedMin) / (FOVSpeedMax - FOVSpeedMin);
		}
		const XMMATRIX newMatProj = XMMatrixPerspectiveFovLH(
			newFOV,
			CMoteurWindows::GetInstance().GetDispositif()->getRatioDAspect(),
			NEARPLANE,
			FARPLANE);
		activeCamera->setMatProj(newMatProj);
		activeCamera->Update();
	}
}

XMVECTOR CameraController::getNewPosition(const XMVECTOR& pos, const XMVECTOR& dir) {

	switch (activeCameraID) {
	case 0:
		return pos - 20 * dir + XMVectorSet(0.0f, 8.0f, 0.0f, 0.0f);
	case 1:
		return pos;
	case 2:
		return XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	default:
		return pos - 20 * dir + XMVectorSet(0.0f, 8.0f, 0.0f, 0.0f);
	}
}

XMVECTOR CameraController::getNewDirection(const XMVECTOR& pos, const DirectX::XMVECTOR& dir) {
	XMVECTOR newDir = dir;
	switch (activeCameraID) {
	case 0:
		newDir = pos + XMVectorSet(0.0f, 3.0f, 0.0f, 0.0f) - activeCamera->getPosition(); //definitely not normalized
	case 1:
		break;
	case 2:
		newDir = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	default:
		break;
	}
	return newDir;
}



