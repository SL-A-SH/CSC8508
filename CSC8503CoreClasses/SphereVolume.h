#pragma once
#include "CollisionVolume.h"

namespace NCL {
	class SphereVolume : public CollisionVolume
	{
	public:
		SphereVolume(float sphereRadius) {
			type = VolumeType::Sphere;
			radius = sphereRadius;
		}
		~SphereVolume() {}

		float GetRadius() const {
			return radius;
		}
	protected:
		float	radius;
	};
}

