//////////////////////////////////////////////////////////////
// File: Lighting.fx
// Author: Duff
// Date Created: 15/02/2007
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
// Variables
//////////////////////////////////////////////////////////////
//Light
float4		lightColor;		//All types
float3		lightDirection;		//Only directional light
float4		lightPos;		//Only omni light
float		lightFalloff;		//Only omni light
int		lightCurrent;
//Special FX
bool		useBump;
bool		useSpecular;
bool		useTransparent;
bool		useEnvironment;
//Camera
float3		cameraPos;
//Ambient color
float4		ambientCol;
//Material
float4		ambientMat;
float4		diffuseMat;
float		shininess;
float		alpha;
//Fog
float4		fogColor;
float		fogStart;
float		fogRange;
bool		fogEnable;
//Texture blend computation
float		textureBlendFactor;


//////////////////////////////////////////////////////////////
// Structures
//////////////////////////////////////////////////////////////
//Application to vertex shader
struct A2V
{ 
	float4 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 Tex0 : TEXCOORD0;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
};




//////////////////////////////////////////////////////////////
// Per-Pixel Point Light
//////////////////////////////////////////////////////////////
struct V2P_Point_PerPixel
{
	float4 Pos : POSITION;
	float4 WorldPos : TEXCOORD0;
	float3 ViewNormal : TEXCOORD1;
	float2 TexCoord0 : TEXCOORD2;
	float2 TexCoord1 : TEXCOORD3;	
	float3 LightDir : TEXCOORD4;
	float3 ViewDir : TEXCOORD5;
	float  fogLerpParam : TEXCOORD6;
};
void VS_Point_PerPixel(in A2V IN, out V2P_Point_PerPixel OUT)
{
	//Transform the position from view space to homogeneous projection space
	OUT.Pos = mul( IN.Pos, matWorldViewProj );

	//Compute modelWorld space position
	OUT.WorldPos = mul( IN.Pos, matWorld );

	//Compute light direction
	OUT.LightDir = lightPos - OUT.WorldPos;

	//Compute camera to light direction
	OUT.ViewDir = cameraPos - OUT.WorldPos;

	// Compute modelWorld space normal
	OUT.ViewNormal = normalize( mul( IN.Normal, (float3x3)matWorld) );

	//Copy the texture coordinate as is
	OUT.TexCoord0 = IN.Tex0;
	OUT.TexCoord1 = IN.Tex0;

	//Calculate Binormal and set Tangent Binormal and Normal matrix
	if (useBump)
	{
		float3x3 TBNMatrix = float3x3(IN.Binormal, IN.Tangent , IN.Normal); 
		OUT.LightDir = mul(TBNMatrix, normalize(OUT.LightDir));
	}

	//Fog
	if (fogEnable)
	{
		//Compute fog color only for the first light
		if (lightCurrent == 1)
		{
			float dist = distance(mul( IN.Pos, matWorld ), cameraPos);
			OUT.fogLerpParam = saturate((dist - fogStart) / fogRange);
		}
	}
}
float4 PS_Point_PerPixel( in V2P_Point_PerPixel IN) : COLOR0
{
	//Pixel to light vector
	float LenSq = dot( IN.LightDir, IN.LightDir );
	IN.LightDir = normalize( IN.LightDir );

	//Find the light's attenuation
	float Attn = min(( lightFalloff * lightFalloff ) / LenSq, 1.0f);

	//Normalize the View direction
	IN.ViewDir = normalize( IN.ViewDir);

	//Uncompress the normal map
	if (useBump)
	{
		IN.ViewNormal = 2.0f * tex2D(textureSamplerNormals, IN.TexCoord0) - 1.0f;
	}

	//Compute lighting amount
	float4 I = saturate( dot( normalize( IN.ViewNormal ), IN.LightDir ) ) * Attn;              
	
	//Compute specular
	float4 Specular = 0.0f;
	if (useSpecular)
	{
		float3 Reflect = normalize(2 * I * IN.ViewNormal -  IN.LightDir);
		Specular = pow(max(dot(Reflect, IN.ViewDir),0), shininess);
	}

	//Compute textures blending
	float4 TexColor =  (tex2D( textureSampler0, IN.TexCoord0 ) * (1.0f - textureBlendFactor)) + (tex2D( textureSampler1, IN.TexCoord1 ) * textureBlendFactor);

	//Final result color
	float4 final = (ambientCol + ambientMat) * lightColor * TexColor * I + Specular;

	//Fog
	if (fogEnable)
	{
		//Compute fog color only for the first light
		if (lightCurrent == 1)
		{
			//Final result color + fog
			final = lerp(final, fogColor, IN.fogLerpParam);
		}
	}

	//Alpha blend (if defined in material)
	final.a = alpha;

	return final;
}








//////////////////////////////////////////////////////////////
// Per-Pixel Directional Light
//////////////////////////////////////////////////////////////
struct V2P_Directional_PerPixel
{
	float4 Pos : POSITION;
	float4 WorldPos : TEXCOORD0;
	float3 ViewNormal : TEXCOORD1;
	float2 TexCoord0 : TEXCOORD2;
	float2 TexCoord1 : TEXCOORD3;
	float3 LightDir : TEXCOORD4;
	float3 ViewDir : TEXCOORD5;
	float  fogLerpParam : TEXCOORD6;
};
void VS_Directional_PerPixel(in A2V IN, out V2P_Directional_PerPixel OUT)
{
	//Transform the position from view space to homogeneous projection space
	OUT.Pos = mul( IN.Pos, matWorldViewProj );

	//Compute world space position
	OUT.WorldPos = mul( IN.Pos, matWorld );

	//Compute light direction
	OUT.LightDir = lightDirection;

	//Compute camera to light direction
	OUT.ViewDir = cameraPos - OUT.WorldPos;

	// Compute world space normal
	OUT.ViewNormal = normalize( mul( IN.Normal, (float3x3)matWorld) );

	//Copy the texture coordinate as is
	OUT.TexCoord0 = IN.Tex0;
	OUT.TexCoord1 = IN.Tex0;

	//Calculate Binormal and set Tangent Binormal and Normal matrix
	if (useBump)
	{
		float3x3 TBNMatrix = float3x3(IN.Binormal, IN.Tangent , IN.Normal); 
		OUT.ViewDir = mul(TBNMatrix, normalize(OUT.ViewDir));
	}

	//Fog
	if (fogEnable)
	{
		//Compute fog color only for the first light
		if (lightCurrent == 1)
		{
			float dist = distance(mul( IN.Pos, matWorld ), cameraPos);
			OUT.fogLerpParam = saturate((dist - fogStart) / fogRange);
		}
	}
}
float4 PS_Directional_PerPixel( in V2P_Directional_PerPixel IN) : COLOR0
{
	//Normalize the light direction
	IN.LightDir = normalize( IN.LightDir );

	//Normalize the View direction
	IN.ViewDir = normalize( IN.ViewDir );

	//Compute lighting amount
	float4 I = saturate( dot( normalize( IN.ViewNormal ), IN.LightDir ) );              

	//Uncompress the normal map
	if (useBump)
	{
		IN.ViewNormal = 2.0f * tex2D(textureSamplerNormals, IN.TexCoord0) - 1.0f;
	}

	//Compute specular
	float4 Specular = 0.0f;
	if (useSpecular)
	{
		float3 Reflect = normalize(2 * I * IN.ViewNormal -  IN.LightDir);
		Specular = pow(max(dot(Reflect, IN.ViewDir),0), shininess);
	}
	       
	//Compute textures blending
	float4 TexColor =  (tex2D( textureSampler0, IN.TexCoord0 ) * (1.0f - textureBlendFactor)) + (tex2D( textureSampler1, IN.TexCoord1 ) * textureBlendFactor);

	//Final result color
	float4 final = (ambientCol + ambientMat) * lightColor * TexColor * I + Specular;

	//Fog
	if (fogEnable)
	{
		//Compute fog color only for the first light
		if (lightCurrent == 1)
		{
			//Final result color + fog
			final = lerp(final, fogColor, IN.fogLerpParam);
		}		
	}
	
	//Alpha blend (if defined in material)
	final.a = alpha;

	return final;
}







//////////////////////////////////////////////////////////////
// Ambient Light
//////////////////////////////////////////////////////////////
//Vertex shader to pixel shader
struct V2P_Ambient
{
	float4 Pos : POSITION0;
	float2 TexCoord0 : TEXCOORD0;
	float2 TexCoord1 : TEXCOORD1;
};
void VS_Ambient( in A2V IN, out V2P_Ambient OUT) 
{
	OUT.Pos = mul(IN.Pos, matWorldViewProj);
	OUT.TexCoord0 = IN.Tex0;
	OUT.TexCoord1 = IN.Tex0;
}
float4 PS_Ambient( in V2P_Ambient IN) : COLOR
{
	float4 TexColor =  (tex2D( textureSampler0, IN.TexCoord0 ) * (1.0f - textureBlendFactor)) + (tex2D( textureSampler1, IN.TexCoord1 ) * textureBlendFactor);
	
	//Carmak's idea..."non influenced meshes are black"
	//final TexColor * ambientCol * 0.0f;
	float4 final = TexColor * ambientCol;
	
	//Alpha blend (if defined in material)
	final.a = alpha;

	return final;
}






//////////////////////////////////////////////////////////////
//Techniques
//////////////////////////////////////////////////////////////
technique Ambient
{
	pass P0
	{
		VertexShader = compile vs_1_1 VS_Ambient();
		PixelShader  = compile ps_1_1 PS_Ambient();
	}
}
technique PerPixel_Directional
{
	pass P0
	{
		VertexShader = compile vs_1_1 VS_Directional_PerPixel();
		PixelShader  = compile ps_2_0 PS_Directional_PerPixel();
	}
}
technique PerPixel_Point
{
	pass P0
	{
		VertexShader = compile vs_1_1 VS_Point_PerPixel();
		PixelShader  = compile ps_2_0 PS_Point_PerPixel();
	}
}
