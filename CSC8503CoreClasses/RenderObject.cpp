#include "RenderObject.h"
#include "Mesh.h"

using namespace NCL::CSC8503;
using namespace NCL;

RenderObject::RenderObject(Transform* parentTransform, Mesh* mesh, Texture* tex, Shader* shader) {
	if (!tex) {
		bool a = true;
	}
	this->transform	= parentTransform;
	this->mesh		= mesh;
	this->defaultTexture	= tex;
	this->albedoTexture = nullptr;
	this->normalTexture = nullptr;
	this->shader	= shader;
	this->colour	= Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	currentFrame = 0
}

RenderObject::RenderObject(Transform* parentTransform, Mesh* mesh, Texture* albedo, Texture* normal, Shader* shader)
{

	this->transform = parentTransform;
	this->mesh = mesh;
	this->albedoTexture = albedoTexture;
	this->normalTexture = normalTexture;
	this->defaultTexture = nullptr;
	this->shader = shader;
	this->colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	currentFrame = 0;

	vector<uint64_t> matTextures = {};
}

RenderObject::~RenderObject() {

}