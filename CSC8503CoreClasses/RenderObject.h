#pragma once
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"

namespace NCL {
	using namespace NCL::Rendering;

	namespace CSC8503 {
		class Transform;
		using namespace Maths;

		class RenderObject
		{
		public:
			RenderObject(Transform* parentTransform, Mesh* mesh, Texture* tex, Shader* shader);
			~RenderObject();

			void SetDefaultTexture(Texture* t) {
				texture = t;
			}

			Texture* GetDefaultTexture() const {
				return texture;
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
			void SetVisible(bool visible) {
				this->visible = visible;
			}
			bool IsVisible() const {
				return visible;
			}

			void SetMaterialTextures(std::vector<int> MaterialTextureList) {
				mMatTextures = MaterialTextureList;
			}
			std::vector<int> GetMaterialTextures() { return mMatTextures; }

		protected:
			Mesh*		mesh;
			Texture*	texture;
			Shader*		shader;
			Transform*	transform;
			Vector4		colour;
			bool visible;

			std::vector<int> mMatTextures;
		};
	}
}
