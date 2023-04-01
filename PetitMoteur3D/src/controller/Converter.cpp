#include "stdafx.h"
#include "Converter.h"

using namespace physx;
using namespace DirectX;

namespace MyConverter {
	PxVec3 XMtoPx(XMVECTOR xm) {
		return PxVec3(xm.vector4_f32[0], xm.vector4_f32[1], xm.vector4_f32[2]);
	}
	XMVECTOR PxtoXM(PxVec3 px) noexcept {
		return XMVectorSet(px.x, px.y, px.z, 0.0f);
	}

	const float distance(const physx::PxVec3& pos1, const physx::PxVec3& pos2) noexcept {
		return static_cast<float>(sqrt(pow(pos1.x - pos2.x, 2) + pow(pos1.y - pos2.y, 2) + pow(pos1.z - pos2.z, 2)));
	}
}