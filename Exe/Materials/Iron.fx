////////////////////////////////////////////////
// Desdinova Engine - Material Definition 1.0 //
////////////////////////////////////////////////

//Name
string	Material_Name			= "Iron";

//Physics properties
float	Material_Density		= 7800.0f;		//See table of densities
float	Material_Restitution		= 0.3f;			//Should not be higher than 1
float	Material_StaticFriction		= 0.5f;			//Can be higher than 1
float	Material_DynamicFriction	= 0.6f;			//Should not be higher than 1
bool	Material_IsAnisotropic		= false;
float	Material_DynamicFrictionV	= 0.8f;			//If anisotropic is True
float	Material_StaticFrictionV	= 1.0f;			//If anisotropic is True
float3	Material_DirOfAnisotropy	= { 0.0f, 0.0f, 1.0f };	//If anisotropic is True (Vector relative to the shape's own coordinate frame)

//Lights properties
float4	Material_Ambient		= { 1.0f, 1.0f, 1.0f, 1.0f };
float4	Material_Diffuse		= { 1.0f, 1.0f, 1.0f, 1.0f };
float	Material_Shininess		= 1000.0f;
float	Material_Alpha			= 0.5f;			//Used by Material_IsTransparent = true

//Graphics properties
bool	Material_UseDirectionalLights	= true;
bool	Material_UsePointLights		= true;
bool	Material_IsSpecular		= false;
bool	Material_IsBumpmapped		= false;
bool	Material_IsTransparent		= false;
bool	Material_IsEnvironmental	= false;		//Not yet implemented


//Default Technique (do NOT cancel)
technique Main
{
}