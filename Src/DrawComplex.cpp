/*=============================================================================
	DrawComplex.cpp: Unreal XOpenGL DrawComplexSurface routines.
	Used for BSP drawing.

	Copyright 2014-2017 Oldunreal

    Todo:
    * implement proper usage of persistent buffers.

	Revision history:
		* Created by Smirftsch

=============================================================================*/

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "XOpenGLDrv.h"
#include "XOpenGL.h"

enum DrawComplexTexCoordsIndices
{
	DIFFUSE_COORDS,
	LIGHTMAP_COORDS,
	FOGMAP_COORDS,
	DETAIL_COORDS,
	MACRO_COORDS,
	BUMPMAP_COORDS,
	ENVIROMAP_COORDS,
	DIFFUSE_INFO,
	MACRO_INFO,
	BUMPMAP_INFO,
	X_AXIS,
	Y_AXIS,
	Z_AXIS,
	DRAWCOLOR
};

UXOpenGLRenderDevice::DrawComplexShaderDrawParams* UXOpenGLRenderDevice::DrawComplexGetDrawParamsRef()
{
	return UsingShaderDrawParameters ? 
		&reinterpret_cast<DrawComplexShaderDrawParams*>(DrawComplexSSBORange.Buffer)[DrawComplexBufferData.Index * MAX_DRAWCOMPLEX_BATCH + DrawComplexMultiDrawCount] :
		&DrawComplexDrawParams;
}

inline glm::vec4 FPlaneToVec4(FPlane Plane)
{
	return glm::vec4(Plane.X, Plane.Y, Plane.Z, Plane.W);
}

void UXOpenGLRenderDevice::DrawComplexSurface(FSceneNode* Frame, FSurfaceInfo& Surface, FSurfaceFacet& Facet)
{
	guard(UXOpenGLRenderDevice::DrawComplexSurface);
	check(Surface.Texture);

	if(Frame->Recursion > MAX_FRAME_RECURSION || NoDrawComplexSurface)
		return;

	clockFast(Stats.ComplexCycles);

	//Draw polygons
	SetProgram(ComplexSurfaceSinglePass_Prog);

	const DWORD NextPolyFlags = SetFlags(Surface.PolyFlags);
	FCachedTexture* Bind;
	
	// always draw immediately in UED
	if (GIsEditor ||
		// Check if the blending mode will change
		WillItBlend(DrawComplexDrawParams.PolyFlags(), NextPolyFlags) ||
		// Check if the surface texture will change	
		WillTextureChange(0, *Surface.Texture, NextPolyFlags, Bind))
	{
		//debugf(TEXT("Polyswitch %08x => %08x"), DrawComplexBufferData.PolyFlags, NextPolyFlags);
		if (DrawComplexBufferData.IndexOffset > 0)
		{
			DrawComplexVertsSinglePass(DrawComplexBufferData);
			WaitBuffer(DrawComplexSinglePassRange, DrawComplexBufferData.Index);
		}
		
		SetBlend(NextPolyFlags, false);
	}

	DrawComplexDrawParams.PolyFlags() = NextPolyFlags;
	DrawComplexDrawParams.DrawFlags() = DF_DiffuseTexture;

	// Editor Support.
	if (GIsEditor)
	{		
		if (HitTesting())
		{
			DrawComplexDrawParams.TexCoords[DRAWCOLOR] = FPlaneToVec4(HitColor);
			DrawComplexDrawParams.HitTesting() = 1;
		}
		else
		{
			DrawComplexDrawParams.HitTesting() = 0;
			if (Surface.PolyFlags & PF_FlatShaded)
			{
				FPlane Flat = Surface.FlatColor.Plane();
				DrawComplexDrawParams.TexCoords[DRAWCOLOR] = FPlaneToVec4(FOpenGLGammaDecompress_sRGB(Flat));
			}
			else
			{
				DrawComplexDrawParams.TexCoords[DRAWCOLOR] = FPlaneToVec4(Surface.FlatColor.Plane());
			}
		}

		if (Frame->Viewport->Actor) // needed? better safe than sorry.
			DrawComplexDrawParams.RendMap() = Frame->Viewport->Actor->RendMap;
	}

	// UDot/VDot
	DrawComplexDrawParams.TexCoords[X_AXIS] = glm::vec4(Facet.MapCoords.XAxis.X, Facet.MapCoords.XAxis.Y, Facet.MapCoords.XAxis.Z, Facet.MapCoords.XAxis | Facet.MapCoords.Origin);
	DrawComplexDrawParams.TexCoords[Y_AXIS] = glm::vec4(Facet.MapCoords.YAxis.X, Facet.MapCoords.YAxis.Y, Facet.MapCoords.YAxis.Z, Facet.MapCoords.YAxis | Facet.MapCoords.Origin);
	DrawComplexDrawParams.TexCoords[Z_AXIS] = glm::vec4(Facet.MapCoords.ZAxis.X, Facet.MapCoords.ZAxis.Y, Facet.MapCoords.ZAxis.Z, Gamma);

	SetTexture(0, *Surface.Texture, DrawComplexDrawParams.PolyFlags(), 0.0, ComplexSurfaceSinglePass_Prog, NORMALTEX);
	DrawComplexDrawParams.TexCoords[DIFFUSE_COORDS] = glm::vec4(TexInfo[0].UMult, TexInfo[0].VMult, TexInfo[0].UPan, TexInfo[0].VPan);
	DrawComplexDrawParams.TexCoords[DIFFUSE_INFO] = glm::vec4(Surface.Texture->Texture->Diffuse, Surface.Texture->Texture->Specular, Surface.Texture->Texture->Alpha, Surface.Texture->Texture->Scale);
	DrawComplexDrawParams.TexNum[0].x = TexInfo[0].TexNum;
	DrawComplexDrawParams.TextureFormat() = Surface.Texture->Texture->Format;

	if (Surface.LightMap) //can not make use of bindless, to many single textures. Determined by Info->Texture.
	{
		DrawComplexDrawParams.DrawFlags() |= DF_LightMap;
		SetTexture(1, *Surface.LightMap, DrawComplexDrawParams.PolyFlags(), -0.5, ComplexSurfaceSinglePass_Prog, LIGHTMAP); //First parameter has to fit the uniform in the fragment shader
		DrawComplexDrawParams.TexCoords[LIGHTMAP_COORDS] = glm::vec4(TexInfo[1].UMult, TexInfo[1].VMult, TexInfo[1].UPan, TexInfo[1].VPan);
		DrawComplexDrawParams.TexNum[0].y = TexInfo[1].TexNum;
	}
	if (Surface.FogMap)
	{
		DrawComplexDrawParams.DrawFlags() |= DF_FogMap;
		SetTexture(2, *Surface.FogMap, PF_AlphaBlend, -0.5, ComplexSurfaceSinglePass_Prog, FOGMAP);
		DrawComplexDrawParams.TexCoords[FOGMAP_COORDS] = glm::vec4(TexInfo[2].UMult, TexInfo[2].VMult, TexInfo[2].UPan, TexInfo[2].VPan);
		DrawComplexDrawParams.TexNum[0].z = TexInfo[2].TexNum;
	}
	if (Surface.DetailTexture && DetailTextures)
	{
		DrawComplexDrawParams.DrawFlags() |= DF_DetailTexture;
		SetTexture(3, *Surface.DetailTexture, DrawComplexDrawParams.PolyFlags(), 0.0, ComplexSurfaceSinglePass_Prog, DETAILTEX);
		DrawComplexDrawParams.TexCoords[DETAIL_COORDS] = glm::vec4(TexInfo[3].UMult, TexInfo[3].VMult, TexInfo[3].UPan, TexInfo[3].VPan);
		DrawComplexDrawParams.TexNum[0].w = TexInfo[3].TexNum;
	}
	if (Surface.MacroTexture && MacroTextures)
	{
		DrawComplexDrawParams.DrawFlags() |= DF_MacroTexture;
		SetTexture(4, *Surface.MacroTexture, DrawComplexDrawParams.PolyFlags(), 0.0, ComplexSurfaceSinglePass_Prog, MACROTEX);
		DrawComplexDrawParams.TexCoords[MACRO_COORDS] = glm::vec4(TexInfo[4].UMult, TexInfo[4].VMult, TexInfo[4].UPan, TexInfo[4].VPan);
		DrawComplexDrawParams.TexCoords[MACRO_INFO] = glm::vec4(Surface.MacroTexture->Texture->Diffuse, Surface.MacroTexture->Texture->Specular, Surface.MacroTexture->Texture->Alpha, Surface.MacroTexture->Texture->Scale);
		DrawComplexDrawParams.TexNum[1].x = TexInfo[4].TexNum;
	}
#if ENGINE_VERSION==227
	if (Surface.BumpMap && BumpMaps)
	{
		DrawComplexDrawParams.DrawFlags() |= DF_BumpMap;
		SetTexture(5, *Surface.BumpMap, DrawComplexDrawParams.PolyFlags(), 0.0, ComplexSurfaceSinglePass_Prog, BUMPMAP);
		DrawComplexDrawParams.TexCoords[BUMPMAP_COORDS] = glm::vec4(TexInfo[5].UMult, TexInfo[5].VMult, TexInfo[5].UPan, TexInfo[5].VPan);
		DrawComplexDrawParams.TexCoords[BUMPMAP_INFO] = glm::vec4(Surface.BumpMap->Texture->Diffuse, Surface.BumpMap->Texture->Specular, Surface.BumpMap->Texture->Alpha, Surface.BumpMap->Texture->Scale);
		DrawComplexDrawParams.TexNum[1].y = TexInfo[5].TexNum;
	}
#else
	FTextureInfo BumpMapInfo;
	if (BumpMaps && Surface.Texture && Surface.Texture->Texture && Surface.Texture->Texture->BumpMap)
	{
# if ENGINE_VERSION==1100
		Surface.Texture->Texture->BumpMap->Lock(BumpMapInfo, Viewport->CurrentTime, 0, this);
# else
		Surface.Texture->Texture->BumpMap->Lock(BumpMapInfo, FTime(), 0, this);
# endif
		DrawComplexDrawParams.DrawFlags() |= DF_BumpMap;
		SetTexture(5, BumpMapInfo, DrawComplexDrawParams.PolyFlags(), 0.0, ComplexSurfaceSinglePass_Prog, BUMPMAP);
		DrawComplexDrawParams.TexCoords[BUMPMAP_COORDS] = glm::vec4(TexInfo[5].UMult, TexInfo[5].VMult, TexInfo[5].UPan, TexInfo[5].VPan);
		DrawComplexDrawParams.TexCoords[BUMPMAP_INFO] = glm::vec4(BumpMapInfo.Texture->Diffuse, BumpMapInfo.Texture->Specular, BumpMapInfo.Texture->Alpha, BumpMapInfo.Texture->Scale);
		DrawComplexDrawParams.TexNum[1].y = TexInfo[5].TexNum;
	}
#endif	
#if ENGINE_VERSION==227
	if (Surface.EnvironmentMap && EnvironmentMaps)
	{
		DrawComplexDrawParams.DrawFlags() |= DF_EnvironmentMap;
		SetTexture(6, *Surface.EnvironmentMap, DrawComplexDrawParams.PolyFlags(), 0.0, ComplexSurfaceSinglePass_Prog, ENVIRONMENTMAP);
		DrawComplexDrawParams.TexCoords[ENVIROMAP_COORDS] = glm::vec4(TexInfo[6].UMult, TexInfo[6].VMult, TexInfo[6].UPan, TexInfo[6].VPan);
		DrawComplexDrawParams.TexNum[1].z = TexInfo[6].TexNum;
	}
#endif

	if (UsingShaderDrawParameters)
		memcpy(DrawComplexGetDrawParamsRef(), &DrawComplexDrawParams, sizeof(DrawComplexDrawParams));
	DrawComplexMultiDrawFacetArray[DrawComplexMultiDrawCount] = DrawComplexMultiDrawVertices;

	INT FacetVertexCount = 0;
	for (FSavedPoly* Poly = Facet.Polys; Poly; Poly = Poly->Next)
	{
		INT NumPts = Poly->NumPts;
		if (NumPts < 3) //Skip invalid polygons,if any?
			continue;

		if (DrawComplexBufferData.IndexOffset >= DRAWCOMPLEX_SIZE - DrawComplexStrideSize * (NumPts - 2) * 3)
		{
			DrawComplexMultiDrawFacetArray[DrawComplexMultiDrawCount++] = FacetVertexCount;
			
			DrawComplexVertsSinglePass(DrawComplexBufferData);
			debugf(TEXT("DrawComplexSurface overflow!"));

			WaitBuffer(DrawComplexSinglePassRange, DrawComplexBufferData.Index);

			if (UsingShaderDrawParameters)
				memcpy(DrawComplexGetDrawParamsRef(), &DrawComplexDrawParams, sizeof(DrawComplexDrawParams));

			// just in case...
			if (DrawComplexStrideSize * (NumPts - 2) * 3 >= DRAWCOMPLEX_SIZE)
			{
				debugf(TEXT("DrawComplexSurface facet too big!"));
				continue;
			}

			FacetVertexCount = 0;
		}				
    			
		FTransform** In = &Poly->Pts[0];
		auto Out = reinterpret_cast<DrawComplexBufferedVert*>(
			&DrawComplexSinglePassRange.Buffer[DrawComplexBufferData.BeginOffset + DrawComplexBufferData.IndexOffset]);
		
		for (INT i = 0; i < NumPts-2; i++)
		{
#if WIN32
			(Out++)->Coords = In[0  ]->Point;
			(Out++)->Coords = In[i+1]->Point;
			(Out++)->Coords = In[i+2]->Point;
#else
			(Out++)->Coords = FPlaneToVec4(In[0    ]->Point);
			(Out++)->Coords = FPlaneToVec4(In[i + 1]->Point);
			(Out++)->Coords = FPlaneToVec4(In[i + 2]->Point);
#endif
		}

		FacetVertexCount += (NumPts - 2) * 3;
		DrawComplexMultiDrawVertices      += (NumPts-2) * 3;		
		DrawComplexBufferData.IndexOffset += (NumPts-2) * 3 * (sizeof(DrawComplexBufferedVert) / sizeof(FLOAT));
	}
	
	DrawComplexMultiDrawVertexCountArray[DrawComplexMultiDrawCount] = FacetVertexCount;
	DrawComplexMultiDrawCount++;
	
	CHECK_GL_ERROR();
#if ENGINE_VERSION!=227
	if(DrawComplexDrawParams.DrawFlags() & DF_BumpMap)
		Surface.Texture->Texture->BumpMap->Unlock(BumpMapInfo);
#endif
	unclockFast(Stats.ComplexCycles);

	unguard;
}

#if ENGINE_VERSION==227

//Draw everything after one pass. This function is called after each internal rendering pass, everything has to be properly indexed before drawing. Used for DrawComplexSurface.
void UXOpenGLRenderDevice::DrawPass(FSceneNode* Frame, INT Pass)
{
	guard(UXOpenGLRenderDevice::DrawPass);
	unguard;
}
#endif

void UXOpenGLRenderDevice::DrawComplexVertsSinglePass(DrawComplexBuffer& BufferData)
{	
	GLuint TotalSize = BufferData.IndexOffset;
    CHECK_GL_ERROR();

	checkSlow(ActiveProgram == ComplexSurfaceSinglePass_Prog);

	// Data
	if (!UsingPersistentBuffersComplex)
	{
		if (UseBufferInvalidation)
			glInvalidateBufferData(DrawComplexVertBuffer);
			
#if defined(__LINUX_ARM__)
		// stijn: we get a 10x perf increase on the pi if we just replace the entire buffer...
		glBufferData(GL_ARRAY_BUFFER, TotalSize * sizeof(FLOAT), DrawComplexSinglePassRange.Buffer, GL_STREAM_DRAW);
#else		
		glBufferSubData(GL_ARRAY_BUFFER, 0, TotalSize * sizeof(FLOAT), DrawComplexSinglePassRange.Buffer);
#endif

		if (UsingShaderDrawParameters)
		{
			if (UseBufferInvalidation)
				glInvalidateBufferData(DrawComplexSSBO);
#if defined(__LINUX_ARM__)
			glBufferData(GL_SHADER_STORAGE_BUFFER, DrawComplexMultiDrawCount * sizeof(DrawComplexShaderParams), DrawComplexSSBORange.Buffer, GL_DYNAMIC_DRAW);
#else		
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, DrawComplexMultiDrawCount * sizeof(DrawComplexShaderDrawParams), DrawComplexSSBORange.Buffer);
#endif
		}
    }

	GLintptr BeginOffset = BufferData.BeginOffset * sizeof(float);
	if (BeginOffset != PrevDrawComplexBeginOffset)
	{
		glVertexAttribPointer  (VERTEX_COORD_ATTRIB, 4 , GL_FLOAT, GL_FALSE, DrawComplexStrideSize, (GLvoid*)(BeginOffset                  ));		
		PrevDrawComplexBeginOffset = BeginOffset;
	}		

	if (!UsingShaderDrawParameters)
	{
		GLuint AllDrawParams[] = { DrawComplexDrawParams.DrawFlags(), DrawComplexDrawParams.TextureFormat(), DrawComplexDrawParams.PolyFlags(), DrawComplexDrawParams.RendMap(), (GIsEditor && HitTesting()) ? 1u : 0u };
		glUniform1uiv(DrawComplexSinglePassDrawParams, 5, AllDrawParams);
		glUniform1uiv(DrawComplexSinglePassTexNum, 8, reinterpret_cast<const GLuint*>(&DrawComplexDrawParams.TexNum[0]));
		glUniform4fv(DrawComplexSinglePassTexCoords, ARRAY_COUNT(DrawComplexDrawParams.TexCoords), reinterpret_cast<const GLfloat*>(DrawComplexDrawParams.TexCoords));
		CHECK_GL_ERROR();
	}

	// Draw
	glMultiDrawArrays(GL_TRIANGLES, DrawComplexMultiDrawFacetArray, DrawComplexMultiDrawVertexCountArray, DrawComplexMultiDrawCount);
    CHECK_GL_ERROR();	

	// reset
	DrawComplexMultiDrawVertices = DrawComplexMultiDrawCount = 0;

	if (UsingPersistentBuffersComplex)
	{		
		LockBuffer(DrawComplexSinglePassRange, DrawComplexBufferData.Index);
		DrawComplexBufferData.Index = (DrawComplexBufferData.Index + 1) % NUMBUFFERS;
		CHECK_GL_ERROR();
	}

	DrawComplexBufferData.BeginOffset = DrawComplexBufferData.Index * DRAWCOMPLEX_SIZE;
	DrawComplexBufferData.IndexOffset = 0;
	BindlessFail = false;

	CHECK_GL_ERROR();
}

//
// Program Switching
//
void UXOpenGLRenderDevice::DrawComplexEnd(INT NextProgram)
{
	if (DrawComplexBufferData.IndexOffset > 0)
		DrawComplexVertsSinglePass(DrawComplexBufferData);

	CHECK_GL_ERROR();

	// Clean up
	glDisableVertexAttribArray(VERTEX_COORD_ATTRIB);
}

void UXOpenGLRenderDevice::DrawComplexStart()
{
	WaitBuffer(DrawComplexSinglePassRange, DrawComplexBufferData.Index);
	
#if !defined(__EMSCRIPTEN__) && !__LINUX_ARM__
	if (UseAA && PrevProgram != GouraudPolyVert_Prog)
		glEnable(GL_MULTISAMPLE);
#endif

	glUseProgram(DrawComplexProg);
	glBindVertexArray(DrawComplexVertsSinglePassVao);
	glBindBuffer(GL_ARRAY_BUFFER, DrawComplexVertBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, DrawComplexSSBO);

	glEnableVertexAttribArray(VERTEX_COORD_ATTRIB);
	
	DrawComplexDrawParams.PolyFlags() = SetFlags(CurrentAdditionalPolyFlags | CurrentPolyFlags);
	PrevDrawComplexBeginOffset = -1;

	CHECK_GL_ERROR();
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
