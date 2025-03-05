#pragma once
#include "OGLRenderer.h"
#include "OGLShader.h"
#include "OGLTexture.h"
#include "OGLMesh.h"
#include "MeshAnimation.h"
#include "MeshMaterial.h"
#include "GameWorld.h"
#include <iostream>
#include <map>
namespace NCL {
	namespace CSC8503 {
		class RenderObject;

		class GameTechRenderer : public OGLRenderer {
		public:
			GameTechRenderer(GameWorld& world);
			~GameTechRenderer();

			Mesh* LoadMesh(const std::string& name);
			void LoadMeshes(std::unordered_map<std::string, Mesh*>& meshMap, const std::vector<std::string>& details);
			Texture* LoadTexture(const std::string& name);
			Shader* LoadShader(const std::string& vertex, const std::string& fragment);
			MeshMaterial* LoadMaterial(const std::string& name);
			MeshAnimation* LoadAnimation(const std::string& name);
			void RenderUI(std::function<void()> callback);
			void SetImguiCanvasFunc(std::function<void()> func);

			GLuint LoadTextureGetID(const std::string& name);
			int FindTextureIndex(GLuint texId);
			std::vector<int> LoadMeshMaterial(Mesh& mesh, MeshMaterial& meshMaterial);


			void GenerateObjectDataUBOs();

		protected:

			enum BufferNames {
				objectsUBO,
			};

			struct TextureHandleData {
				GLuint64 handles[256] = { 0 };
			};

			struct TextureHandles {
				int albedoIndex = 0;
				int normalIndex = 0;
			};

			void NewRenderLines();
			void NewRenderText();
			void NewRenderTextures();

			void RenderFrame()	override;

			OGLShader* defaultShader;

			GameWorld& gameWorld;

			void BuildObjectList();
			void SortObjectList();
			void RenderShadowMap();
			void RenderCamera();
			void RenderSkybox();

			void LoadSkybox();



			void SetDebugStringBufferSizes(size_t newVertCount);
			void SetDebugLineBufferSizes(size_t newVertCount);

			std::function<void()> mImguiCanvasFuncToRender = nullptr;
			void SetupImgui();


			vector<const RenderObject*> activeObjects;

			OGLShader* debugShader;
			OGLShader* skyboxShader;
			OGLMesh* skyboxMesh;
			OGLMesh* debugTexMesh;
			GLuint		skyboxTex;

			//shadow mapping things
			OGLShader* shadowShader;
			GLuint		shadowTex;
			GLuint		shadowFBO;
			Matrix4     shadowMatrix;

			Vector4		lightColour;
			float		lightRadius;
			Vector3		lightPosition;

			//Debug data storage things
			vector<Vector3> debugLineData;

			vector<Vector3> debugTextPos;
			vector<Vector4> debugTextColours;
			vector<Vector2> debugTextUVs;

			GLuint lineVAO;
			GLuint lineVertVBO;
			size_t lineCount;

			GLuint textVAO;
			GLuint textVertVBO;
			GLuint textColourVBO;
			GLuint textTexVBO;
			size_t textCount;

			std::unordered_map<std::string, GLuint> mLoadedTextureList;

			vector<std::pair<GLuint, GLuint64>> mTextureIDList;
		};
	}
}

