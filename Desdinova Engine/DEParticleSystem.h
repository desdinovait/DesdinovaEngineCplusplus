#ifndef H_PARTICLESYSTEM_H
#define H_PARTICLESYSTEM_H

/*
[CLASS] 
[
    [NOME] 
    DEParticleSystem
    
    [DESCRIZIONE]   
    Classe per la creazione di un sistema particellare configurabile attraverso
	varie proprietà (vedere alcuni esempi in fondo a questo file).
	E' implementata anche una semplice procedura di collisione delle particelle
	tramite piani.
]
*/
class DEParticleSystem
{
	private:
		//** Strutture e altre definizioni **
		
		// Classify Point
		int CP_FRONT;
		int CP_BACK;
		int CP_ONPLANE;
		// Collision Results
		int CR_BOUNCE;
		int CR_STICK;
		int CR_RECYCLE;

		struct Plane
		{
			D3DXVECTOR3 m_vNormal;           // The plane's normal
			D3DXVECTOR3 m_vPoint;            // A coplanar point within the plane
			float       m_fBounceFactor;     // Coefficient of restitution (or how bouncy the plane is)
			int         m_nCollisionResult;  // What will particles do when they strike the plane
			Plane      *m_pNext;             // Next plane in list
		};

		struct Particle
		{
			D3DXVECTOR3 m_vCurPos;    // Current position of particle
			D3DXVECTOR3 m_vCurVel;    // Current velocity of particle
			float       m_fInitTime;  // Time of creation of particle
			D3DXCOLOR	m_cColor;
			Particle   *m_pNext;      // Next particle in list
		};

		// Custom vertex and FVF declaration for point sprite vertex points
		struct PointVertex
		{
			D3DXVECTOR3 posit;
			D3DCOLOR    color;
			enum FVF
			{
				FVF_Flags = D3DFVF_XYZ | D3DFVF_DIFFUSE
			};
		};


		//** Variabili **
		DWORD					vertexProcessingUsage;
		LPDIRECT3DVERTEXBUFFER9 m_pVB;
		char					fileName[MAX_PATH];
		char					name[MAX_PATH];
		DWORD					m_dwVBOffset;
		DWORD					m_dwFlush;
		DWORD					m_dwDiscard;
		Particle				*m_pActiveList;
		Particle				*m_pFreeList;
		Plane					*m_pPlanes;
		DWORD					m_dwActiveCount;
		float					m_fCurrentTime;
		float					m_fLastUpdate;
		float					m_fMaxPointSize;
		bool					m_bDeviceSupportsPSIZE;

		DWORD					m_dwMaxParticles;
		DWORD					m_dwNumToRelease;
		float					m_fReleaseInterval;
		float					m_fLifeCycle;
		float					m_fSize;
		D3DXCOLOR				m_clrColor;
		D3DXVECTOR3				m_vPosition;
		D3DXVECTOR3				m_vVelocity;
		D3DXVECTOR3				m_vGravity;
		D3DXVECTOR3				m_vWind;
		bool					m_bAirResistence;
		float					m_fVelocityVar;
		char					*m_chTexFile;
		float					g_fElpasedTime;
		double					g_dCurTime;
		double					g_dLastTime;
		ENUM_DE_PSTYPE			g_psType;
		D3DXVECTOR3				m_vDistance;
		bool					m_bRelativeY;
		unsigned int			textureID;
		char					textureFile[MAX_PATH];
		LPCSTR					mainLogTitle;
		LPCSTR					mainTag;

	public:
		//** Funzioni **
		DLL_API					DEParticleSystem();
		DLL_API HRESULT			Create(LPCSTR logTitle="DEParticleSystem");
		DLL_API bool			Invalidate();
		DLL_API bool			Restore();
		DLL_API bool			Release();

		DLL_API void			SetMaxParticles( DWORD dwMaxParticles ) { m_dwMaxParticles = dwMaxParticles; }
		DLL_API void			SetNumToRelease( DWORD dwNumToRelease ) { m_dwNumToRelease = dwNumToRelease; }
		DLL_API void			SetReleaseInterval( float fReleaseInterval ) { m_fReleaseInterval = fReleaseInterval; }
		DLL_API void			SetLifeCycle( float fLifeCycle ) { m_fLifeCycle = fLifeCycle; }
		DLL_API void			SetSize( float fSize ) { m_fSize = fSize; }
		DLL_API void			SetColor( D3DXCOLOR clrColor ) { m_clrColor = clrColor; }
		DLL_API void			SetPosition( D3DXVECTOR3 vPosition ) { m_vPosition = vPosition; }
		DLL_API void			SetVelocity( D3DXVECTOR3 vVelocity ) { m_vVelocity = vVelocity; }	
		DLL_API void			SetGravity( D3DXVECTOR3 vGravity ) { m_vGravity = vGravity; }
		DLL_API void			SetWind( D3DXVECTOR3 vWind ) { m_vWind = vWind; }
		DLL_API void			SetDistance( D3DXVECTOR3 vDistance ) { m_vDistance = vDistance; }
		DLL_API void			SetType(ENUM_DE_PSTYPE psNewType){ g_psType = psNewType;}
		DLL_API void			SetRelativeY(bool bNewRelativeY) {m_bRelativeY = bNewRelativeY;}
		DLL_API void			SetAirResistence( bool bAirResistence ) { m_bAirResistence = bAirResistence; }
		DLL_API void			SetVelocityVar( float fVelocityVar ) { m_fVelocityVar = fVelocityVar; }
		DLL_API bool			SetTexture( char *chTexFile );
		DLL_API void			SetCollisionPlane( D3DXVECTOR3 vPlaneNormal, D3DXVECTOR3 vPoint, float fBounceFactor = 1.0f, int nCollisionResult = 0 );

		DLL_API DWORD			GetMaxParticles( void ) { return m_dwMaxParticles; }
		DLL_API DWORD			GetNumToRelease( void ) { return m_dwNumToRelease; }
		DLL_API float			GetReleaseInterval( void ) { return m_fReleaseInterval; }
		DLL_API float			GetLifeCycle( void ) { return m_fLifeCycle; }
		DLL_API float			GetSize( void ) { return m_fSize; }
		DLL_API D3DXCOLOR		GetColor( void ) { return m_clrColor; }
		DLL_API D3DXVECTOR3		GetPosition( void ) { return m_vPosition; }
		DLL_API D3DXVECTOR3		GetVelocity( void ) { return m_vVelocity; }
		DLL_API D3DXVECTOR3		GetGravity( void ) { return m_vGravity; }
		DLL_API D3DXVECTOR3		GetWind( void ) { return m_vWind; }
		DLL_API D3DXVECTOR3		GetDistance( void ) { return m_vDistance; }
		DLL_API ENUM_DE_PSTYPE	GetType( void ) { return g_psType;}
		DLL_API bool			GetRelativeY( void ) { return m_bRelativeY;}
		DLL_API bool			GetAirResistence( void ) { return m_bAirResistence; }
		DLL_API float			GetVelocityVar( void ) { return m_fVelocityVar; }
		DLL_API float			GetMaxPointSize( void ) { return m_fMaxPointSize; }
		DLL_API char			*GetName();
		DLL_API char			*GetTextureName();

		DLL_API void			SetDefaultRenderStates();
		DLL_API HRESULT			Update( float fElapsedTime );
		DLL_API HRESULT			Render();
		DLL_API void			Execute(int mode);
		DLL_API void			RestartParticleSystem(void);
		DLL_API int				classifyPoint( D3DXVECTOR3 *vPoint, Plane *pPlane );

		DLL_API bool			New(LPCTSTR newFileNameImage);
		DLL_API bool			Load(LPCTSTR file);
		DLL_API bool			Reload();
		DLL_API bool			Save(LPCTSTR scriptFileName);
		//Tag
		DLL_API bool			SetTag(LPCSTR newTag);
		DLL_API LPCSTR			GetTag();
};

/***** EXAMPLE ****

//
// Exploding burst
//
g_pParticleSystems[0] = new CParticleSystem();
g_pParticleSystems[0]->SetTexture( "particle.bmp" );
g_pParticleSystems[0]->SetMaxParticles( 100 );
g_pParticleSystems[0]->SetNumToRelease( 100 );
g_pParticleSystems[0]->SetReleaseInterval( 0.05f );
g_pParticleSystems[0]->SetLifeCycle( 0.5f );
g_pParticleSystems[0]->SetSize( 1.0f );
g_pParticleSystems[0]->SetColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
g_pParticleSystems[0]->SetPosition( D3DXVECTOR3( 0.0f, 5.0f, 0.0f) );
g_pParticleSystems[0]->SetVelocity( D3DXVECTOR3( 0.0f, 0.0f, 0.0f) );
g_pParticleSystems[0]->SetGravity( D3DXVECTOR3( 0.0f, 0.0f, 0.0f) );
g_pParticleSystems[0]->SetWind( D3DXVECTOR3( 0.0f, 0.0f, 0.0f) );
g_pParticleSystems[0]->SetVelocityVar( 10.0f );
g_pParticleSystems[0]->Init( g_pd3dDevice );

//
// Wind blown fountain
//
g_pParticleSystems[1] = new CParticleSystem();
g_pParticleSystems[1]->SetTexture( "particle.bmp" );
g_pParticleSystems[1]->SetMaxParticles( 500 );
g_pParticleSystems[1]->SetNumToRelease( 5 );
g_pParticleSystems[1]->SetReleaseInterval( 0.05f );
g_pParticleSystems[1]->SetLifeCycle( 4.0f );
g_pParticleSystems[1]->SetSize( 0.5f );
g_pParticleSystems[1]->SetColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ));
g_pParticleSystems[1]->SetPosition( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
g_pParticleSystems[1]->SetVelocity( D3DXVECTOR3( 0.0f, 5.0f, 0.0f ) );
g_pParticleSystems[1]->SetGravity( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
g_pParticleSystems[1]->SetWind( D3DXVECTOR3( 2.0f, 0.0f, 0.0f ) );
g_pParticleSystems[1]->SetVelocityVar( 1.5f );
g_pParticleSystems[1]->Init( g_pd3dDevice );

//
// Omni-directiional emission expanding into space with no air resistence
//
g_pParticleSystems[2] = new CParticleSystem();
g_pParticleSystems[2]->SetTexture( "particle.bmp" );
g_pParticleSystems[2]->SetMaxParticles( 2048 );
g_pParticleSystems[2]->SetNumToRelease( 10 );
g_pParticleSystems[2]->SetReleaseInterval( 0.05f );
g_pParticleSystems[2]->SetLifeCycle( 5.0f );
g_pParticleSystems[2]->SetSize( 0.5f );
g_pParticleSystems[2]->SetColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ));
g_pParticleSystems[2]->SetPosition( D3DXVECTOR3( 0.0f, 0.0f, 0.0f) );
g_pParticleSystems[2]->SetVelocity( D3DXVECTOR3( 0.0f, 0.0f, 0.0f) );
g_pParticleSystems[2]->SetGravity( D3DXVECTOR3( 0.0f, 0.0f, 0.0f) );
g_pParticleSystems[2]->SetWind( D3DXVECTOR3( 0.0f, 0.0f, 0.0f) );
g_pParticleSystems[2]->SetAirResistence( false );
g_pParticleSystems[2]->SetVelocityVar(2.0f);
g_pParticleSystems[2]->Init( g_pd3dDevice );

//
// Fountain particles behave like paint spots when striking a plane as 
// the wind blowing them towards us
//
g_pParticleSystems[3] = new CParticleSystem();
g_pParticleSystems[3]->SetTexture( "particle.bmp" );
g_pParticleSystems[3]->SetMaxParticles( 100 );
g_pParticleSystems[3]->SetNumToRelease( 10 );
g_pParticleSystems[3]->SetReleaseInterval( 0.05f );
g_pParticleSystems[3]->SetLifeCycle( 3.0f );
g_pParticleSystems[3]->SetSize( 0.5f );
g_pParticleSystems[3]->SetColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ));
g_pParticleSystems[3]->SetPosition( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
g_pParticleSystems[3]->SetVelocity( D3DXVECTOR3( 0.0f, 5.0f, 0.0f ) );
g_pParticleSystems[3]->SetGravity( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
g_pParticleSystems[3]->SetWind( D3DXVECTOR3( 0.0f, 0.0f, -20.0f ) );
g_pParticleSystems[3]->SetVelocityVar( 2.5f );
g_pParticleSystems[3]->SetCollisionPlane( D3DXVECTOR3( 0.0f, 0.0f,1.0f ), 
                                        D3DXVECTOR3( 0.0f, 0.0f, -5.0f ),
                                        1.0f, CR_STICK );
g_pParticleSystems[3]->Init( g_pd3dDevice );

//
// Fountain using a single collision plane acting as a floor
//
g_pParticleSystems[4] = new CParticleSystem();
g_pParticleSystems[4]->SetTexture( "particle.bmp" );
g_pParticleSystems[4]->SetMaxParticles( 200 );
g_pParticleSystems[4]->SetNumToRelease( 10 );
g_pParticleSystems[4]->SetReleaseInterval( 0.05f );
g_pParticleSystems[4]->SetLifeCycle( 5.0f );
g_pParticleSystems[4]->SetSize( 1.5f );
g_pParticleSystems[4]->SetColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ));
g_pParticleSystems[4]->SetPosition( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
g_pParticleSystems[4]->SetVelocity( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
g_pParticleSystems[4]->SetGravity( D3DXVECTOR3( 0.0f, -9.8f, 0.0f ) );
g_pParticleSystems[4]->SetWind( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
g_pParticleSystems[4]->SetVelocityVar( 20.0f );
g_pParticleSystems[4]->SetCollisionPlane( D3DXVECTOR3( 0.0f, 1.0f, 0.0f ), 
                                        D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
g_pParticleSystems[4]->Init( g_pd3dDevice );

//
// Fountain boxed-in by 6 collision planes
//
g_pParticleSystems[5] = new CParticleSystem();
g_pParticleSystems[5]->SetTexture( "particle.bmp" );
g_pParticleSystems[5]->SetMaxParticles( 100 );
g_pParticleSystems[5]->SetNumToRelease( 5 );
g_pParticleSystems[5]->SetReleaseInterval( 0.05f );
g_pParticleSystems[5]->SetLifeCycle( 5.0f );
g_pParticleSystems[5]->SetSize( 1.5f );
g_pParticleSystems[5]->SetColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ));
g_pParticleSystems[5]->SetPosition( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
g_pParticleSystems[5]->SetVelocity( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
g_pParticleSystems[5]->SetGravity( D3DXVECTOR3( 0.0f, -9.8f, 0.0f ) );
g_pParticleSystems[5]->SetWind( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
g_pParticleSystems[5]->SetVelocityVar( 20.0f );

// Create a series of planes to collide with
g_pParticleSystems[5]->SetCollisionPlane( D3DXVECTOR3( 0.0f, 1.0f, 0.0f ), 
                                        D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) ); // Floor
g_pParticleSystems[5]->SetCollisionPlane( D3DXVECTOR3( 1.0f, 0.0f, 0.0f ), 
                                        D3DXVECTOR3(-3.0f, 0.0f, 0.0f ) ); // Left Wall
g_pParticleSystems[5]->SetCollisionPlane( D3DXVECTOR3(-1.0f, 0.0f, 0.0f ), 
                                        D3DXVECTOR3( 3.0f, 0.0f, 0.0f ) ); // Right Wall
g_pParticleSystems[5]->SetCollisionPlane( D3DXVECTOR3( 0.0f, 0.0f, 1.0f ), 
                                        D3DXVECTOR3( 0.0f, 0.0f,-3.0f ) ); // Front Wall
g_pParticleSystems[5]->SetCollisionPlane( D3DXVECTOR3( 0.0f, 0.0f,-1.0f ), 
                                        D3DXVECTOR3( 0.0f, 0.0f, 3.0f ) ); // Back Wall
g_pParticleSystems[5]->SetCollisionPlane( D3DXVECTOR3( 0.0f,-1.0f, 0.0f ), 
                                        D3DXVECTOR3( 0.0f, 5.0f, 0.0f ) ); // Ceiling
g_pParticleSystems[5]->Init( g_pd3dDevice );


*/
#endif