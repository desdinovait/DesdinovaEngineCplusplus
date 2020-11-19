#ifndef H_STARFIELD_H
#define H_STARFIELD_H



struct Vertex
{
    D3DXVECTOR3 Position;
    D3DCOLOR    Diffuse;
    D3DXVECTOR2 TexCoord;
};

D3DVERTEXELEMENT9 VertexElements[] =
{
    { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
    { 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    D3DDECL_END()
};


float RandFloat( float fMin, float fMax )
{
	const float fRand = (float)rand () / RAND_MAX;
	return fMin + ( fMax - fMin ) * fRand;
}

#define		DEG_TO_RAD( x )		( x ) * 0.0174532925f
#define		MAX_STARS		10000
const int NUM_STAR_VERTS = MAX_STARS * 2;
const float g_fStarDist = 1.0f;
float g_fSpeed = 0.5f;
float g_fStreakDelta = 0.4f;
float g_fMaxStreakLen = 0.5f;
int g_iNumStarsToDisplay = MAX_STARS / 2;
float g_fFOV = 45.0f;
float g_fAspectRatio;
#define SQUARE( x )		( ( x ) * ( x ) )
const float TWO_PI = 3.14f * 2.0f;
const float invRandMax = 1.0f / RAND_MAX;
const float invRandMaxTimesPI = TWO_PI * invRandMax;
float theta, phi, u, v;




class CStar
{
public:
	D3DXVECTOR3		m_vCurPos;
	D3DXVECTOR3		m_vWorldLastPos;
	D3DXVECTOR3		m_vWorldPos;
	D3DCOLOR		m_Color;
	D3DXVECTOR3		m_vVel;

	// The next time to update the streak.
	double			m_fNextUpdate;
	double			m_fUpdateDelta;

	// Constructor.
	CStar()
	{
		m_fNextUpdate = 0.0;
	}

	// Generate a new random Position for this Star.
	void GenNewPos()
	{
		// Uniformly distribute the points around the sphere.
		// Equation From: http://mathworld.wolfram.com/SpherePointPicking.html

		u = RandFloat( 0.0f, 1.0f );
		v = RandFloat( 0.0f, 1.0f );
		theta = TWO_PI * u;
		phi = acosf( 2 * v - 1.0f );

		m_vCurPos.x = cosf( theta ) * sinf( phi );
		m_vCurPos.y = sinf( theta ) * sinf( phi );
		m_vCurPos.z = cosf( phi );
	}

	// Calculate the new color.
	void CalcNewColor()
	{
		int iIntensity = int( ( ( g_fStarDist + m_vCurPos.z ) * 0.5f ) * 255.0f );
		m_Color = D3DCOLOR_ARGB( 255, iIntensity, iIntensity, iIntensity );
	}

	// Calculate the new velocity.
	void CalcNewVel()
	{
		// Random Velocity.
		m_vVel = D3DXVECTOR3( 0.0f, 0.0f, RandFloat( g_fSpeed * 0.95f, g_fSpeed * 1.05f ) );
	}
};





class DEStarfield
{
	private:
		CStar g_Stars[ MAX_STARS ];
		IDirect3DVertexDeclaration9* g_pVertexDecl;
		IDirect3DVertexBuffer9*      g_pVBTriangles;
		ID3DXEffect*				 g_pEffect;

	public:	
		DLL_API	void Create();
		DLL_API void Render(float time);

};


#endif