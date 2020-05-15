#version 400 core
layout(vertices =3) out; // num vertices in output patch
uniform vec3 cameraPos;

in Vertex 
{
	vec2 texCoord; // From Vertex Shader
	vec4 colour;
	vec3 normal;
} IN[]; // Equal to size of the draw call vertex count

out Vertex 
{
	vec2 texCoord; // To Evaluation Shader
	vec4 colour;
	vec3 normal;
} OUT[]; // Equal to the size of the layout vertex count

float GetTessLevel(float Distance0, float Distance1)
{
    float avgDist = (Distance0 + Distance1) / 2.0;

	//return 2.0;

    if (avgDist < 150) 
	{
        return 100.0;
    }
    else if (avgDist < 200) 
	{
        return 30.0;
    }
    else if (avgDist < 300)
	{
        return 15.0;
    }
	else
	{
		return 5.0;
	}
}

void main () 
{
	// Calculate the distance from the camera to the three control points
    float camDist0 = distance(cameraPos, gl_in[0].gl_Position.xyz);
    float camDist1 = distance(cameraPos, gl_in[1].gl_Position.xyz);
    float camDist2 = distance(cameraPos, gl_in[2].gl_Position.xyz);

	float level0 = GetTessLevel(camDist1, camDist2);
	float level1 = GetTessLevel(camDist0, camDist2);
	float level2 = GetTessLevel(camDist1, camDist0);

	gl_TessLevelInner[0] = (level0 + level1 + level2)/3;

	gl_TessLevelOuter[0] =  level0;
	gl_TessLevelOuter[1] =  level1;
	gl_TessLevelOuter[2] =  level2;
	
	/*gl_TessLevelInner[0] = 32;
	gl_TessLevelOuter[0] =  32;
	gl_TessLevelOuter[1] =  32;
	gl_TessLevelOuter[2] =  32;*/

	OUT[gl_InvocationID].texCoord = IN[gl_InvocationID ].texCoord;
	OUT[gl_InvocationID].colour = IN[gl_InvocationID ].colour;
	OUT[gl_InvocationID].normal = IN[gl_InvocationID ].normal;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
