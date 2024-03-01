#include "MeshInstance.h"
#include "GeometryInitData.h"
#include "ObjectRenderInstance.h"
#include "ObjectPhysicalInstance.h"

#ifndef gameobject_h
#define gameobject_h

//TODO : GameObject.cpp
class GameObject : public CObjectRenderInstance, public ObjectPhysicalInstance //, CMeshInstance
{
public:
	// should only be used to test mesh loading
	GameObject(
		XMFLOAT4  scale,
		XMFLOAT4  position,
		XMFLOAT4  rotationQuaternion,
		GeometryInitData& geometryInitData,
		string materialInstanceName,
		CDispositifD3D11* pDispositif,
		PhysicalInitData& physicsInitData
	) :
		CObjectRenderInstance(geometryInitData, materialInstanceName, pDispositif),
		m_Scale(scale),
		m_Position(position),
		m_RotationQuaternion(rotationQuaternion),
		ObjectPhysicalInstance(physicsInitData)
	{
		return;
	}

	GameObject(
		XMFLOAT4  scale,
		XMFLOAT4  position,
		XMFLOAT4  rotationQuaternion,
		GeometryInitData& geometryInitData,
		string materialInstanceName,
		CDispositifD3D11* pDispositif
		) :
		CObjectRenderInstance(geometryInitData, materialInstanceName, pDispositif),
		m_Scale(scale),
		m_Position(position),
		m_RotationQuaternion(rotationQuaternion)
	{
		return;
	}

	GameObject(
		XMFLOAT4  scale,
		XMFLOAT4  position,
		XMFLOAT4  rotationQuaternion,
		PhysicalInitData& physicsInitData
	) :
		m_Scale(scale),
		m_Position(position),
		m_RotationQuaternion(rotationQuaternion),
		ObjectPhysicalInstance(physicsInitData)
	{
		return;
	}

	GameObject(
		XMFLOAT4  scale,
		XMFLOAT4  position,
		XMFLOAT4  rotationQuaternion
	) :
		m_Scale(scale),
		m_Position(position),
		m_RotationQuaternion(rotationQuaternion)
	{
		return;
	}

private:
	XMFLOAT4 m_Scale = {};
	XMFLOAT4 m_Position = {};
	XMFLOAT4 m_RotationQuaternion = {};

public:

	void Draw() override {
		int cameraDistance = static_cast<int>(computeDistance(XMLoadFloat4(&m_Position), getCameraPosition()));
		CObjectRenderInstance::Draw(cameraDistance, XMLoadFloat4(&m_Scale), XMLoadFloat4(&m_Position), XMLoadFloat4(&m_RotationQuaternion));
	};

	void BasicRender(const XMMATRIX& matViewProj) override {
		int cameraDistance = static_cast<int>(computeDistance(XMLoadFloat4(&m_Position), getCameraPosition()));
		CObjectRenderInstance::BasicRender(cameraDistance, XMLoadFloat4(&m_Scale), XMLoadFloat4(&m_Position), XMLoadFloat4(&m_RotationQuaternion), matViewProj);
	}

	void stepPhysics() override {
		// render mesh on 
		setTransform(getGlobalPose());
	}

	PxVec3 getPosition() {
		return getGlobalPose().p;
	};

	PxVec3 getDirection() {
		return getGlobalPose().q.getBasisVector0().getNormalized();
	}
	PxVec3 getUp() {
		return getGlobalPose().q.getBasisVector1().getNormalized();
	}
	PxVec3 getLeft() {
		return getGlobalPose().q.getBasisVector2().getNormalized();
	}

	PxQuat getRotationQuaternion() {
		return PxQuat(m_RotationQuaternion.x, m_RotationQuaternion.y, m_RotationQuaternion.z, m_RotationQuaternion.w);
	}

	void setPosition(PxVec3 newPos) noexcept {
		m_Position = XMFLOAT4(newPos.x, newPos.y, newPos.z,0.0f);
	};
	void setRotationQuaternion(PxQuat newRot) noexcept {
		m_RotationQuaternion = XMFLOAT4(newRot.x, newRot.y, newRot.z, newRot.w);
	}

	void setTransform(PxTransform pose) {
		setPosition(pose.p);
		setRotationQuaternion(pose.q);
	}
};

#endif