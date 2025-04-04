#version 400

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 colour;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 normal;
layout (location = 5) in vec4 jointWeights;
layout (location = 6) in ivec4 jointIndices;

uniform mat4 	joints[110];

//uniform mat4 	otherJoints[128];

uniform float frameLerp;

out Vertex {
    vec4 colour;     
    vec2 texCoord;    
    vec4 shadowProj;  
    vec3 normal;  
    vec3 worldPos;
} OUT;

void main(void) {
	vec4 localNormal = vec4(normal, 1.0f);
	vec4 localPos 	= vec4(position, 1.0f);
	vec4 skelPos 	= vec4(0,0,0,0);
	vec4 skelNormal 	= vec4(0,0,0,0);

	for(int i = 0; i < 4; ++i) {
		int   jointIndex 	= jointIndices[i];
		float jointWeight 	= jointWeights[i];

		skelPos += joints[jointIndex] * localPos * jointWeight;
		skelNormal += joints[jointIndex] * localNormal * jointWeight;
	}



	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
		
	gl_Position = mvp * vec4(skelPos.xyz, 1.0);


	OUT.normal = mat3(modelMatrix) * normalize(skelNormal.xyz);
	vec4 worldPos = modelMatrix * vec4 (skelPos.xyz, 1.0);
	OUT.shadowProj = textureMatrix * worldPos;
	OUT.texCoord = texCoord;
	OUT.colour = vec4(1.0);

}