//////////////////////////////////////////////////////////
//							//
//   ..::Desdinova Engine: 18 Post-process effects::..	//
//							//
//////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------
// Effect File Variables
//-----------------------------------------------------------------------------
texture offscreenTexture;
texture normalTexture;

string normalFilename = "Textures//Normal.tga";
float2 BlurTexelsize = {0.0015f, 0.0015f};
float2 EmbossTexelsize = {0.0015f, 0.0015f};
float2 EdgeTexelsize = {0.0015f, 0.0015f};
float BloomScale = 1.5f;
float TonemapLuminance = 0.09f;
float3 FadeValue;

sampler2D PostProcessSampler = sampler_state
{
	Texture   = <offscreenTexture>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = Wrap;
	AddressV  = Wrap;
};

sampler2D NormalMap = sampler_state
{
	Texture	= <normalTexture>;
	MagFilter = LINEAR;
	MinFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = Wrap;
	AddressV  = Wrap;
};



//-----------------------------------------------------------------------------
// Pixel and Vertex Shaders
//-----------------------------------------------------------------------------

//Pass Through Vertex Shader
void PassThrough_VS(in float4 iPosition : POSITION, in float2 iTexCoord : TEXCOORD0, out float4 oPosition   : POSITION, out float2 oTexCoord   : TEXCOORD0)
{	
	oPosition=iPosition;
	oTexCoord=iTexCoord;
}


//Sepia
float4 Sepia_PS(float2 iTexCoord : TEXCOORD0) : COLOR
{	
	float4 vWeightsBW=float4(0.3333,0.59,0.11,0);
	float4 vWeightsSepia=float4(0.9,0.7,0.3,1);
	float4 cColor=tex2D(PostProcessSampler,iTexCoord);
	//Apply sepia tone
	return dot(cColor,vWeightsBW) * vWeightsSepia;
}


//Inverse Color
float4 Inverse_PS(float2 iTexCoord : TEXCOORD0) : COLOR
{	
	return 1.0f - tex2D(PostProcessSampler, iTexCoord);
}


//Black and White
float4 BlackAndWhite_PS(float2 iTexCoord : TEXCOORD0) : COLOR
{	
	float3 LuminanceConv = { 0.2125f, 0.7154f, 0.0721f };
	return dot( (float3)tex2D( PostProcessSampler, iTexCoord ), LuminanceConv );
}


//Blur
float4 Blur_PS(float2 iTexCoord : TEXCOORD0) : COLOR
{	
	float2 usedTexelBlur[8] = {
		-1, -1,
		 0, -1,
		 1, -1,
		-1,  0,
		 1,  0,
		-1,  1,
		 0,  1,
	     1,  1,	
	};

	float4 cAvgColor=tex2D(PostProcessSampler, iTexCoord);	
	
	cAvgColor+=tex2D(PostProcessSampler, iTexCoord + BlurTexelsize*usedTexelBlur[0]);
	cAvgColor+=tex2D(PostProcessSampler, iTexCoord + BlurTexelsize*usedTexelBlur[1]);
	cAvgColor+=tex2D(PostProcessSampler, iTexCoord + BlurTexelsize*usedTexelBlur[2]);
	cAvgColor+=tex2D(PostProcessSampler, iTexCoord + BlurTexelsize*usedTexelBlur[3]);
	cAvgColor+=tex2D(PostProcessSampler, iTexCoord + BlurTexelsize*usedTexelBlur[4]);
	cAvgColor+=tex2D(PostProcessSampler, iTexCoord + BlurTexelsize*usedTexelBlur[5]);
	cAvgColor+=tex2D(PostProcessSampler, iTexCoord + BlurTexelsize*usedTexelBlur[6]);
	cAvgColor+=tex2D(PostProcessSampler, iTexCoord + BlurTexelsize*usedTexelBlur[7]);
	// Average
	cAvgColor/=9;
		
	return cAvgColor;
}


//Emboss
float4 Emboss_PS(float2 iTexCoord : TEXCOORD0) : COLOR
{	
	float4 cAvgColor=tex2D(PostProcessSampler, iTexCoord);	
	cAvgColor-=tex2D(PostProcessSampler, iTexCoord + EmbossTexelsize*float2(-1,-1));		
	cAvgColor-=0.5*tex2D(PostProcessSampler, iTexCoord + EmbossTexelsize*float2(0,-1));		
	cAvgColor-=0.5*tex2D(PostProcessSampler, iTexCoord + EmbossTexelsize*float2(-1,0));		
	cAvgColor+=0.5*tex2D(PostProcessSampler, iTexCoord + EmbossTexelsize*float2(1,0));		
	cAvgColor+=0.5*tex2D(PostProcessSampler, iTexCoord + EmbossTexelsize*float2(0,1));		
	cAvgColor+=tex2D(PostProcessSampler, iTexCoord + EmbossTexelsize*float2(1,1));	
				
	return cAvgColor;
}


//Glass
float4 Glass_PS(float2 iTexCoord : TEXCOORD0) : COLOR
{	
	float4 normal=2.0f*(tex2D(NormalMap,iTexCoord*3)-0.5);
	float4 color=tex2D(PostProcessSampler,iTexCoord+normal.xy*0.05f);
	return color;
}


//Edge Detection
float4 EdgeDetection_PS(float2 iTexCoord : TEXCOORD0) : COLOR
{	
	float2 usedTexelED[8] = 
	{		
		-1, -1,
		0, -1,
		1, -1,
		-1,  0,
		1,  0,
		-1,  1,
		0,  1,
		1,  1,	
	};	
	float4 cAvgColor=9*tex2D(PostProcessSampler, iTexCoord);	
	
	for(int t=0; t<8; t++)
	{
		cAvgColor-=tex2D(PostProcessSampler, iTexCoord + EdgeTexelsize*usedTexelED[t]);		
	}
	cAvgColor-=tex2D(PostProcessSampler, iTexCoord);
	cAvgColor=1-cAvgColor;
	return cAvgColor;
}


//Bloom
float4 Bloom_PS( float2 Tex : TEXCOORD0 ) : COLOR0
{
	static const int g_cKernelSize = 13;
	float4 Color = 0;
	float2 PixelKernel[g_cKernelSize] =
	{
	    { -6, 0 },
	    { -5, 0 },
	    { -4, 0 },
	    { -3, 0 },
	    { -2, 0 },
	    { -1, 0 },
	    {  0, 0 },
	    {  1, 0 },
	    {  2, 0 },
	    {  3, 0 },
	    {  4, 0 },
	    {  5, 0 },
	    {  6, 0 },
	};
	static const float BlurWeights[g_cKernelSize] = 
	{
	    0.002216,
	    0.008764,
	    0.026995,
	    0.064759,
	    0.120985,
	    0.176033,
	    0.199471,
	    0.176033,
	    0.120985,
	    0.064759,
	    0.026995,
	    0.008764,
	    0.002216,
	};

	for (int i = 0; i < g_cKernelSize; i++)
	{    
		Color += tex2D( PostProcessSampler, Tex ) * BlurWeights[i];
	}
	return Color * BloomScale;
}


//Tone Mapping
float4 ToneMap_PS( float2 Tex : TEXCOORD ) : COLOR0
{
	static const float fMiddleGray = 0.18f;
	static const float fWhiteCutoff = 0.8f;
	float4 Color;
	Color = tex2D( PostProcessSampler, Tex ) * fMiddleGray / ( TonemapLuminance + 0.001f );
	Color *= ( 1.0f + ( Color / ( fWhiteCutoff * fWhiteCutoff ) ) );
	Color /= ( 1.0f + Color );
	return Color;
}


//Old B&W
float4 BlackAndWhite2_PS(float2 iTexCoord : TEXCOORD) : COLOR0
{
    float4 Orig = tex2D(PostProcessSampler, iTexCoord);
    float2 PixelKernel[4] =
    {
        { 0,  1},
        { 1,  0},
        { 0, -1},
        {-1,  0}
    };
    float4 Sum = 0;
    for(int i = 0; i < 4; i++)
    {
        Sum += saturate(1 - dot(Orig.xyz, tex2D(PostProcessSampler, iTexCoord + PixelKernel[i]).xyz))*0.275;
    }

    return Sum;
}

//Green
float4 Fade_PS(float2 iTexCoord : TEXCOORD) : COLOR0
{
    float4 Color = tex2D(PostProcessSampler,iTexCoord); 
    Color.x = Color.x * FadeValue.x;
    Color.y = Color.y * FadeValue.y;
    Color.z = Color.z * FadeValue.z;
    return Color;
}

//Red
float4 Red_PS(float2 iTexCoord : TEXCOORD) : COLOR0
{
    float4 Color = tex2D(PostProcessSampler, iTexCoord); 
    Color.y = 0.0f;
    Color.z = 0.0f;
    return Color;
}


//Green
float4 Green_PS(float2 iTexCoord : TEXCOORD) : COLOR0
{
    float4 Color = tex2D(PostProcessSampler,iTexCoord); 
    Color.x = 0.0f;
    Color.z = 0.0f;
    return Color;
}


//Blue
float4 Blue_PS(float2 iTexCoord : TEXCOORD) : COLOR0
{
    float4 Color = tex2D(PostProcessSampler, iTexCoord); 
    Color.x = 0.0f;
    Color.y = 0.0f;
    return Color;
}


//Bright
float4 Bright_PS(float2 iTexCoord : TEXCOORD) : COLOR0
{
    float4 Color = tex2D(PostProcessSampler, iTexCoord); 
    return Color + (Color * 0.5f);
}


//Dark
float4 Dark_PS(float2 iTexCoord : TEXCOORD) : COLOR0
{
    float4 Color = tex2D(PostProcessSampler, iTexCoord); 
    return Color - (Color * 0.5f);
}


//Blur2
float4 Blur2_PS(float2 iTexCoord : TEXCOORD) : COLOR0
{
    float4 Color = tex2D(PostProcessSampler, iTexCoord); 
    Color += tex2D(PostProcessSampler, iTexCoord + float2(0.010f, 0.0f));
    Color += tex2D(PostProcessSampler, iTexCoord + float2(0.020f, 0.0f));
    return Color/3;
}


//Underwater
float4 Underwater_PS(float2 iTexCoord : TEXCOORD) : COLOR0
{
    //Vertical pertubation
    iTexCoord.y = iTexCoord.y + (sin(iTexCoord.x*50.0f)*0.01f);
    iTexCoord.x = iTexCoord.x + (cos(0.5f)*0.01f);
    
    //Orizontal perturbation
    iTexCoord.x = iTexCoord.x + (sin(iTexCoord.y*50.0f)*0.01f);
    iTexCoord.y = iTexCoord.y + (cos(0.5f)*0.01f);

    return tex2D(PostProcessSampler, iTexCoord);
}


//Sharpen
float4 Sharpen_PS(float2 iTexCoord : TEXCOORD) : COLOR0
{
    float4 Color = tex2D(PostProcessSampler, iTexCoord); 
    Color -= tex2D(PostProcessSampler, iTexCoord + 0.0001f) * 10.0f; 
    Color += tex2D(PostProcessSampler, iTexCoord - 0.0001f) * 10.0f;
    return Color;
}




//-----------------------------------------------------------------------------
// List of PostProcessing Technique
//-----------------------------------------------------------------------------
technique TechniqueBlackAndWhite
{
	Pass P0
	{	
		CullMode=none;
		ZEnable=false;
		VertexShader = compile vs_1_0 PassThrough_VS();
		PixelShader = compile ps_2_0 BlackAndWhite_PS();
	}
}

technique TechniqueBlackAndWhite2
{
	Pass P0
	{	
		CullMode=none;
		ZEnable=false;
		VertexShader = compile vs_1_0 PassThrough_VS();
		PixelShader = compile ps_2_0 BlackAndWhite2_PS();
	}
}

technique TechniqueInverse
{
	Pass P0
	{	
		CullMode=none;
		ZEnable=false;
		VertexShader = compile vs_1_0 PassThrough_VS();
		PixelShader = compile ps_1_0 Inverse_PS();
	}
}

technique TechniqueSepia
{
	Pass P0
	{	
		CullMode=none;
		ZEnable=false;
		VertexShader = compile vs_1_0 PassThrough_VS();
		PixelShader = compile ps_2_0 Sepia_PS();
	}
}

technique TechniqueBlur
{
	Pass P0
	{	
		CullMode=none;
		ZEnable=false;
		VertexShader = compile vs_1_0 PassThrough_VS();
		PixelShader = compile ps_2_0 Blur_PS();
	
	}
}

technique TechniqueBlur2
{
	Pass P0
	{	
		CullMode=none;
		ZEnable=false;		
		VertexShader = compile vs_1_0 PassThrough_VS();
		PixelShader = compile ps_2_0 Blur2_PS();
	}
}

technique TechniqueTonemap
{
	Pass P0
	{	
		CullMode=none;
		ZEnable=false;
		VertexShader = compile vs_1_0 PassThrough_VS();
		PixelShader  = compile ps_2_0 ToneMap_PS();
	}
}

technique TechniqueBloom
{
	Pass P0
	{	
		CullMode=none;
		ZEnable=false;
		VertexShader = compile vs_1_0 PassThrough_VS();
		PixelShader  = compile ps_2_0 Bloom_PS();
	}
}

technique TechniqueGlass
{
	Pass P0
	{	
		CullMode=none;
		ZEnable=false;		
		VertexShader = compile vs_1_0 PassThrough_VS();
		PixelShader = compile ps_2_0 Glass_PS();
	}
}

technique TechniqueEdgeDetection
{
	Pass P0
	{	
		CullMode=none;
		ZEnable=false;		
		VertexShader = compile vs_1_0 PassThrough_VS();
		PixelShader = compile ps_2_0 EdgeDetection_PS();
	}
}

technique TechniqueEmboss
{
	Pass P0
	{	
		CullMode=none;
		ZEnable=false;		
		VertexShader = compile vs_1_0 PassThrough_VS();
		PixelShader = compile ps_2_0 Emboss_PS();
	}
}

technique TechniqueSharpen
{
	Pass P0
	{	
		CullMode=none;
		ZEnable=false;		
		VertexShader = compile vs_1_0 PassThrough_VS();
		PixelShader = compile ps_2_0 Sharpen_PS();
	}
}

technique TechniqueFade
{
	Pass P0
	{	
		CullMode=none;
		ZEnable=false;		
		VertexShader = compile vs_1_0 PassThrough_VS();
		PixelShader = compile ps_1_0 Fade_PS();
	}
}

technique TechniqueRed
{
	Pass P0
	{	
		CullMode=none;
		ZEnable=false;		
		VertexShader = compile vs_1_0 PassThrough_VS();
		PixelShader = compile ps_1_0 Red_PS();
	}
}

technique TechniqueGreen
{
	Pass P0
	{	
		CullMode=none;
		ZEnable=false;		
		VertexShader = compile vs_1_0 PassThrough_VS();
		PixelShader = compile ps_1_0 Green_PS();
	}
}

technique TechniqueBlue
{
	Pass P0
	{	
		CullMode=none;
		ZEnable=false;		
		VertexShader = compile vs_1_0 PassThrough_VS();
		PixelShader = compile ps_1_0 Blue_PS();
	}
}

technique TechniqueBright
{
	Pass P0
	{	
		CullMode=none;
		ZEnable=false;		
		VertexShader = compile vs_1_0 PassThrough_VS();
		PixelShader = compile ps_1_0 Bright_PS();
	}
}

technique TechniqueDark
{
	Pass P0
	{	
		CullMode=none;
		ZEnable=false;		
		VertexShader = compile vs_1_0 PassThrough_VS();
		PixelShader = compile ps_1_0 Dark_PS();
	}
}

technique TechniqueUnderwater
{
	Pass P0
	{	
		CullMode=none;
		ZEnable=false;		
		VertexShader = compile vs_1_0 PassThrough_VS();
		PixelShader = compile ps_2_0 Underwater_PS();
	}
}