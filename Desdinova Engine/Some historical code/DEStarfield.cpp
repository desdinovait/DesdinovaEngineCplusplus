#include "DEGeneralIncludes.h"
#include "DEStarfield.h"


DLL_API void DEStarfield::Create()
{

	D3DXCreateEffectFromFile( DECore::D3DDevice, "d://Starfield.fx", NULL, NULL, D3DXFX_NOT_CLONEABLE,  NULL, &g_pEffect, NULL );


    // Create the vertex declaration
     DECore::D3DDevice->CreateVertexDeclaration( VertexElements, &g_pVertexDecl );;

    // Create the vertex buffer for the triangles / quads
     DECore::D3DDevice->CreateVertexBuffer( NUM_STAR_VERTS * sizeof(Vertex), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &g_pVBTriangles, NULL );

	// Generate the Star Field.
	for ( int i = 0; i < MAX_STARS; i++ )
	{
		g_Stars[ i ].GenNewPos();
		g_Stars[ i ].CalcNewColor();
		g_Stars[ i ].CalcNewVel();
	}

}


DLL_API void DEStarfield::Render(float time)
{
    // Update the effect's variables.  Instead of using strings, it would 
    // be more efficient to cache a handle to the parameter by calling 
    // ID3DXEffect::GetParameterByName


	D3DXMATRIX m_matProj;
	D3DXMATRIX m_matView;
	D3DXMATRIX mWorldViewProjection;
	D3DXMATRIX mWorld;
	D3DXMATRIX mWorld2;
	D3DXMatrixIdentity( &mWorld2 );
	float fTime = 0.0f;

	DECore::D3DDevice->GetTransform(D3DTS_PROJECTION,&m_matProj);
	DECore::D3DDevice->GetTransform(D3DTS_VIEW,&m_matView);
	DECore::D3DDevice->GetTransform(D3DTS_WORLD,&mWorld);
	mWorldViewProjection = m_matView * m_matProj;


    g_pEffect->SetMatrix( "g_mWorldViewProjection", &mWorldViewProjection ) ;
    g_pEffect->SetMatrix( "g_mWorld", &mWorld2 ) ;
    g_pEffect->SetFloat( "g_fTime", (float)fTime ) ;

	DECore::D3DDevice->SetStreamSource( 0, g_pVBTriangles, 0, sizeof( Vertex ) ) ;
    DECore::D3DDevice->SetVertexDeclaration( g_pVertexDecl ) ;

	g_pEffect->SetTexture( "g_MeshTexture", NULL );

	// No depth reads/writes.
	DECore::D3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	DECore::D3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	// Update all the Stars (outside of the lock).
	for ( int i = 0; i < g_iNumStarsToDisplay; i++ )
	{
		if ( g_Stars[ i ].m_vCurPos.z >= g_fStarDist )
		{
			g_Stars[ i ].GenNewPos();
			D3DXVec3TransformCoord( &g_Stars[ i ].m_vWorldPos, &g_Stars[ i ].m_vCurPos, &mWorld );
			g_Stars[ i ].m_vWorldLastPos = g_Stars[ i ].m_vWorldPos;
			g_Stars[ i ].CalcNewVel();
		}
		else
		{
			g_Stars[ i ].m_vCurPos.z += time * g_Stars[ i ].m_vVel.z;

			// Transform the star position into world space.
			D3DXVec3TransformCoord ( &g_Stars[ i ].m_vWorldPos, &g_Stars[ i ].m_vCurPos, &mWorld );
		}

		// Calculate the new color.
		g_Stars[ i ].CalcNewColor();

		// Calculate a new update delta.
		g_Stars[ i ].m_fUpdateDelta = (double)RandFloat( 0.0f, g_fStreakDelta );
	}

	Vertex* pVertex = NULL;
	g_pVBTriangles->Lock( 0, 0, (void**) &pVertex, 0 );

	D3DXVECTOR3 vDif;

	// Update the Star Streaks Vertex Buffer.
	for ( int i = 0; i < g_iNumStarsToDisplay; i++ )
	{
		pVertex->Position = g_Stars[ i ].m_vWorldPos;
		pVertex->Diffuse = g_Stars[ i ].m_Color;

		pVertex++;

		pVertex->Position = g_Stars[ i ].m_vWorldLastPos;
		pVertex->Diffuse = 0xFF888888;

		pVertex++;

		vDif = g_Stars[ i ].m_vWorldLastPos - g_Stars[ i ].m_vWorldPos;

		// Update the last position if it's time to, or FORCE an update if the streak is too long.
		if ( g_Stars[ i ].m_fNextUpdate <= fTime ||
				D3DXVec3Length( &vDif ) > g_fMaxStreakLen )
		{
			g_Stars[ i ].m_fNextUpdate = fTime + g_Stars[ i ].m_fUpdateDelta;
			g_Stars[ i ].m_vWorldLastPos = g_Stars[ i ].m_vWorldPos;
		}
	}

	g_pVBTriangles->Unlock();

    UINT NumPasses;
    g_pEffect->Begin( &NumPasses, 0 );

	// NOTE: Drawing more stars than needed.
    for( UINT iPass=0; iPass < NumPasses; iPass++ )
    {
        g_pEffect->BeginPass( iPass ) ;

		DECore::D3DDevice->DrawPrimitive( D3DPT_POINTLIST, 0, g_iNumStarsToDisplay * 2 );

        g_pEffect->EndPass() ;
    }

	// Draw the Streaks.
	for( UINT iPass=0; iPass < NumPasses; iPass++ )
    {
        g_pEffect->BeginPass( iPass );

		DECore::D3DDevice->DrawPrimitive( D3DPT_LINELIST, 0, g_iNumStarsToDisplay );

        g_pEffect->EndPass() ;
    }

    g_pEffect->End();

}