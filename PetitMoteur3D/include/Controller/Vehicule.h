#include "GameObject.h"
#include "objet3d.h"

#ifndef vehicule_h
#define vehicule_h


class Vehicule : public GameObject
{
public:
	bool isOnGround = false;
	bool canShoot = true;
	bool isGrounded = true;

private:
	GameObject wheel;
	GameObject driver;
	GameObject cannon;
	float wheelRotationAngle;
	float acceleration;
	float vitesseRotation;
	float coeffFrictionTerrain;
	int64_t lastShot;
	vector<float> previousSpeeds;
	int currentIndexPreviousSpeeds;

	void TickPreviousSpeed();

public:
	float vitesseMax;
	Vehicule(
		XMFLOAT4  scale,
		XMFLOAT4  position,
		XMFLOAT4  rotationQuaternion,
		GeometryInitData& vehiculeGeometry,
		GeometryInitData& driverGeometry,
		GeometryInitData& cannonGeometry,
		string materialInstanceName,
		CDispositifD3D11* pDispositif,
		PhysicalInitData& physicalInitData
	);
	~Vehicule();

	float getAcceleration() noexcept;
	float getVitMax() noexcept;
	float getVitRot() noexcept;
	PxVec3 getXZspeed();
	float getSpeed();
	float getMeanSpeed();

	void accelerate(float magnitude = 1.0f);
	void decelerate(float magnitude = 1.0f);
	void strafeForward(float magnitude = 1.0f);
	void strafeBackward(float magnitude = 1.0f);
	void strafeLeft(float magnitude = 1.0f);
	void strafeRight(float magnitude = 1.0f);
	void turnY(float axis);

	void jump(float height);
	void shoot();
	void projectSpeedOnDirection();
	void tryShoot();

	void updateTransforms();
	void CheckTerrain();
	bool IsSliding();
	void teleport(PxVec3 position, float angleRadian);

	void onContact(PxActor*) override;
	void Draw() override;
	void BasicRender(const XMMATRIX& matViewProj) override;
};
#endif