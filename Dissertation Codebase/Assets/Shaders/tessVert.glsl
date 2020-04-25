#version 400 core

uniform mat4 modelMatrix = mat4 (1.0f);
 
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;


uniform float time = 0.0;

out Vertex 
{
	vec2 texCoord;
	vec4 colour;
	vec3 normal; 
} OUT;

void main (void) 
{
	gl_Position = vec4(position, 1.0);
	OUT.colour	= colour;
	OUT.texCoord = texCoord;
	
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	OUT.normal = normalize(normalMatrix * normalize(normal));
}