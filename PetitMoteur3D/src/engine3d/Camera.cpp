#include "stdafx.h"
#include "MoteurWindows.h"
#include "Camera.h"


PM3D::CCamera::CCamera(const XMVECTOR& position_in, const XMVECTOR& direction_in, const XMVECTOR& up_in, XMMATRIX* pMatView_in, XMMATRIX* pMatProj_in, XMMATRIX* pMatViewProj_in)
{
	Init(position_in, direction_in, up_in, pMatView_in, pMatProj_in, pMatViewProj_in);
}

void PM3D::CCamera::Init(const XMVECTOR& position_in, const XMVECTOR& direction_in, const XMVECTOR& up_in, XMMATRIX* pMatView_in, XMMATRIX* pMatProj_in, XMMATRIX* pMatViewProj_in)
{

	pMatView = pMatView_in;
	pMatProj = pMatProj_in;
	pMatViewProj = pMatViewProj_in;
	position = position_in;
	direction = direction_in;
	up = up_in;
}

void PM3D::CCamera::TurnY(float YRotDelta)
{
	//yRotation += XM_PI * turnSpeed * tempsEcoule;
	direction = XMVector3Transform(direction, XMMatrixRotationY(YRotDelta));
}

void PM3D::CCamera::TurnX(float XRotDelta)
{
	//yRotation += XM_PI * turnSpeed * tempsEcoule;
	direction = XMVector3Transform(direction, XMMatrixRotationAxis(XMVector3Cross(direction, { 0.0f,-1.0f,0.0f }), XRotDelta));
}

void PM3D::CCamera::MoveLeft()
{
	position += XMVector3Cross(direction, { 0.0f,1.0f,0.0f }) * translation;
}

void PM3D::CCamera::MoveRight()
{
	position += XMVector3Cross(direction, { 0.0f,-1.0f,0.0f }) *translation;
}

void PM3D::CCamera::MoveForward()
{
	position += direction * translation;
}

void PM3D::CCamera::MoveBackward()
{
	position -= direction * translation;
}

void PM3D::CCamera::HandleKeyboardInput()
{
	CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
	CDIManipulateur& rGS = rMoteur.GetGestionnaireDeSaisie();

	if (rGS.ToucheAppuyee(DIK_LEFT))
	{
		TurnY(-turnAngle);
	}
	if (rGS.ToucheAppuyee(DIK_RIGHT))
	{
		TurnY(turnAngle);
	}
	if (rGS.ToucheAppuyee(DIK_UP))
	{
		TurnX(-turnAngle);
	}
	if (rGS.ToucheAppuyee(DIK_DOWN))
	{
		TurnX(turnAngle);
	}
	if (rGS.ToucheAppuyee(DIK_W))
	{
		MoveForward();
	}
	if (rGS.ToucheAppuyee(DIK_S))
	{
		MoveBackward();
	}
	if (rGS.ToucheAppuyee(DIK_A))
	{
		MoveLeft();
	}
	if (rGS.ToucheAppuyee(DIK_D))
	{
		MoveRight();
	}
}

void PM3D::CCamera::HandleMouseInput()
{
	CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
	CDIManipulateur& rGS = rMoteur.GetGestionnaireDeSaisie();
	auto& delta = rGS.EtatSouris();
	if (delta.rgbButtons[0] & 0x80)
	{
		// delta Y use with turn X because axis impact is inverted
		TurnX(delta.lY * turnAngle * mouseSensitivity);
		TurnY(delta.lX * turnAngle * mouseSensitivity);
	}
}

void PM3D::CCamera::setHeightToTerrain()
{
	CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
	auto& listeScene = rMoteur.getScene();
	for (auto& object : listeScene) {
		if (dynamic_cast<CTerrain*>(object.get()) != nullptr) {
			CTerrain& mesh = *dynamic_cast<CTerrain*>(object.get());
			float ycollision = mesh.getTerrainY(XMVectorGetX(position), XMVectorGetZ(position));
			if (ycollision >= -999.0) {
				position = XMVectorSetY(position, ycollision + 1.0f);
			}
			break;
		}
	}
}

void PM3D::CCamera::Update(float _tempsEcoule)
{
	tempsEcoule = _tempsEcoule;
	turnAngle = XM_PI * turnSpeed * tempsEcoule;
	translation = moveSpeed * tempsEcoule;

	HandleKeyboardInput();
	HandleMouseInput();
	if (!isFreeCam) setHeightToTerrain();

	// Matrice de la vision
	*pMatView = XMMatrixLookAtLH(position,
		(position + direction),
		up);

	// Recalculer matViewProj
	*pMatViewProj = (*pMatView) * (*pMatProj);
}

void PM3D::CCamera::Update()
{
	// Matrice de la vision
	*pMatView = XMMatrixLookAtLH(position,
		(position + direction),
		up);

	// Recalculer matViewProj
	*pMatViewProj = (*pMatView) * (*pMatProj);
}
