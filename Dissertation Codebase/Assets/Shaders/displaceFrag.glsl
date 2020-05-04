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
	
	float normY = abs(IN.polarity);
	
	float waterHeight = 0.001;
	float sandHeight = waterHeight + 0.002;
	float mountHeight = 0.1;
	int snowFade = 5;
		
		if (IN.height < waterHeight)
		{
			fragColor = water;
			power = 50.0f;
		}
		
		else if (IN.height < ((sandHeight - waterHeight)/2) + waterHeight)
		{
			fragColor = mix(water, sand, (IN.height - waterHeight));
		}
	
		else if(IN.height < sandHeight)
		{
			fragColor = sand;

			power = 2.0f;
		}
		
		else if(IN.height < sandHeight + 0.001)
		{
			fragColor = mix(sand, grass, (IN.height - sandHeight) * 1000);
		}
		
		else if(IN.height < mountHeight)
		{
			fragColor = mix(grass, rock, IN.height *25);
		}
		
		else
		{
			fragColor = mix(rock, snow, IN.height *10); //*10
			power = 10.0f;
		}
		

		fragColor = mix(fragColor, snow, pow(normY, snowFade));

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
	fragColor.rgb += (diffuse.rgb * lightColour.rgb) *0.5;
}