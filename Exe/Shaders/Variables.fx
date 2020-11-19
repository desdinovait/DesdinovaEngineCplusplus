//////////////////////////////////////////////////////////////
// File: Shaded.fx
// Author: Duff
// Date Created: 15/02/2007
//////////////////////////////////////////////////////////////

//Main matricies
float4x4	matProj;
float4x4	matView;
float4x4	matWorld;
float4x4	matWorldViewProj;

//Textures
texture		texture0;
texture		texture1;
texture		textureNormals;

//Sampler Texture0
sampler textureSampler0 = sampler_state
{
	Texture = <texture0>;
	MIPFILTER = LINEAR;
	MAGFILTER = LINEAR;
	MINFILTER = LINEAR;
	ADDRESSU  = WRAP;
	ADDRESSV  = WRAP;
};

//Sampler Texture1
sampler textureSampler1 = sampler_state
{
	Texture = <texture1>;
	MIPFILTER = LINEAR;
	MAGFILTER = LINEAR;
	MINFILTER = LINEAR;
	ADDRESSU  = WRAP;
	ADDRESSV  = WRAP;
};

//Sampler Texture for Normals
sampler textureSamplerNormals = sampler_state
{
	Texture = <textureNormals>;
	MIPFILTER = LINEAR;
	MAGFILTER = LINEAR;
	MINFILTER = LINEAR;
	ADDRESSU  = WRAP;
	ADDRESSV  = WRAP;
};