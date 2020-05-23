#version 400

layout(triangles, ccw) in;

uniform sampler2D mainTex;
uniform sampler2D secondTex;
uniform float time ;
uniform float asteroidSize;

uniform vec3 collisionPos;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 shadowMatrix;

float radius = 1;
bool noise = true;

in Vertex // Sent from the TCS
{ 
	vec2 texCoord ;
	vec4 colour;
	vec3 normal;
} IN[]; // Equal to TCS layout size

out Vertex // Each TES works on a single vertex !
{ 
	vec2 texCoord ;
	vec4 colour;
	vec3 normal;
	float polarity;
	float height;
	vec3 worldPos;
	float terrainNoise;
	vec4 shadowProj;
	bool crater;
	float craterDepth;
	float craterNoise;
} OUT;

vec3 permute(vec3 x) 
{ 
	return mod(((x*34.0)+1.0)*x, 289.0); 
}

float sNoise(vec2 v)
{
	//skew the coordinates to find which simplex point is in
	const vec4 C = vec4(0.211324865405187, 0.366025403784439, -0.577350269189626, 0.024390243902439);
	vec2 i  = floor(v + dot(v, C.yy));
	vec2 x0 = v - i + dot(i, C.xx);
	
	vec2 i1; 
	
	if(x0.x > x0.y)
	{
		i1 = vec2(1.0, 0.0);
	}
	else
	{
		vec2(0.0, 1.0);
	}
	
	vec4 x12 = x0.xyxy + C.xxzz;
	x12.xy -= i1;
	i = mod(i, 289.0);
	
	//Work out the hashed gradient indices of the three simplex corners
	vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 )) + i.x + vec3(0.0, i1.x, 1.0 ));
	vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
	
	// Calculate the contribution from the three corners
	m = m*m ;
	m = m*m ;
	
	vec3 x = 2.0 * fract(p * C.www) - 1.0;
	vec3 h = abs(x) - 0.5;
	vec3 ox = floor(x + 0.5);
	vec3 a0 = x - ox;
	
	m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
	
	vec3 g;
	g.x  = a0.x  * x0.x  + h.x  * x0.y;
	g.yz = a0.yz * x12.xz + h.yz * x12.yw;
	return 130.0 * dot(m, g);
}

float GenerateNoiseValue (vec3 vector)
{
	float seed0 = 12;
	vec2 point0 = vec2(vector.x + seed0, vector.z + vector.y + seed0);
	
	float value = 0.0f;
    float amplitude = 0.2f;
	 
    // Loop of octaves
    for (int i = 0; i < 5; i++) 
	{
        value += amplitude * (sNoise(point0));
        point0 *= 5;
        amplitude *= 0.5;
    }
	
	value = pow(value, 2);
	
	return value;
}

vec3 normaliseVector3(vec3 vector)
{
	float value = GenerateNoiseValue(vector);
	OUT.height = value;

	if(noise)
	{
		radius += value;
	}
	
	
	float dx = vector.x - 0;
	float dy = vector.y - 0;
	float dz = vector.z - 0;

	float magnitude = sqrt(pow(dx,2) + pow(dy, 2) + pow(dz, 2));

	dx = dx * (radius) / magnitude;
	dy = dy * (radius) / magnitude;
	dz = dz * (radius) / magnitude;
	
	return vec3(dx, dy, dz);
}

vec3 GenerateSurfaceNormal(vec3 a, vec3 b, vec3 c)
{
	vec3 ba = b - a;
	vec3 ca = c - a;

	vec3 normal = cross(ba, ca);
	normal = normalize(normal);
	return normal;
}

void main () 
{

	vec2 p3 = gl_TessCoord.x * IN[0].texCoord;
	vec2 p4 = gl_TessCoord.y * IN[1].texCoord;
	vec2 p5 = gl_TessCoord.z * IN[2].texCoord;
	OUT.texCoord = (p3 + p4 + p5) * 20;
	
	vec3 p0 = gl_TessCoord.x * gl_in[0].gl_Position.xyz;
	vec3 p1 = gl_TessCoord.y * gl_in[1].gl_Position.xyz;
	vec3 p2 = gl_TessCoord.z * gl_in[2].gl_Position.xyz;
	
	vec3 combinedPos = p0 + p1 + p2;

	vec4 worldPos = modelMatrix * vec4(combinedPos, 1.0);

	float distance  = sqrt(pow(worldPos.x - collisionPos.x, 2) + pow(worldPos.y - collisionPos.y, 2) + pow(worldPos.z - collisionPos.z, 2));

	if(distance < asteroidSize && collisionPos != vec3(0,0,0))
	{
		if(distance < 2)
		{
			distance = 2;
		}

		OUT.crater = true;
		float crater = 1 / distance;

		OUT.craterDepth = crater;
		radius -= crater;

		float craterNoise = sNoise(vec2(combinedPos.x + combinedPos.y, combinedPos.z)) / 5;
		OUT.craterNoise = craterNoise;

		radius += craterNoise;

		noise  = false;
	}

	else
	{
		OUT.crater = false;
	}

	OUT.terrainNoise = sNoise(vec2(combinedPos.x + combinedPos.y, combinedPos.z));


	//add noise to original point
	combinedPos = normaliseVector3(combinedPos);
	

	worldPos = modelMatrix * vec4(combinedPos, 1.0);
	OUT.worldPos = worldPos.xyz;
	
	
	vec4 p6 = gl_TessCoord.x * IN[0].colour;
	vec4 p7 = gl_TessCoord.y * IN[1].colour;
	vec4 p8 = gl_TessCoord.z * IN[2].colour;
	OUT.colour = p6 + p7 + p8;
	
	
	vec3 p9 = gl_TessCoord.x * IN[0].normal;
	vec3 p10 = gl_TessCoord.y * IN[1].normal;
	vec3 p11 = gl_TessCoord.z * IN[2].normal;
	OUT.polarity = normalize(p9 + p10 + p11).y;
	OUT.normal = normalize(p9 + p10 + p11);
	
	gl_Position = projMatrix * viewMatrix * worldPos;

	OUT.shadowProj = shadowMatrix * vec4 ( combinedPos,1);

}
