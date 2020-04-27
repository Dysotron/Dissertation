#version 400 core
uniform sampler2D texture0;
uniform sampler2D secondTex;
uniform sampler2D thirdTex;
uniform sampler2D fourthTex;
uniform sampler2D fifthTex;

in Vertex 
{
	vec2 texCoord;
	vec4 colour;
	vec3 normal;
	float height;
} IN;

out vec4 fragColor;

void main(void) 
{	
	fragColor = texture(texture0, IN.texCoord) * IN.colour;
	//fragColor = IN.colour;
	
	/*//mix textures based on the normal value
	vec4 grass = texture ( mainTex , IN.texCoord );
	vec4 snow = texture ( secondTex , IN.texCoord );
	vec4 water = texture ( thirdTex , IN.texCoord );
	vec4 sand = texture ( fourthTex , IN.texCoord );
	vec4 rock = texture ( fifthTex , IN.texCoord );
	
	float normY = abs(IN.normal.y);
	
	float waterHeight = 0.001;
	float sandHeight = waterHeight + 0.002;
	float mountHeight = 0.1;
	//float capHeight = 0.03;
	int snowFade = 3;
	
		//fragColor = mix(grass, snow, pow(normY,snowFade));
		
		if (IN.height < waterHeight)
		{
			fragColor = water;
			//fragColor = mix(water, snow, pow(normY,snowFade));
		}
		
		else if (IN.height < ((sandHeight - waterHeight)/2) + waterHeight)
		{
			fragColor = mix(water, sand, (IN.height - waterHeight) * 1000);
		}
	
		else if(IN.height < sandHeight)
		{
			//fragColor = mix(sand, snow, pow(normY,snowFade));
			fragColor = sand;
		}
		
		else if(IN.height < sandHeight + 0.001)
		{
			fragColor = mix(sand, grass, (IN.height - sandHeight) * 1000);
		}
		
		else if(IN.height < mountHeight)
		{
			//fragColor = mix(fragColor, rock, IN.height * 25);
			fragColor = mix(grass, rock, IN.height *25); //*25
		}
		
		else
		{
			fragColor = mix(rock, snow, IN.height *10); //*10
		}
		
		fragColor = mix(fragColor, snow, pow(normY, snowFade));
		
		//fragColor = IN.colour;
		
		//fragColor = mix(grass, snow, pow(normY, snowFade));
		//fragColor = mix(grass, snow, normY);*/
	
	
}