#pragma once
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"
#include "../CSC8503/AnimationObject.h"

namespace NCL {
	using namespace NCL::Rendering;

	namespace CSC8503 {
		class Transform;
		using namespace Maths;

		class RenderObject
		{
		public:
			RenderObject(Transform* parentTransform, Mesh* mesh, Texture* defaultTexture, Shader* shader);
			RenderObject(Transform* parentTransform, Mesh* mesh, Texture* albedo, Texture* normal, Shader* shader);

			~RenderObject();

			void SetDefaultTexture(Texture* t) {
				defaultTexture = t;
			}

			Texture* GetDefaultTexture() const {
				return defaultTexture;
			}

			void SetNormalTexture(Texture* t) {
				normalTexture = t;
			}

			Texture* GetNormalTexture() const {
				return normalTexture;
			}
			void SetAlbedoTexture(Texture* t) {
				albedoTexture = t;
			}

			Texture* GetAlbedoTexture() const {
				return albedoTexture;
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

			AnimationObject* GetAnimationObject() const { return animationObject; }

			void SetAnimationObject(AnimationObject* animObj) { animationObject = animObj; }

			vector<vector<Matrix4>> const& GetFrameMatricesVector() const {
				return frameMatricesVector;
			}

			void SetFrameMatricesVector(vector<vector<Matrix4>> fmv) {
				frameMatricesVector = fmv;
			}

			void SetColour(const Vector4& c) {
				colour = c;
			}

			Vector4 GetColour() const {
				return colour;
			}

			
			vector<int> GetMatTextures() const { return matTextures; }
			void SetMatTextures(vector<int> mt) { matTextures = mt; }

			
		protected:
			Mesh*		mesh;

			Texture*	defaultTexture;
			Texture*	normalTexture;
			Texture*	albedoTexture;

			Shader*		shader;
			Transform*	transform;
			Vector4		colour;

			AnimationObject* animationObject = nullptr;
			int currentFrame;


			vector<vector<Matrix4>> frameMatricesVector;
			vector<int> matTextures;
		};
	}
}
