#version 400 core
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;

uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform vec4 lightColour;
uniform float lightRadius;

in Vertex 
{
	vec2 texCoord;
	vec4 colour;
	vec3 normal;
	float polarity;
	float height;
	vec3 worldPos;
	float terrainNoise;
} IN;

out vec4 fragColor;

float TextureMixing()
{
	float power = 1.0f;

	//mix textures based on the normal value
	vec4 grass = texture ( texture0 , IN.texCoord );
	vec4 snow = texture ( texture1 , IN.texCoord );
	vec4 water = texture ( texture2 , IN.texCoord );
	vec4 sand = texture ( texture3 , IN.texCoord );
	vec4 rock = texture ( texture4 , IN.texCoord );

	vec4 base = mix(grass, sand, IN.terrainNoise);
	
	float normY = abs(IN.polarity);
	
	float waterHeight = 0.001;
	float sandHeight = waterHeight + 0.002;
	float mountHeight = 0.1;
	int snowFade = 5;

	float waterPower = 20.0f;
	float sandPower = 2.0f;
	float grassPower = 5.0f;
	float rockPower = 10.0f;
	float snowPower = 10.0f;

	float basePower = mix(grassPower, sandPower, IN.terrainNoise);
		
		if (IN.height < waterHeight)
		{
			fragColor = water;
			power = waterPower;
		}
		
		else if (IN.height < ((sandHeight - waterHeight)/2) + waterHeight)
		{
			fragColor = mix(water, sand, (IN.height - waterHeight));
		}
	
		else if(IN.height < sandHeight)
		{
			fragColor = sand;
			power = sandPower;
		}
		
		else if(IN.height < sandHeight + 0.001)
		{
			fragColor = mix(sand, base, (IN.height - sandHeight) * 1000);
			power = mix(sandPower, basePower, (IN.height - sandHeight) * 1000);
		}
		
		else if(IN.height < mountHeight)
		{
			fragColor = mix(base, rock, IN.height *20);
			power = mix(basePower, rockPower, IN.height *20);
		}
		
		else
		{
			fragColor = mix(rock, snow, IN.height *10);
			power = mix(rockPower, snowPower, IN.height *10);
		}
		

		fragColor = mix(fragColor, snow, pow(normY, snowFade));
		power = mix(power, snowPower, pow(normY, snowFade));

		return power;
}

void main(void) 
{	
	float power = TextureMixing();

	vec4 diffuse = fragColor;

	vec3 incident = normalize(lightPos - IN.worldPos);
	float lambert = max(0.0, dot(incident, IN.normal));

	float dist = length(lightPos - IN.worldPos);
	float atten = 1.0 - clamp(dist/lightRadius, 0.0, 1.0);

	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	float rFactor = max(0.0, dot(halfDir, IN.normal));
	float sFactor = pow(rFactor, power);

	vec3 colour = (diffuse.rgb * lightColour.rgb);
	colour += (lightColour.rgb * sFactor) * 0.33;
	fragColor = vec4(colour * atten * lambert, diffuse.a);
	fragColor.rgb += (diffuse.rgb * lightColour.rgb) * 0.2;
}