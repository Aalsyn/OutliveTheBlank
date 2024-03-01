#include "GameObject.h"
//#include "objet3d.h"
#include "NodePath.h"

#ifndef monstre_h
#define monstre_h

enum class Situation {
	Default,
	NeedToSplit,
	NeedToMerge,
	Idle,
};

class Monstre : public GameObject
{
private:
	GameObject body;
	GameObject bladesBack;
	GameObject bladesFront;
	float rotationBladesAngle;
public:
	Monstre(
		XMFLOAT4  scale,
		XMFLOAT4  position,
		XMFLOAT4  rotationQuaternion,
		GeometryInitData& monstreBody,
		GeometryInitData& monstreBladesBack,
		GeometryInitData& monstreBladesFront,
		string materialInstanceName,
		CDispositifD3D11* pDispositif,
		PhysicalInitData& physicalInitData);
	~Monstre() = default;

	Situation situation;
	bool isSplit;
	Path monsterPath;
	bool chasePlayer;
	void Move(const int& subPath=0);
	void updateTransforms();
	bool IsCloseToPlayer();
	physx::PxVec3 ChasePlayer();
	physx::PxVec3 FollowPath();
	tuple<int,int,int> FindClosestNode(const PxVec3& position);
	void setProgressToClosestNode();
	void StepNode(const int& subPath = 0);
	void CopyProgress(Monstre& monster);
	void onTrigger(PxActor*) override;
	void Draw() override;
	float speed;
	void ResetMonstre();
};

class Monstres : public Monstre {
private:
	Monstre subMonster0;
	Monstre subMonster1;
	bool isSplit;
	bool playerCheese;
	void Split();
	void Merge();
	void counterCheese();
	void IsCheesing();

public:
	Monstres(XMFLOAT4  scale,
		XMFLOAT4  position,
		XMFLOAT4  rotationQuaternion,
		GeometryInitData& monstreBody,
		GeometryInitData& monstreBladesBack,
		GeometryInitData& monstreBladesFront,
		string materialInstanceName,
		CDispositifD3D11* pDispositif,
		PhysicalInitData& physicalInitData);

	~Monstres();
	void ResetMonstres();
	void Step();
	void Draw() override;

	float getVitesse();
	float getDistance(const PxVec3& position);
	void setVitesse(float newV);
};

#endif