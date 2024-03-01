#include <stdafx.h>
#include "Vehicule.h"
#include "MoteurWindows.h"

using namespace physx;

Vehicule::Vehicule(
	XMFLOAT4  scale,
	XMFLOAT4  position,
	XMFLOAT4  rotationQuaternion,
	GeometryInitData& vehiculeGeometry,
	GeometryInitData& driverGeometry,
	GeometryInitData& cannonGeometry,
	string materialInstanceName,
	CDispositifD3D11* pDispositif,
	PhysicalInitData& physicalInitData
) :
	GameObject(scale, position, rotationQuaternion, physicalInitData),
	wheel(scale, position, rotationQuaternion, vehiculeGeometry, materialInstanceName, pDispositif),
	driver(scale, position, rotationQuaternion, driverGeometry, materialInstanceName, pDispositif),
	cannon(scale, position, rotationQuaternion, cannonGeometry, materialInstanceName, pDispositif)
{
	acceleration = 70.0f;
	vitesseMax = 65.0f;
	vitesseRotation = 4.0f;
	wheelRotationAngle = 0;
	coeffFrictionTerrain = 2.0f;
	lastShot = 0;
	setLinearDamping(0.0f);
	setAngularDamping(4.0f);
	previousSpeeds = { 0,0,0,0,0,0,0,0,0,0 };
	currentIndexPreviousSpeeds = 0;
}

Vehicule::~Vehicule()
{
	CMoteurWindows::GetInstance().vehicule = nullptr;
}

float Vehicule::getAcceleration() noexcept {
	return acceleration;
}

float Vehicule::getVitMax() noexcept {
	return vitesseMax;
}

float Vehicule::getVitRot() noexcept {
	return vitesseRotation;
}

float Vehicule::getSpeed() {
	const PxVec3 speed = getXZspeed();
	const float speedDir = speed.dot(getDirection()) > 0 ? 1.0f : -1.0f;
	return speedDir * speed.magnitude();
}

//Projects X and Z components of the speed on direction. 
void Vehicule::projectSpeedOnDirection() {
	const PxVec3 vitXZ = getLinearVelocity().dot(getDirection()) * getDirection();
	setLinearVelocity(PxVec3(vitXZ.x, getLinearVelocity().y, vitXZ.z));
}

//Return a vector with X and Z components of the speed
PxVec3 Vehicule::getXZspeed() {
	return PxVec3(getLinearVelocity().x, 0.0f, getLinearVelocity().z);
}

void Vehicule::accelerate(float magnitude) {
	const float normeVitesseXZ = getXZspeed().magnitude();
	if (normeVitesseXZ <= vitesseMax)
		addForce(getDirection() * acceleration * magnitude * getMass());
}

void Vehicule::decelerate(float magnitude) {
	accelerate(-magnitude / 1.5f);
}

void Vehicule::strafeForward(float magnitude) {
	accelerate(magnitude/3);
}
void  Vehicule::strafeBackward(float magnitude) {
	decelerate(magnitude/3);
}
void  Vehicule::strafeLeft(float magnitude) {
	addForce(getLeft() * acceleration/3 * magnitude * getMass());
}
void  Vehicule::strafeRight(float magnitude) {
	strafeLeft(-magnitude);
}

void Vehicule::turnY(float axis) {
	addTorque({ 0,vitesseRotation *axis,0 });
}

void Vehicule::jump(float height) {
	setLinearVelocity(getLinearVelocity() + PxVec3(0, height, 0));
}

void Vehicule::updateTransforms() {
	TickPreviousSpeed();
	CheckTerrain();
	//Calculate the rotation of the wheel
	const float forwardSpeed = getXZspeed().dot(getDirection());
	const int speedDir = forwardSpeed > 0 ? -1 : 1;
	const float wheelRotation = speedDir * min(fabs(forwardSpeed), 30.0f) / IMAGESPARSECONDE;
	wheelRotationAngle += wheelRotation;
	const PxQuat wheelRotationQuat = PxQuat(wheelRotationAngle, getLeft());

	//Calculate the tilt of the wheel
	const float turnSpeed = getAngularVelocity().dot(getUp());
	const int turnDir = turnSpeed > 0 ? 1 : -1;
	const float tilt = speedDir * min(fabs(forwardSpeed), 30.0f) / 20.0f * turnDir * min(fabs(turnSpeed), 1.3f) / 5.0f;
	const PxQuat tiltQuat = PxQuat(tilt, getDirection());
	
	//Sets the transform of all the objects of the vehicle
	const PxTransform centerPose = getGlobalPose();
	setTransform(centerPose);
	wheel.setTransform(PxTransform(centerPose.p,  tiltQuat * wheelRotationQuat * centerPose.q));
	driver.setTransform(PxTransform(centerPose.p, tiltQuat * centerPose.q));
	cannon.setTransform(PxTransform(centerPose.p, tiltQuat * centerPose.q ));
}

void Vehicule::TickPreviousSpeed() {
	previousSpeeds[currentIndexPreviousSpeeds] = getSpeed();
	currentIndexPreviousSpeeds++;
	if (currentIndexPreviousSpeeds >= previousSpeeds.size()) {
		currentIndexPreviousSpeeds = 0;
	}
}

float Vehicule::getMeanSpeed() {
	float sum = 0.0f;
	for (const float& speed : previousSpeeds) {
		sum += speed;
	}
	return sum / previousSpeeds.size();
}

void Vehicule::onContact(PxActor*)
{
	/*isOnGround = true;
	projectSpeedOnDirection();*/
}

bool Vehicule::IsSliding()
{
	return coeffFrictionTerrain <= 0.5;
}

void Vehicule::CheckTerrain()
{
	const PxVec3 origin = getGlobalPose().p;
	const PxVec3 raycastDir = -getUp();
	PxReal maxDist = 3;
	PxRaycastBuffer hit;
	PxQueryFilterData filterData(PxQueryFlag::eSTATIC);
	bool status = CMoteurWindows::GetInstance().getPxScene()->raycast(origin, raycastDir, maxDist, hit, PxHitFlag::eDEFAULT, filterData);
	if (!status)
	{
		isOnGround = status;
	}
	else
	{
		const PxMaterial* material = static_cast<ObjectPhysicalInstance*>(hit.block.actor->userData)->getMaterial();
		coeffFrictionTerrain = material->getDynamicFriction();
	}
	maxDist = 2.2f;
	status = CMoteurWindows::GetInstance().getPxScene()->raycast(origin, raycastDir, maxDist, hit, PxHitFlag::eDEFAULT, filterData);
	if (status)
	{
		isOnGround = status;
	}
	else
	{
		isGrounded = false;
	}
}

//Shoots with the cannon, only if it's ready
void Vehicule::tryShoot() {
	Horloge horloge = CMoteurWindows::GetInstance().GetHorloge();
	const int64_t newShot = horloge.GetTimeCount();
	if (horloge.GetTimeBetweenCounts(lastShot,newShot) >= 3) {
		lastShot = newShot;
		shoot();
	}
}

void Vehicule::shoot() {
	//TODO : lancer un projectile
	addForce(-getDirection() * 2500.0f * getMass());
	//TODO : animer le cannon
}

void Vehicule::teleport(PxVec3 position, float angleRadian) {
	setGlobalPose(PxTransform(position, PxQuat(angleRadian, getUp())));
	setAngularVelocity(PxVec3(0));
	setLinearVelocity(PxVec3(0));
}

void Vehicule::Draw() {
	updateTransforms();

	//Doesn't render the vehicle in 1st person camera
	if (CMoteurWindows::GetInstance().cameraController.getActiveCameraID() != 1) {
		wheel.Draw();
		driver.Draw();
		cannon.Draw();
	}
}

void Vehicule::BasicRender(const XMMATRIX& matViewProj)
{
	updateTransforms();

	//Doesn't render the vehicle in 1st person camera
	if (CMoteurWindows::GetInstance().cameraController.getActiveCameraID() != 1) {
		wheel.BasicRender(matViewProj);
		driver.BasicRender(matViewProj);
		cannon.BasicRender(matViewProj);
	}
}
