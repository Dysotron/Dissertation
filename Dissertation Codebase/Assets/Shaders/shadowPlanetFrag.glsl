#version 400 core
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;

uniform sampler2DShadow shadowTex;

uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform vec4 lightColour;
uniform float lightRadius;

out vec4 fragColor;



void main(void) 
{	
	fragColor = vec4(1,1,1,1);
}