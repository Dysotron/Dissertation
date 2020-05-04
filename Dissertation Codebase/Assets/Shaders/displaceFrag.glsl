#version 400 core
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;

in Vertex 
{
	vec2 texCoord;
	vec4 colour;
	vec3 normal;
	float polarity;
	float height;
} IN;

out vec4 fragColor;

void TextureMixing()
{
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
		}
		
		else if (IN.height < ((sandHeight - waterHeight)/2) + waterHeight)
		{
			fragColor = mix(water, sand, (IN.height - waterHeight));
		}
	
		else if(IN.height < sandHeight)
		{
			fragColor = sand;
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
		}
		

		fragColor = mix(fragColor, snow, pow(normY, snowFade));
}

void main(void) 
{	
	TextureMixing();
}