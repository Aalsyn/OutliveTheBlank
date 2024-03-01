#ifndef converter_h
#define converter_h

namespace MyConverter {
	physx::PxVec3 XMtoPx(DirectX::XMVECTOR xm);
	DirectX::XMVECTOR PxtoXM(physx::PxVec3 px) noexcept;
	const float distance(const physx::PxVec3& pos1, const physx::PxVec3& pos2) noexcept;
}

#endif