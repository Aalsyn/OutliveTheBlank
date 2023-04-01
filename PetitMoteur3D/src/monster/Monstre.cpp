#include "stdafx.h"
#include "Monstre.h"
#include "MoteurWindows.h"


using namespace physx;
using namespace std;

Monstre::Monstre(
	XMFLOAT4  scale,
	XMFLOAT4  position,
	XMFLOAT4  rotationQuaternion,
	GeometryInitData& monstreBody,
	GeometryInitData& monstreBladesBack,
	GeometryInitData& monstreBladesFront,
	string materialInstanceName,
	CDispositifD3D11* pDispositif,
	PhysicalInitData& physicalInitData
) :
	GameObject(scale, position, rotationQuaternion, physicalInitData),
	body(scale, position, rotationQuaternion, monstreBody, materialInstanceName, pDispositif),
	bladesBack(scale, position, rotationQuaternion, monstreBladesBack, materialInstanceName, pDispositif),
	bladesFront(scale, position, rotationQuaternion, monstreBladesFront, materialInstanceName, pDispositif)
{
	situation = Situation::Default;
	speed = 20.0f;
	rotationBladesAngle = 0.0f;
	chasePlayer = false;
	monsterPath = Path({
//Wasteland
{SubPath({
	PathNode(0,32,0),
	PathNode(6,33,-132),
	PathNode(112,46,-227),
	PathNode(70,30,-384),
	PathNode(73,30,-462),
	})
},
{SubPath({
	PathNode(63,5,-538),
	PathNode(63,3,-667),
	PathNode(1,17,-823),
	PathNode(-123,41,-880),
	PathNode(-191,38,-823),
	}),
	SubPath({
		PathNode(47, 39, -562),
		PathNode(14,43,-647),
		PathNode(-64,39,-742),
		})
},
{SubPath({
	PathNode(-200,38,-793),
	PathNode(-284,43,-775),
	PathNode(-349,53,-756),
	PathNode(-476,29,-769),
	PathNode(-553,34,-831),
	PathNode(-732,32,-832),
	})
},
//Forest
{SubPath({
	PathNode(-814,28,-908),
	PathNode(-902,28,-1014),
	PathNode(-1033,28,-1091),
	PathNode(-1152,28,-1042),
	PathNode(-1235,28,-942),
	}),
	SubPath({
		PathNode(-818,29,-828),
		PathNode(-888,40,-808),
		PathNode(-1038,40,-790),
		PathNode(-1197,36,-787),
		})
},
{SubPath({
	PathNode(-1303,27,-811),
	PathNode(-1389,18,-753),
	PathNode(-1505,10,-692),
	PathNode(-1615,19,-790),
	PathNode(-1715,33,-934),
	PathNode(-1779,25,-1000),
	PathNode(-1893,26,-1100),
	PathNode(-2018,28,-1200),
	PathNode(-2161,28,-1231),
	PathNode(-2273,28,-1112),
	PathNode(-2305,29,-881),
	PathNode(-2418,-22,-623),
	PathNode(-2500,-30,-532),
	PathNode(-2600,-41,-529),
	PathNode(-2643,-36,-419),
	PathNode(-2622,-29,-262),
	PathNode(-2695,-28,-215),
	PathNode(-2785,-17,-290),
	PathNode(-2875,-8,-398)
	})
},
//Mountain
{SubPath({
	PathNode(-2969,11,-434),
	PathNode(-3000,22,-526),
	PathNode(-2922,23,-575),
	PathNode(-2832,28,-502),
	PathNode(-2847,31,-334),
	PathNode(-2661,26,-254),
	PathNode(-2449,3,58),
	PathNode(-2855,1,174),
	PathNode(-2983,-7,166),
	PathNode(-3066,-9,60),
	PathNode(-3072,-8,-6),
	PathNode(-3145,5,-26),
	PathNode(-3225,11,1),
	PathNode(-3270,16,-5),
	PathNode(-3426,16,-35),
	PathNode(-3484,21,-141),
	PathNode(-3455,30,-268),
	PathNode(-3525,30,-399)
	})
},
//Moon
{SubPath({
	PathNode(-138, 820, 16)
	})
}
});
}

void Monstre::updateTransforms() {
	//Body
	const PxTransform& pose = getGlobalPose();
	const PxVec3 vecSpeed = getLinearVelocity().getNormalized();
	const PxVec3 XZSpeed = PxVec3(vecSpeed.x, 0.0f, vecSpeed.z).getNormalized();
	const float theta = -acos(getDirection().dot(XZSpeed));
	const PxQuat qRotation = PxQuat(theta - PxPi/2, getUp());
	body.setTransform(PxTransform(pose.p,  qRotation * pose.q));
	//Blades
	/*
	const float bladesRotation = speed / IMAGESPARSECONDE;
	rotationBladesAngle += bladesRotation;
	const PxQuat wheelRotationQuat = PxQuat(rotationBladesAngle, getUp().cross(vecSpeed));
	bladesBack.setTransform(PxTransform(pose.p, wheelRotationQuat * qRotation * pose.q));
	bladesFront.setTransform(PxTransform(pose.p, wheelRotationQuat * qRotation * pose.q));*/
	bladesBack.setTransform(PxTransform(pose.p, qRotation * pose.q));
	bladesFront.setTransform(PxTransform(pose.p, qRotation * pose.q));
}

bool Monstre::IsCloseToPlayer() {
	const PxVec3& vehiculePos = CMoteurWindows::GetInstance().vehicule->getPosition();
	const PxVec3 diff = vehiculePos - getPosition();

	return diff.magnitude() < 80.0f;

}

tuple<int,int,int> Monstre::FindClosestNode(const PxVec3& position) {
	int minPath = 0;
	int minSubPath = 0;
	int minNode = 0;
	float minDist = 9999.0f;
	auto path = monsterPath.getPath();
	PxVec3 nodePos;
	float dist;
	for (int i = 0; i < path.size(); i++) {
		for (int j = 0; j < path[i].size(); j++) {
			for (int k = 0; k < path[i][j].getSize(); k++) {
				nodePos = path[i][j].getNode(k).Position();
				dist = (nodePos - position).magnitude();
				if (dist < minDist) {
					minPath = i;
					minSubPath = j; //MONSTER CAN CHANGE SUBPATH
					minNode = k;
					minDist = dist;
				}
			}
		}
	}
	return tuple<int,int,int>(minPath, minSubPath, minNode);
	monsterPath.SetProgressTo(minPath, minSubPath, minNode);
	monsterPath.advanceNextNode();
	chasePlayer = false;
}

void Monstre::setProgressToClosestNode() {
	auto closestNode = FindClosestNode(getPosition());
	monsterPath.SetProgressTo(get<0>(closestNode), get<1>(closestNode), get<2>(closestNode));
	monsterPath.advanceNextNode();
	chasePlayer = false;
}

PxVec3 Monstre::ChasePlayer() {
	PxVec3 dir;
	chasePlayer = true;
	const PxVec3& vehiculePos = CMoteurWindows::GetInstance().vehicule->getPosition();
	dir = vehiculePos - getPosition();
	dir.normalize();
	return dir;
}

void Monstre::StepNode(const int& subPath) {
	const int prevPath = monsterPath.getCurrentPath();
	monsterPath.advanceNextNode(subPath);
	const int newPath = monsterPath.getCurrentPath();
	if (situation == Situation::NeedToMerge) {
		situation = Situation::Idle;
	}
	int oldSize = static_cast<int>(monsterPath.getPath()[prevPath].size());
	int newSize = static_cast<int>(monsterPath.getPath()[newPath].size());
	if (newPath != prevPath && oldSize !=newSize ) {
		if (monsterPath.getPath()[newPath].size() > 1) {
			situation = Situation::NeedToSplit;
		}
		if (monsterPath.getPath()[newPath].size() == 1) {
			situation = Situation::NeedToMerge;
		}
	}
}

PxVec3 Monstre::FollowPath() {
	return monsterPath.getDirectionToCurrentNode(getPosition());
}

void Monstre::Move(const int& subPath) {
	if (situation == Situation::Idle) {
		setLinearVelocity(PxVec3(0, 0, 0));
		return;
	}
	PxVec3 dir = PxVec3(0,0,0);
	if (IsCloseToPlayer() && GameStateManager::GetInstance().getInGame() == true) {
		SoundController::GetInstance().playMonster();
		dir = ChasePlayer();
	}
	else {
		SoundController::GetInstance().stopMonster();
		if (chasePlayer) {
			setProgressToClosestNode();
		}
		if (monsterPath.hasNodeBeenReached(getPosition())) {
			StepNode(subPath);
		}
		dir = FollowPath();
	}
	setLinearVelocity(speed * dir);
}

void Monstre::onTrigger(PxActor* actor)
{
	if (static_cast<ObjectPhysicalInstance*>(actor->userData)->getIsVehicule())
	{
		SoundController::GetInstance().stopMonster();
		SoundController::GetInstance().playDeath();
		GameStateManager::GetInstance().addCall(Call::Reset);
	}	
}

void Monstre::CopyProgress(Monstre& monster) {
	monsterPath.SetProgressTo(monster.monsterPath);
	setGlobalPose(monster.getGlobalPose());
}

void Monstre::Draw() {
	updateTransforms();
	body.Draw();
	bladesBack.Draw();
	bladesFront.Draw();
}

void Monstre::ResetMonstre() {
	monsterPath.resetProgress();
	situation = Situation::Default;
	PxVec3 spawn = PxVec3(0.0f, -200.0f, 0.0f);
	setGlobalPose(PxTransform(spawn, PxQuat(0.0f, 1.0f, 0.0f,0.0f)));
}

//MONSTRES

Monstres::Monstres(
	XMFLOAT4  scale,
	XMFLOAT4  position,
	XMFLOAT4  rotationQuaternion,
	GeometryInitData& monstreBody,
	GeometryInitData& monstreBladesBack,
	GeometryInitData& monstreBladesFront,
	string materialInstanceName,
	CDispositifD3D11* pDispositif,
	PhysicalInitData& physicalInitData
) :
	Monstre(scale, position, rotationQuaternion, monstreBody, monstreBladesBack, monstreBladesFront, materialInstanceName, pDispositif, physicalInitData),
	subMonster0(Monstre(scale, position, rotationQuaternion, monstreBody, monstreBladesBack, monstreBladesFront, materialInstanceName, pDispositif, physicalInitData)),
	subMonster1(Monstre(scale, position, rotationQuaternion, monstreBody, monstreBladesBack, monstreBladesFront, materialInstanceName, pDispositif, physicalInitData))
{
	isSplit = false;
	playerCheese = false;
}


void Monstres::Split() {
	isSplit = true;
}

void Monstres::Merge() {
	isSplit = false;
	situation = Situation::Default;
	subMonster0.situation = Situation::Default;
	subMonster1.situation = Situation::Default;
}

void Monstres::Step() {
	if (playerCheese) {
		counterCheese();
		return;
	}
	IsCheesing();
	if (!isSplit) {
		subMonster0.CopyProgress(*this);
		subMonster1.CopyProgress(*this);
		Move();
		if (situation == Situation::NeedToSplit) {
			Split();
		}
	}
	else {
		subMonster0.Move(0);
		subMonster1.Move(1);
		CopyProgress(subMonster0);
		if (subMonster0.situation == Situation::Idle && subMonster1.situation == Situation::Idle) {
			Merge();
		}
	}
}

void Monstres::ResetMonstres() {
	isSplit = false;
	playerCheese = false;
	ResetMonstre();
	subMonster0.ResetMonstre();
	subMonster1.ResetMonstre();
}

void Monstres::Draw() {
	Step();
	if (!isSplit) {
		Monstre::Draw();
	}
	else {
		subMonster0.Draw();
		subMonster1.Draw();
	}
}

Monstres::~Monstres() {
	CMoteurWindows::GetInstance().monstres = nullptr;
}

float Monstres::getVitesse() {
	return speed;
}

void Monstres::setVitesse(float newV) {
	speed = newV;
	subMonster0.speed = newV;
	subMonster1.speed = newV;
}

float Monstres::getDistance(const PxVec3& position) {
	using namespace MyConverter;
	if (isSplit) {
		return min(distance(position, subMonster0.getPosition()), distance(position, subMonster1.getPosition()));
	}
	else {
		return distance(position, getPosition());
	}
}

void Monstres::counterCheese() {
	if (isSplit) {
		Merge();
	}
	setLinearVelocity(speed * ChasePlayer());
}

void Monstres::IsCheesing() {
	if (chasePlayer || subMonster0.chasePlayer || subMonster1.chasePlayer) {
		return;
	}
	if (get<0>(FindClosestNode(CMoteurWindows::GetInstance().vehicule->getPosition())) < monsterPath.getCurrentPath()) {
		playerCheese = true;
	}
}