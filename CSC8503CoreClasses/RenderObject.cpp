#include "RenderObject.h"
#include "Mesh.h"

using namespace NCL::CSC8503;
using namespace NCL;

RenderObject::RenderObject(Transform* parentTransform, Mesh* mesh, Texture* albedo, Texture* normal, Shader* shader) {

	this->transform = parentTransform;
	this->mesh = mesh;
	this->mAlbedo = albedo;
	this->mNormal = normal;
	this->shader = shader;
	this->colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	this->mAnimObject = nullptr;
	this->mCurrentFrame = 0;
	
	vector<uint64_t> mMatTextures = {};
}

RenderObject::~RenderObject() {

}