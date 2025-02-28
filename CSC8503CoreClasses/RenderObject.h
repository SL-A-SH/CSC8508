#pragma once
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"
#include "AnimatedObject.h"

namespace NCL {
	using namespace NCL::Rendering;

	namespace CSC8503 {
		class Transform;
		using namespace Maths;

		class RenderObject
		{
		public:
			RenderObject(Transform* parentTransform, Mesh* mesh, Texture* albedo, Texture* normal, Shader* shader);
			~RenderObject();

			void SetAlbedo(Texture* t) {
				mAlbedo = t;
			}

			void SetNormal(Texture* t) {
				mNormal = t;
			}

			Texture* GetAlbedo() const {
				return mAlbedo;
			}

			Texture* GetNormal() const {
				return mNormal;
			}

			Mesh*	GetMesh() const {
				return mesh;
			}

			Transform*		GetTransform() const {
				return transform;
			}

			Shader*		GetShader() const {
				return shader;
			}

			void SetColour(const Vector4& c) {
				colour = c;
			}

			Vector4 GetColour() const {
				return colour;
			}

			void SetMatTextures(vector<int> matTextures) {
				mMatTextures = matTextures;
			}

			vector<int> GetMatTextures() const {
				return mMatTextures;
			}

			void SetAnimatedObject(AnimatedObject* animObj) {
				mAnimObject = animObj;
			}

			AnimatedObject* GetAnimatedObject() { return mAnimObject; }

			void SetCurrentFrame(int frame) {
				mCurrentFrame = frame;
			}

			int GetCurrentFrame() { return mCurrentFrame; }

		protected:
			Mesh*		mesh;
			Texture* mNormal = nullptr;
			Texture* mAlbedo = nullptr;
			Shader*		shader;
			Transform*	transform;
			Vector4		colour;
			
			vector<int> mMatTextures;

			AnimatedObject* mAnimObject = nullptr;
			int mCurrentFrame;

		};
	}
}
