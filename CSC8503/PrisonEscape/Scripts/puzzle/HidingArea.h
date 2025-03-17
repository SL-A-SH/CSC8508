#pragma once
#include "GameObject.h"
#include "Vector.h"

namespace NCL {
    namespace CSC8503 {
        class HidingArea : public GameObject {
        public:
            HidingArea(const Vector3& position, const Vector3& size);
            ~HidingArea();

            void Update(float dt) ;

            bool IsPlayerInside(const Vector3& playerPosition) const;

        private:
            Vector3 hidingAreaPosition;
            Vector3 hidingAreaSize;
        };
    }
}