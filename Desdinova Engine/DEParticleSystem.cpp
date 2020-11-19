#include "DEGeneralIncludes.h"

DLL_API DEParticleSystem::DEParticleSystem()
{
	mainLogTitle = NULL;
	mainTag = NULL;
	//isCreated = false;
}

DLL_API int DEParticleSystem::classifyPoint( D3DXVECTOR3 *vPoint, Plane *pPlane )
{
	D3DXVECTOR3 vDirection = pPlane->m_vPoint - *vPoint;
	float fResult = D3DXVec3Dot( &vDirection, &pPlane->m_vNormal );

	if( fResult < -0.001f )
        return CP_FRONT;

	if( fResult >  0.001f )
        return CP_BACK;

	return CP_ONPLANE;
}


DLL_API bool DEParticleSystem::Release()
{
	try
	{
		while( m_pPlanes )
		{
			Plane *pPlane = m_pPlanes;
			m_pPlanes = pPlane->m_pNext;
			delete pPlane;
		}

		while( m_pActiveList )
		{
			Particle* pParticle = m_pActiveList;
			m_pActiveList = pParticle->m_pNext;
			delete pParticle;
		}
		m_pActiveList = NULL;

		while( m_pFreeList )
		{
			Particle *pParticle = m_pFreeList;
			m_pFreeList = pParticle->m_pNext;
			delete pParticle;
		}
		m_pFreeList = NULL;

		SafeRelease(m_pVB);

		DETextureManager::ReleaseTexture(textureID);
		return true;
	}
	catch(...)
	{
		DELog::LogWarning("<li>%s : Unknown statement error - Probably NOT Released", mainLogTitle);
		return false;
	}
	return false;
}


DLL_API HRESULT	DEParticleSystem::Create(LPCSTR logTitle)
{
    HRESULT hr;

	memset(name, 0, sizeof(char) * MAX_PATH);
	strcpy(name, "NewParticlesSystem");

	mainLogTitle = logTitle;
	mainTag = NULL;

	m_dwVBOffset       = 0;
    m_dwFlush          = 512;  // Number of point sprites to load before sending them to hardware(512 = 2048 divided into 4 chunks)
    m_dwDiscard        = 2048; // Max number of point sprites the vertex buffer can load until we are forced to discard and start over
    m_pActiveList      = NULL;
    m_pFreeList        = NULL;
    m_pPlanes          = NULL;
	m_dwActiveCount    = 0;
	m_fCurrentTime     = 0.0f;
	m_fLastUpdate      = 0.0f;
    m_pVB              = NULL;
    m_dwMaxParticles   = 1;
    m_dwNumToRelease   = 1;
    m_fReleaseInterval = 1.0f;
    m_fLifeCycle       = 1.0f;
    m_fSize            = 1.0f;
    m_clrColor         = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
    m_vPosition        = D3DXVECTOR3(0.0f,0.0f,0.0f);
    m_vVelocity        = D3DXVECTOR3(0.0f,0.0f,0.0f);
    m_vGravity         = D3DXVECTOR3(0.0f,0.0f,0.0f);
    m_vWind            = D3DXVECTOR3(0.0f,0.0f,0.0f);
    m_bAirResistence   = true;
    m_fVelocityVar     = 1.0f;
	g_psType		   = DE_PSTYPE_POINT;
	m_vDistance		   = D3DXVECTOR3(1.0f,1.0f,1.0f);
	m_bRelativeY	   = true;
	
	// Classify Point
	CP_FRONT   = 0;
	CP_BACK    = 1;
	CP_ONPLANE = 2;
	// Collision Results
	CR_BOUNCE  = 0;
	CR_STICK   = 1;
	CR_RECYCLE = 2;

	//Guarda se è stato settato il software processing
	if (DECore::D3DDevice->GetSoftwareVertexProcessing())
	{
		//In Software
		vertexProcessingUsage = D3DUSAGE_WRITEONLY | D3DUSAGE_SOFTWAREPROCESSING;
	}
	else
	{
		//In hardware
		vertexProcessingUsage = D3DUSAGE_WRITEONLY;
	}

    // Initialize the particle system
    if( FAILED( hr = Restore() ) )
        return hr;

    // Get max point size
    D3DCAPS9 d3dCaps;
    DECore::D3DDevice->GetDeviceCaps( &d3dCaps );
    m_fMaxPointSize = d3dCaps.MaxPointSize;

    // Check and see if we can change the size of point sprites 
    // in hardware by sending D3DFVF_PSIZE with the FVF.
    if( d3dCaps.FVFCaps & D3DFVFCAPS_PSIZE )	m_bDeviceSupportsPSIZE = true;
    else										m_bDeviceSupportsPSIZE = false;

    return true;
}



//-----------------------------------------------------------------------------
// Name: SetCollisionPlane()
// Desc: 
//-----------------------------------------------------------------------------
DLL_API void DEParticleSystem::SetCollisionPlane( D3DXVECTOR3 vPlaneNormal, D3DXVECTOR3 vPoint, float fBounceFactor, int nCollisionResult )
{
    Plane *pPlane = new Plane;

    pPlane->m_vNormal          = vPlaneNormal;
    pPlane->m_vPoint           = vPoint;
    pPlane->m_fBounceFactor    = fBounceFactor;
    pPlane->m_nCollisionResult = nCollisionResult;

    pPlane->m_pNext = m_pPlanes; // Attach the curent list to it...
    m_pPlanes = pPlane;          // ... and make it the new head.
}


//-----------------------------------------------------------------------------
// Name: Update()
// Desc:
//-----------------------------------------------------------------------------
DLL_API HRESULT DEParticleSystem::Update( FLOAT fElpasedTime )
{
    Particle  *pParticle;
    Particle **ppParticle;
    Plane     *pPlane;
    Plane    **ppPlane;
    D3DXVECTOR3 vOldPosition;

    m_fCurrentTime += fElpasedTime;     // Update our particle system timer...

    ppParticle = &m_pActiveList; // Start at the head of the active list


    while( *ppParticle )
    {
        pParticle = *ppParticle; // Set a pointer to the head

        // Calculate new position
        float fTimePassed  = m_fCurrentTime - pParticle->m_fInitTime;


        if( fTimePassed >= m_fLifeCycle )
        {
            // Time is up, put the particle back on the free list...
            *ppParticle = pParticle->m_pNext;
            pParticle->m_pNext = m_pFreeList;
            m_pFreeList = pParticle;

            --m_dwActiveCount;
        }
        else
		{
            //Update color for alpha value
			//fTimePassed : m_fLifeCycle = x : m_clrColor.a
			D3DXCOLOR m_cTemp = m_clrColor;
			m_cTemp.a = m_clrColor.a - ((fTimePassed * m_clrColor.a )/ m_fLifeCycle);
			pParticle->m_cColor = m_cTemp;

            // Update velocity with respect to Gravity (Constant Accelaration)
            pParticle->m_vCurVel += m_vGravity * fElpasedTime;

            // Update velocity with respect to Wind (Accelaration based on 
            // difference of vectors)
            if( m_bAirResistence == true )
                pParticle->m_vCurVel += (m_vWind - pParticle->m_vCurVel) * fElpasedTime;

            // Finally, update position with respect to velocity
            vOldPosition = pParticle->m_vCurPos;
            pParticle->m_vCurPos += pParticle->m_vCurVel * fElpasedTime;

            //-----------------------------------------------------------------
            // BEGIN Checking the particle against each plane that was set up

            ppPlane = &m_pPlanes; // Set a pointer to the head

            while( *ppPlane )
            {
                pPlane = *ppPlane;
                int result = classifyPoint( &pParticle->m_vCurPos, pPlane );

                if( result == CP_BACK  )
                {
                    if( pPlane->m_nCollisionResult == CR_BOUNCE )
                    {
                        pParticle->m_vCurPos = vOldPosition;

            //-----------------------------------------------------------------
            //
            // The new velocity vector of a particle that is bouncing off
            // a plane is computed as follows:
            //
            // Vn = (N.V) * N
            // Vt = V - Vn
            // Vp = Vt - Kr * Vn
            //
            // Where:
            // 
            // .  = Dot product operation
            // N  = The normal of the plane from which we bounced
            // V  = Velocity vector prior to bounce
            // Vn = Normal force
            // Kr = The coefficient of restitution ( Ex. 1 = Full Bounce, 
            //      0 = Particle Sticks )
            // Vp = New velocity vector after bounce
            //
            //-----------------------------------------------------------------

                        float Kr = pPlane->m_fBounceFactor;

                        D3DXVECTOR3 Vn = D3DXVec3Dot( &pPlane->m_vNormal, 
                                                      &pParticle->m_vCurVel ) * 
                                                      pPlane->m_vNormal;
                        D3DXVECTOR3 Vt = pParticle->m_vCurVel - Vn;
                        D3DXVECTOR3 Vp = Vt - Kr * Vn;

                        pParticle->m_vCurVel = Vp;
                    }
                    else if( pPlane->m_nCollisionResult == CR_RECYCLE )
                    {
                        pParticle->m_fInitTime -= m_fLifeCycle;
                    }

                    else if( pPlane->m_nCollisionResult == CR_STICK )
                    {
                        pParticle->m_vCurPos = vOldPosition;
                        pParticle->m_vCurVel = D3DXVECTOR3(0.0f,0.0f,0.0f);
                    }
                }

                ppPlane = &pPlane->m_pNext;
            }

            // END Plane Checking
            //-----------------------------------------------------------------

            ppParticle = &pParticle->m_pNext;
        }
    }

    //-------------------------------------------------------------------------
    // Emit new particles in accordance to the flow rate...
    // 
    // NOTE: The system operates with a finite number of particles.
    //       New particles will be created until the max amount has
    //       been reached, after that, only old particles that have
    //       been recycled can be reintialized and used again.
    //-------------------------------------------------------------------------

    if( m_fCurrentTime - m_fLastUpdate > m_fReleaseInterval )
    {
        // Reset update timing...
        m_fLastUpdate = m_fCurrentTime;
    
        // Emit new particles at specified flow rate...
        for( DWORD i = 0; i < m_dwNumToRelease; ++i )
        {
            // Do we have any free particles to put back to work?
            if( m_pFreeList )
            {
                // If so, hand over the first free one to be reused.
                pParticle = m_pFreeList;
               // Then make the next free particle in the list next to go!
                m_pFreeList = pParticle->m_pNext;
				
            }
            else
            {
                // There are no free particles to recycle...
                // We'll have to create a new one from scratch!
                if( m_dwActiveCount < m_dwMaxParticles )
                {
                    if( NULL == ( pParticle = new Particle ) )
                        return E_OUTOFMEMORY;
                }
            }

            if( m_dwActiveCount < m_dwMaxParticles )
            {
                pParticle->m_pNext = m_pActiveList; // Make it the new head...
                m_pActiveList = pParticle;
                
                // Set the attributes for our new particle...
                pParticle->m_vCurVel = m_vVelocity;

                if( m_fVelocityVar != 0.0f )
                {
					D3DXVECTOR3 vRandomVec = DEUtility::GetRandomVector(-1.0f, 1.0f);
                    pParticle->m_vCurVel += vRandomVec * m_fVelocityVar;
                }

                pParticle->m_fInitTime  = m_fCurrentTime;
                
				D3DXVECTOR3 m_vTypePosition;
				//Imposta il tipo di emettitore
				if (g_psType == DE_PSTYPE_POINT)
				{
					m_vTypePosition.x = m_vPosition.x;
					m_vTypePosition.y = m_vPosition.y;
					m_vTypePosition.z = m_vPosition.z;
				}
				if (g_psType == DE_PSTYPE_CUBE)
				{
					m_vTypePosition.x = m_vPosition.x + DEUtility::GetRandomMinMax(-(m_vDistance.x /2), (m_vDistance.x /2));
					if (m_bRelativeY)	m_vTypePosition.y = m_vPosition.y + DEUtility::GetRandomMinMax(-(m_vDistance.y /2), (m_vDistance.y /2));
					else				m_vTypePosition.y = m_vPosition.y + DEUtility::GetRandomMinMax(0, (m_vDistance.y));
					m_vTypePosition.z = m_vPosition.z + DEUtility::GetRandomMinMax(-(m_vDistance.z /2), (m_vDistance.z /2));
				}
				if (g_psType == DE_PSTYPE_CIRCLE)
				{
					float t = DEUtility::GetRandomMinMax(-D3DX_PI,D3DX_PI);
					m_vTypePosition.x = m_vPosition.x + (float)sinf(t)*(m_vDistance.x);
					if (m_bRelativeY)	m_vTypePosition.y = m_vPosition.y + DEUtility::GetRandomMinMax(-(m_vDistance.y /2), (m_vDistance.y /2));
					else				m_vTypePosition.y = m_vPosition.y + DEUtility::GetRandomMinMax(0, (m_vDistance.y));
					m_vTypePosition.z = m_vPosition.z + (float)cosf(t)*(m_vDistance.z);
				}

				//Setta il vettore posizione
				pParticle->m_vCurPos    = m_vTypePosition;
               
                ++m_dwActiveCount;
            }
        }
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RestartParticleSystem()
// Desc:
//-----------------------------------------------------------------------------
DLL_API void DEParticleSystem::RestartParticleSystem( void )
{
	Particle  *pParticle;
	Particle **ppParticle;

	ppParticle = &m_pActiveList; // Start at the head of the active list

	while( *ppParticle )
	{
		pParticle = *ppParticle; // Set a pointer to the head

		// Put the particle back on the free list...
		*ppParticle = pParticle->m_pNext;
		pParticle->m_pNext = m_pFreeList;
		m_pFreeList = pParticle;

		--m_dwActiveCount;
	}
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the particle system using pointsprites loaded in a vertex 
//       buffer.
//
// Note: D3DLOCK_DISCARD:
//
//       The application overwrites, with a write-only operation, the entire 
//       index buffer. This enables Direct3D to return a pointer to a new 
//       memory area so that the dynamic memory access (DMA) and rendering 
//       from the old area do not stall.
//
//       D3DLOCK_NOOVERWRITE:
//
//       Indicates that no vertices that were referred to in drawing calls 
//       since the start of the frame or the last lock without this flag will 
//       be modified during the lock. This can enable optimizations when the 
//       application is appending data only to the vertex buffer. 
//-----------------------------------------------------------------------------
DLL_API HRESULT DEParticleSystem::Render(  )
{
	//Setta la matrice a 0
	D3DXMATRIX	finalMatrix;
	D3DXMatrixIdentity(&finalMatrix);
	DECore::D3DDevice->SetTransform(D3DTS_WORLD,&finalMatrix );
	
	//Applica i renderstates
	SetDefaultRenderStates();

	//PointSprites
	DECore::D3DDevice->SetTexture(0, DETextureManager::GetTexture(textureID).Texture_Pointer);
	DECore::D3DDevice->SetRenderState( D3DRS_POINTSIZE,     DEUtility::FtoDW(m_fSize) );

    HRESULT hr;  
	Particle    *pParticle = m_pActiveList;
    PointVertex *pVertices;
    DWORD        dwNumParticlesToRender = 0;

    // Lock the vertex buffer.  We fill the vertex buffer in small
    // chunks, using D3DLOCK_NOOVERWRITE.  When we are done filling
    // each chunk, we call DrawPrim, and lock the next chunk.  When
    // we run out of space in the vertex buffer, we start over at
    // the beginning, using D3DLOCK_DISCARD.

    // Move the offset forward so we can fill the next chunk of the vertex buffer
    m_dwVBOffset += m_dwFlush;

    // If we're about to overflow the buffer, reset the offset counter back to 0
    if( m_dwVBOffset >= m_dwDiscard )
        m_dwVBOffset = 0;

    if( FAILED( hr = m_pVB->Lock( m_dwVBOffset * sizeof(PointVertex), // Offset to lock
                                  m_dwFlush * sizeof(PointVertex),    // Size to lock
                                  (void **) &pVertices, 
                                  m_dwVBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD)))
    {
        return hr;
    }

    // Render each particle
    while( pParticle )
    {
		//Setta il colore di trasparenza della texture in base al valore passato
		//come argomento a del colore di alpha

		D3DXVECTOR3 vPos(pParticle->m_vCurPos);
        D3DXVECTOR3 vVel(pParticle->m_vCurVel);
		D3DXCOLOR   cCol(pParticle->m_cColor);

        pVertices->posit = vPos;
        pVertices->color = cCol;
        pVertices++;

        if( ++dwNumParticlesToRender == m_dwFlush )
        {
            // Done filling this chunk of the vertex buffer.  Lets unlock and
            // draw this portion so we can begin filling the next chunk.

			
			m_pVB->Unlock();

			DECore::D3DDevice->SetStreamSource( 0, m_pVB, 0, sizeof(PointVertex) );
			DECore::D3DDevice->SetFVF( PointVertex::FVF_Flags );

            if( FAILED(hr = DECore::D3DDevice->DrawPrimitive( D3DPT_POINTLIST, 
                m_dwVBOffset, dwNumParticlesToRender)))
            {
                return hr;
            }

            // Lock the next chunk of the vertex buffer.  If we are at the 
            // end of the vertex buffer, DISCARD the vertex buffer and start
            // at the beginning.  Otherwise, specify NOOVERWRITE, so we can
            // continue filling the VB while the previous chunk is drawing.
            m_dwVBOffset += m_dwFlush;

            // If we're about to overflow the buffer, reset the offset counter back to 0
            if( m_dwVBOffset >= m_dwDiscard )
                m_dwVBOffset = 0;

            if( FAILED( hr = m_pVB->Lock( 
                m_dwVBOffset * sizeof(PointVertex), // Offset to lock
                m_dwFlush    * sizeof(PointVertex), // Size to lock
                (void **) &pVertices, 
                m_dwVBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD)))
            {
                return hr;
            }

            dwNumParticlesToRender = 0;
        }

        pParticle = pParticle->m_pNext;
    }

    // Unlock the vertex buffer
    m_pVB->Unlock();

    // Render any remaining particles
    if( dwNumParticlesToRender )
    {
		DECore::D3DDevice->SetStreamSource( 0, m_pVB, 0,sizeof(PointVertex) );
		DECore::D3DDevice->SetFVF( PointVertex::FVF_Flags );

        if(FAILED(hr = DECore::D3DDevice->DrawPrimitive( D3DPT_POINTLIST, m_dwVBOffset, 
                                                  dwNumParticlesToRender )))
            return hr;
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RestoreDECore::D3DDeviceObjects()
// Desc:
// Note: If your application performs its own vertex processing and passes 
//       transformed, lit, and clipped vertices to rendering methods, then the 
//       application can directly write vertices to a vertex buffer allocated  
//       in driver-optimal memory. This technique prevents a redundant copy  
//       operation later. Note that this technique will not work well if your 
//       application reads data back from a vertex buffer, because read 
//       operations done by the host from driver-optimal memory can be very 
//       slow. Therefore, if your application needs to read during processing 
//       or writes data to the buffer erratically, a system-memory vertex 
//       buffer is a better choice.
//
//       When using the Direct3D vertex processing features (by passing 
//       untransformed vertices to vertex-buffer rendering methods), processing 
//       can occur in either hardware or software depending on the DECore::D3DDevice type  
//       and DECore::D3DDevice creation flags. It is recommended that vertex buffers be 
//       allocated in pool D3DPOOL_DEFAULT for best performance in virtually 
//       all cases. When a DECore::D3DDevice is using hardware vertex processing, there 
//       is a number of additional optimizations that may be done based on the 
///      flags  D3DUSAGE_DYNAMIC and D3DUSAGE_WRITEONLY. 
//       See IDirect3DDECore::D3DDevice8::CreateVertexBuffer for more information on 
//       using these flags.
//-----------------------------------------------------------------------------
DLL_API bool DEParticleSystem::Restore()
{
    HRESULT hr;

    // Create a vertex buffer for the particle system.  The size of this buffer
    // does not relate to the number of particles that exist.  Rather, the
    // buffer is used as a communication channel with the DECore::D3DDevice. we fill in 
    // a chunck, and tell the DECore::D3DDevice to draw. While the DECore::D3DDevice is drawing, we
    // fill in the next chunck using NOOVERWRITE. We continue doing this until 
    // we run out of vertex buffer space, and are forced to DISCARD the buffer
    // and start over at the beginning.

    if( FAILED( hr = DECore::D3DDevice->CreateVertexBuffer( m_dwDiscard * sizeof(PointVertex), D3DUSAGE_DYNAMIC | vertexProcessingUsage | D3DUSAGE_POINTS, PointVertex::FVF_Flags, D3DPOOL_DEFAULT, &m_pVB,NULL )))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Name: InvalidateDECore::D3DDeviceObjects()
// Desc:
//-----------------------------------------------------------------------------
DLL_API bool DEParticleSystem::Invalidate()
{
	try
	{
		if( m_pVB != NULL ) m_pVB->Release();
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}



DLL_API void DEParticleSystem::Execute(int mode)
{
	//Play
	if (mode == 0)
	{
		g_dCurTime     = timeGetTime();
		g_fElpasedTime = (float)((g_dCurTime - g_dLastTime) * 0.001);
		g_dLastTime    = g_dCurTime;
		Update( (float)g_fElpasedTime );
		Render();
	}
	//Pause
	if (mode == 1)
	{
		//g_dCurTime     = timeGetTime();
		//g_fElpasedTime = (float)((g_dCurTime - g_dLastTime) * 0.001);
		//g_dLastTime    = g_dCurTime;*/
		g_fElpasedTime = 0;
		g_dLastTime = 0;
		Update( (float)g_fElpasedTime );
		Render();
	}
	//Stop
	if (mode == 2)
	{
		RestartParticleSystem();
		//Update( (float)g_fElpasedTime );
		//Render();
	}

}

DLL_API char *DEParticleSystem::GetName()
{
	return name;
}

DLL_API char *DEParticleSystem::GetTextureName()
{
	return textureFile;
}

DLL_API bool DEParticleSystem::Load(LPCTSTR file)
{
	HRESULT hr;
	STRUCT_DE_PARTICLES particleProperties;
	LPD3DXEFFECT tempMaterial = NULL;
	char namePath[MAX_PATH];
	strcpy(namePath, DEUtility::GetCurrentPath());
	strcat(namePath, file);
	strcpy(fileName, file);


	hr = D3DXCreateEffectFromFile(DECore::D3DDevice, namePath, NULL,NULL,D3DXSHADER_USE_LEGACY_D3DX9_31_DLL,NULL,&tempMaterial, NULL );
	if (hr==D3D_OK)
	{
		//** PROPERTIES **
		//Texture
		LPCSTR Particles_Texture;
		hr = tempMaterial->GetString("Particle_Texture", &Particles_Texture);
		if (hr!=D3D_OK)	strcpy(particleProperties.Particle_TextureFile, "No Name");
		else			strcpy(particleProperties.Particle_TextureFile, Particles_Texture);
		//Max
		FLOAT Particles_MaxParticles = 0.0f;
		hr = tempMaterial->GetFloat("Particle_MaxParticles", &Particles_MaxParticles);
		if (hr!=D3D_OK)	particleProperties.Particle_Max = 0.5f;
		else			particleProperties.Particle_Max = Particles_MaxParticles;
		//NumToRelease
		FLOAT Particles_NumToRelease = 0.0f;
		hr = tempMaterial->GetFloat("Particle_NumToRelease", &Particles_NumToRelease);
		if (hr!=D3D_OK)	particleProperties.Particle_Release = 0.5f;
		else			particleProperties.Particle_Release = Particles_NumToRelease;
		//NumToRelease
		FLOAT Particles_ReleaseInterval = 0.0f;
		hr = tempMaterial->GetFloat("Particle_ReleaseInterval", &Particles_ReleaseInterval);
		if (hr!=D3D_OK)	particleProperties.Particle_Interval = 0.5f;
		else			particleProperties.Particle_Interval = Particles_ReleaseInterval;
		//LifeCycle
		FLOAT Particles_LifeCycle = 0.0f;
		hr = tempMaterial->GetFloat("Particle_LifeCycle", &Particles_LifeCycle);
		if (hr!=D3D_OK)	particleProperties.Particle_Life = 0.5f;
		else			particleProperties.Particle_Life = Particles_LifeCycle;
		//Size
		FLOAT Particles_Size = 0.0f;
		hr = tempMaterial->GetFloat("Particle_Size", &Particles_Size);
		if (hr!=D3D_OK)	particleProperties.Particle_Size = 0.5f;
		else			particleProperties.Particle_Size = Particles_Size;
		//VelocityVar
		FLOAT Particles_VelocityVar = 0.0f;
		hr = tempMaterial->GetFloat("Particle_VelocityVar", &Particles_VelocityVar);
		if (hr!=D3D_OK)	particleProperties.Particle_VelocityVar = 0.5f;
		else			particleProperties.Particle_VelocityVar = Particles_VelocityVar;
		//Resistence
		BOOL Particles_AirResistence;
		hr = tempMaterial->GetBool("Particle_AirResistence", &Particles_AirResistence);
		if (hr!=D3D_OK)
		{
			particleProperties.Particle_Resistence = false;
		}
		else
		{
			if (Particles_AirResistence == 0)	particleProperties.Particle_Resistence = false;
			else 								particleProperties.Particle_Resistence = true;
		}			
		//Color
		float *Particles_Color;
		Particles_Color = new float[4];
		hr = tempMaterial->GetFloatArray("Particle_Color", Particles_Color, 3);
		if (hr!=D3D_OK)	particleProperties.Particle_Color = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
		else			particleProperties.Particle_Color = D3DXCOLOR(Particles_Color[0], Particles_Color[1], Particles_Color[2], Particles_Color[3]);
		delete Particles_Color;
		//Position
		float *Particles_Position;
		Particles_Position = new float[3];
		hr = tempMaterial->GetFloatArray("Particle_Position", Particles_Position, 3);
		if (hr!=D3D_OK)	particleProperties.Particle_Position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		else			particleProperties.Particle_Position = D3DXVECTOR3(Particles_Position[0], Particles_Position[1], Particles_Position[2]);
		delete Particles_Position;
		//Wind
		float *Particles_Wind;
		Particles_Wind = new float[3];
		hr = tempMaterial->GetFloatArray("Particle_Wind", Particles_Wind, 3);
		if (hr!=D3D_OK)	particleProperties.Particle_Wind = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		else			particleProperties.Particle_Wind = D3DXVECTOR3(Particles_Wind[0], Particles_Wind[1], Particles_Wind[2]);
		delete Particles_Wind;
		//Velocity
		float *Particles_Velocity;
		Particles_Velocity = new float[3];
		hr = tempMaterial->GetFloatArray("Particle_Velocity", Particles_Velocity, 3);
		if (hr!=D3D_OK)	particleProperties.Particle_Velocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		else			particleProperties.Particle_Velocity = D3DXVECTOR3(Particles_Velocity[0], Particles_Velocity[1], Particles_Velocity[2]);
		delete Particles_Velocity;
		//Velocity
		float *Particles_Gravity;
		Particles_Gravity = new float[3];
		hr = tempMaterial->GetFloatArray("Particle_Gravity", Particles_Gravity, 3);
		if (hr!=D3D_OK)	particleProperties.Particle_Gravity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		else			particleProperties.Particle_Gravity = D3DXVECTOR3(Particles_Gravity[0], Particles_Gravity[1], Particles_Gravity[2]);
		delete Particles_Gravity;
		//Type
		INT Particles_Type;
		hr = tempMaterial->GetInt("Particle_Type", &Particles_Type);
		if (hr!=D3D_OK)
		{
			particleProperties.Particle_Type = DE_PSTYPE_POINT;
		}
		else	
		{
			if (Particles_Type == 0)	particleProperties.Particle_Type = DE_PSTYPE_POINT;
			if (Particles_Type == 1)	particleProperties.Particle_Type = DE_PSTYPE_CUBE;
			if (Particles_Type == 2)	particleProperties.Particle_Type = DE_PSTYPE_CIRCLE;
		}
		//Distance
		float *Particles_Distance;
		Particles_Distance = new float[3];
		hr = tempMaterial->GetFloatArray("Particle_Distance", Particles_Distance, 3);
		if (hr!=D3D_OK)	particleProperties.Particle_Distance = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		else			particleProperties.Particle_Distance = D3DXVECTOR3(Particles_Distance[0], Particles_Distance[1], Particles_Distance[2]);
		delete Particles_Distance;
		//RelativeY
		BOOL Particles_RelativeY;
		hr = tempMaterial->GetBool("Particle_RelativeY", &Particles_RelativeY);
		if (hr!=D3D_OK)
		{
			particleProperties.Particle_RelativeY = false;
		}
		else
		{
			if (Particles_RelativeY == 0)	particleProperties.Particle_RelativeY = false;
			else 							particleProperties.Particle_RelativeY = true;
		}

		SetTexture((char *)particleProperties.Particle_TextureFile);
		SetMaxParticles((DWORD)particleProperties.Particle_Max);
		SetNumToRelease((DWORD)particleProperties.Particle_Release);
		SetReleaseInterval(particleProperties.Particle_Interval);
		SetLifeCycle(particleProperties.Particle_Life);
		SetSize(particleProperties.Particle_Size);
		SetVelocityVar(particleProperties.Particle_VelocityVar);
		SetAirResistence(particleProperties.Particle_Resistence);
		SetColor(particleProperties.Particle_Color);
		SetPosition(particleProperties.Particle_Position);
		SetWind(particleProperties.Particle_Wind);
		SetVelocity(particleProperties.Particle_Velocity);
		SetGravity(particleProperties.Particle_Gravity);
		SetType(particleProperties.Particle_Type);
		SetDistance(particleProperties.Particle_Distance);
		SetRelativeY(particleProperties.Particle_RelativeY);

		//Pone a zero la struttura caricate(causerebbe errore)
		ZeroMemory(&particleProperties, sizeof(STRUCT_DE_PARTICLES));

	}//*/
	return true;
}

DLL_API bool DEParticleSystem::Reload()
{
	return Load(fileName);
}


DLL_API bool DEParticleSystem::SetTexture( char *chTexFile )
{
	//Crea le texture
	textureID = DETextureManager::CreateTexture(chTexFile, NULL, false);
	if (textureID < 0)
	{
		DELog::LogWarning("<li>D3DXCreateTextureFromFileEx(%s) - Object may appear without texture", chTexFile);
		return false;
	}
	else
	{
		//Crea la stringa completa
		memset(textureFile, 0, sizeof(char) * MAX_PATH);
		strcpy(textureFile, chTexFile);
		return true;
	}
	return false;
}


DLL_API bool DEParticleSystem::New(LPCTSTR newFileNameImage)
{

	//Setta il sistema particellare con la texture caricata e valori standard
	SetTexture( (char *)newFileNameImage );
	SetMaxParticles( 300 );
	SetNumToRelease( 3 );
	SetReleaseInterval( 0.05f );
	SetLifeCycle( 1.0f );
	SetSize( 5.0f );
	SetColor( D3DXCOLOR( 0.7f, 0.3f, 1.0f, 0.85f ));
	SetPosition( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
	SetVelocity( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
	SetGravity( D3DXVECTOR3( 0.0f,  0.0f, 0.0f ) );
	SetWind( D3DXVECTOR3( 0.0f, 0.0f, 20.0f ) );
	SetVelocityVar( 0.9f );
	SetAirResistence(true);
	SetType(DE_PSTYPE_POINT);
	SetDistance(D3DXVECTOR3( 1.0f, 1.0f, 1.0f ));
	SetRelativeY(true);

	return true;
}


DLL_API bool DEParticleSystem::Save(LPCTSTR scriptFileName)
{

	HANDLE hFile=CreateFile(scriptFileName,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,NULL,NULL);
	if (hFile==INVALID_HANDLE_VALUE)
	{
		return false;
	}
	else
	{
		STRUCT_DE_PARTICLES particleProperties;
		strcpy(particleProperties.Particle_TextureFile, textureFile);
		particleProperties.Particle_Max = (float)GetMaxParticles();
		particleProperties.Particle_Release = (float)GetNumToRelease();
		particleProperties.Particle_Interval = GetReleaseInterval();
		particleProperties.Particle_Life = GetLifeCycle();
		particleProperties.Particle_Size = GetSize();
		particleProperties.Particle_VelocityVar = GetVelocityVar();
		particleProperties.Particle_Resistence =  GetAirResistence();
		particleProperties.Particle_Color = GetColor();
		particleProperties.Particle_Position = GetPosition();
		particleProperties.Particle_Wind = GetWind();
		particleProperties.Particle_Velocity = GetVelocity();
		particleProperties.Particle_Gravity = GetGravity();
		particleProperties.Particle_Type = GetType();
		particleProperties.Particle_Distance = GetDistance();
		particleProperties.Particle_RelativeY = GetRelativeY();

		unsigned long rwBytes;
		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		WriteFile(hFile, &particleProperties, sizeof(STRUCT_DE_PARTICLES), &rwBytes, NULL);
		CloseHandle(hFile);

		//Pone a zero la struttura caricate(causerebbe errore)
		ZeroMemory(&particleProperties, sizeof(STRUCT_DE_PARTICLES));

		//Nuovo file corrente senza estensione ne percorso(lo ricava dal file passato
		memset(name, 0, sizeof(char) * MAX_PATH);
		char *pdest1;
		int result1=0;
		pdest1 = strrchr( (char *)scriptFileName, '.' );
		result1 = pdest1 - scriptFileName + 1;
		strncpy(name, scriptFileName, result1-1 );	
		char *reversed;
		reversed = (char *)malloc(MAX_PATH);
		memset(reversed, 0, sizeof(char) * MAX_PATH);
		reversed = _strrev( _strdup( name ) );
		char *pdest2;
		int result2=0;
		pdest2 = strrchr( reversed, '\\' );
		result2 = pdest2 - reversed + 1;
		memset(name, 0, sizeof(char) * MAX_PATH);
		strncpy(name, reversed, result2-1 );	
		char *reversed2;
		reversed2 = (char *)malloc(MAX_PATH);
		memset(reversed2, 0, sizeof(char) * MAX_PATH);
		reversed2 = _strrev( _strdup( name ) );
		memset(name, 0, sizeof(char) * MAX_PATH);
		strcpy(name, reversed2);
		free(reversed);
		free(reversed2);

	}

	return true;
}


DLL_API void DEParticleSystem::SetDefaultRenderStates()
{
	DECore::D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	DECore::D3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
	
	DECore::D3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );			
		DECore::D3DDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB(96,96,96));
	
	DECore::D3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE);
	
	DECore::D3DDevice->SetRenderState( D3DRS_ZWRITEENABLE,FALSE);
		DECore::D3DDevice->SetRenderState( D3DRS_ZFUNC,  D3DCMP_LESSEQUAL  );
	
	DECore::D3DDevice->SetRenderState( D3DRS_NORMALIZENORMALS, FALSE);
	DECore::D3DDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT);
		
	DECore::D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		DECore::D3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		DECore::D3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	DECore::D3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE );
		//DECore::D3DDevice->SetRenderState( D3DRS_ALPHAREF,		0x00000000 );
		//DECore::D3DDevice->SetRenderState( D3DRS_ALPHAFUNC,	D3DCMP_ALWAYS );

	DECore::D3DDevice->SetRenderState( D3DRS_COLORVERTEX, FALSE);
			//DECore::D3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
			
	DECore::D3DDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE );
			//DECore::D3DDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2);
			
	DECore::D3DDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
	DECore::D3DDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1);


	DECore::D3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE);
		/*DECore::D3DDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
		DECore::D3DDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
		DECore::D3DDevice->SetRenderState(D3DRS_FOGCOLOR, 0);
		DECore::D3DDevice->SetRenderState(D3DRS_FOGSTART, (DWORD)0.0);
		DECore::D3DDevice->SetRenderState(D3DRS_FOGEND,   (DWORD)1.0);
		DECore::D3DDevice->SetRenderState(D3DRS_FOGDENSITY, (DWORD)1.0);*/

	DECore::D3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);

		//DECore::D3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		//DECore::D3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);	
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT );
		DECore::D3DDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
		DECore::D3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );

    DECore::D3DDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
		DECore::D3DDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
		DECore::D3DDevice->SetRenderState( D3DRS_POINTSIZE_MIN, DEUtility::FtoDW(100.0f) );
		DECore::D3DDevice->SetRenderState( D3DRS_POINTSCALE_A,  DEUtility::FtoDW(10.0f) );
		DECore::D3DDevice->SetRenderState( D3DRS_POINTSCALE_B,  DEUtility::FtoDW(10.0f) );
		DECore::D3DDevice->SetRenderState( D3DRS_POINTSCALE_C,  DEUtility::FtoDW(10.0f) );
}

DLL_API bool DEParticleSystem::SetTag(LPCSTR newTag)
{
	try
	{
		mainTag = newTag;
		return true;
	}
	catch(...)
	{
		return false;
	}
	return false;
}

DLL_API LPCSTR DEParticleSystem::GetTag()
{
	LPCSTR	nullTag="NoTag";
	try
	{
		if (mainTag!=NULL)	return mainTag;
		else				return nullTag;
	}
	catch(...)
	{
		return nullTag;
	}
	return nullTag;
}

