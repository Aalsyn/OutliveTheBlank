#pragma once

using namespace DirectX;
namespace PM3D {

	class CCamera
	{
		XMVECTOR initialPosition{};
		XMVECTOR initialDirection{};
		XMVECTOR initialUp{};
		XMVECTOR position{};
		XMVECTOR direction{};
		XMVECTOR up{};
		float xRotation{};
		float yRotation{};
		float zRotation{};
		float translation{};
		XMMATRIX* pMatView = nullptr;
		XMMATRIX* pMatProj = nullptr;
		XMMATRIX* pMatViewProj = nullptr;

		bool isFreeCam = true;

		float tempsEcoule = 0;
		const float turnSpeed = .5f;
		const float moveSpeed = .8f;
		float turnAngle = XM_PI * 2.0f ;
		float angleDirectionCamera;
		float mouseSensitivity = 0.1f;
		void HandleKeyboardInput();
		void HandleMouseInput();
		//input functions

		void MoveForward();
		void MoveBackward();
		void MoveLeft();
		void MoveRight();
		void MoveUp();
		void MoveDown();

		void reInit();
	public:

		void TurnY(float YRotDelta);
		void TurnX(float XRotDelta);

	public:
		CCamera() = default;
		CCamera(const XMVECTOR& position_in,
			const XMVECTOR& direction_in,
			const XMVECTOR& up_in,
			XMMATRIX* pMatView_in,
			XMMATRIX* pMatProj_in,
			XMMATRIX* pMatViewProj_in);

		void Init(const XMVECTOR& position_in,
			const XMVECTOR& direction_in,
			const XMVECTOR& up_in,
			XMMATRIX* pMatView_in,
			XMMATRIX* pMatProj_in,
			XMMATRIX* pMatViewProj_in);

		void Update(float _tempsEcoule);
		void Update();

		void setHeightToTerrain();

		//getters and setters
		void SetPosition(const XMVECTOR& position_in) { position = position_in; };
		void SetDirection(const XMVECTOR& direction_in) { direction = direction_in; }
		void SetUp(const XMVECTOR& up_in) { up = up_in; }
		const XMVECTOR& getPosition() const { return position; }
		const XMVECTOR& getDirection() const { return direction; }

		const double& getTurnAngle() { return turnAngle; }

		bool getCameraMode() const { return isFreeCam; }
		void switchCameraMode() { isFreeCam = !isFreeCam; }
		void setMatProj(const XMMATRIX& matProj) { *pMatProj = matProj; }
};

}
