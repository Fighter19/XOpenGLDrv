/*=============================================================================
CheckExtensions.cpp: Check if extensions are available.
Copyright 2014-2017 Oldunreal

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


UBOOL UXOpenGLRenderDevice::GLExtensionSupported(FString ExtensionName)
{
    #if SDL2BUILD
        return SDL_GL_ExtensionSupported(appToAnsi(*ExtensionName));
    #else
        return AllExtensions.InStr(*FString::Printf(TEXT("%ls "), *ExtensionName)) != -1;
    #endif
}

void UXOpenGLRenderDevice::CheckExtensions()
{
	guard(UXOpenGLRenderDevice::CheckExtensions);

	#ifdef __LINUX_ARM__
        if (GenerateMipMaps)
        {
            if (GLExtensionSupported(TEXT("GL_EXT_texture_storage"))  && GenerateMipMaps)
            {
                debugf(TEXT("XOpenGL: GL_EXT_texture_storage found. GenerateMipMaps enabled."));
            }
            else
            {
                debugf(TEXT("XOpenGL: GL_EXT_texture_storage not found. GenerateMipMaps disabled."));
                GenerateMipMaps = false;
            }
        }

        if (UseBindlessTextures)
        {
            if (GLExtensionSupported(TEXT("GL_IMG_bindless_texture")))
            {
                debugf(TEXT("XOpenGL: GL_IMG_bindless_texture found. UseBindlessTextures enabled."));
            }
            else
            {
                debugf(TEXT("XOpenGL: GL_IMG_bindless_texture not found. UseBindlessTextures disabled"));
                UseBindlessTextures = false;
            }
        }
		//usually we would assume this extension to be supported in general, but it seems not every driver really does in ES mode. (RasPi, AMD Radeon R5 Graphics, ???)
        if (GLExtensionSupported(TEXT("GL_EXT_clip_cull_distance")) || GLExtensionSupported(TEXT("GL_ARB_cull_distance")))
        {
            debugf(TEXT("XOpenGL: GL_ARB_cull_distance / GL_EXT_clip_cull_distance found."));
        }
        else
        {
            debugf(TEXT("XOpenGL: GL_ARB_cull_distance / GL_EXT_clip_cull_distance not found."));
            SupportsClipDistance = false; // have to disable this functionality.
        }

        NVIDIAMemoryInfo = false; // found no such info available...yet?
        AMDMemoryInfo = false;
	#else
        if (UsePersistentBuffers)
        {
            if (GLExtensionSupported(TEXT("GL_ARB_buffer_storage")))
            {
                debugf(TEXT("XOpenGL: GL_ARB_buffer_storage found. UsePersistentBuffers enabled."));
            }
            else
            {
                debugf(TEXT("XOpenGL: GL_ARB_buffer_storage not found. UsePersistentBuffers and UseBindlessTextures disabled."));
                UsePersistentBuffers = false;
                UseBindlessTextures = false;
            }
        }

        if (UseBufferInvalidation)
        {
            if (GLExtensionSupported(TEXT("GL_ARB_invalidate_subdata")))
            {
                debugf(TEXT("XOpenGL: GL_ARB_invalidate_subdata found. UseBufferInvalidation enabled."));
            }
            else
            {
                debugf(TEXT("XOpenGL: GL_ARB_invalidate_subdata not found. UseBufferInvalidation disabled."));
                UseBufferInvalidation = false;
            }
        }

        if (GenerateMipMaps)
        {
            if (GLExtensionSupported(TEXT("GL_ARB_texture_storage")))
            {
                debugf(TEXT("XOpenGL: GL_ARB_texture_storage found. GenerateMipMaps enabled."));
            }
            else
            {
                debugf(TEXT("XOpenGL: GL_ARB_texture_storage not found. GenerateMipMaps disabled."));
                GenerateMipMaps = false;
            }
        }

        if (UseBindlessTextures)
        {
            if (GLExtensionSupported(TEXT("GL_ARB_gpu_shader_int64")) && GLExtensionSupported(TEXT("GL_ARB_shading_language_420pack")) && GLExtensionSupported(TEXT("GL_ARB_bindless_texture")))
            {
                debugf(TEXT("XOpenGL: GL_ARB_gpu_shader_int64, GL_ARB_shading_language_420pack, and GL_ARB_bindless_texture found. UseBindlessTextures enabled."));
            }
            else
            {
                debugf(TEXT("XOpenGL: GL_ARB_gpu_shader_int64, GL_ARB_shading_language_420pack, or GL_ARB_bindless_texture not found. UseBindlessTextures disabled."));
                UseBindlessTextures = false;
            }
        }

        if (UseShaderDrawParameters)
        {
            if (GLExtensionSupported(TEXT("GL_ARB_shader_draw_parameters")))
            {
                debugf(TEXT("XOpenGL: GL_ARB_shader_draw_parameters found. UseShaderDrawParameters enabled."));
            }
            else
            {
                debugf(TEXT("XOpenGL: GL_ARB_shader_draw_parameters not found. UseShaderDrawParameters disabled."));
                UseShaderDrawParameters = false;
            }
        }

        //usually we would assume this extension to be supported in general, but it seems not every driver really does in ES mode. (RasPi, AMD Radeon R5 Graphics, ???)
        if (GLExtensionSupported(TEXT("GL_EXT_clip_cull_distance")) || GLExtensionSupported(TEXT("GL_ARB_cull_distance")))
        {
            debugf(TEXT("XOpenGL: GL_ARB_cull_distance / GL_EXT_clip_cull_distance found."));
        }
        else
        {
            debugf(TEXT("XOpenGL: GL_ARB_cull_distance / GL_EXT_clip_cull_distance not found."));
            SupportsClipDistance = false; // have to disable this functionality.
        }

		if (UseShaderDrawParameters)
		{
            if (GLExtensionSupported(TEXT("GL_ARB_shader_draw_parameters")))
            {
                debugf(TEXT("XOpenGL: GL_ARB_shader_draw_parameters found. UseShaderDrawParameters enabled."));
            }
            else
            {
                debugf(TEXT("XOpenGL: GL_ARB_shader_draw_parameters not found. UseShaderDrawParameters disabled."));
                UseShaderDrawParameters = false;
            }
		}

        if (GLExtensionSupported(TEXT("GL_NVX_gpu_memory_info")))
        {
            debugf(TEXT("XOpenGL: GL_NVX_gpu_memory_info found."));
            NVIDIAMemoryInfo = true;
        }
        else NVIDIAMemoryInfo = false;

        if (GLExtensionSupported(TEXT("GL_ATI_meminfo")))
        {
            debugf(TEXT("XOpenGL: GL_ATI_meminfo found."));
            AMDMemoryInfo = true;
        }
        else AMDMemoryInfo = false;

#ifndef SDL2BUILD // not worth the hassle with GLX, let SDL check if it works for now.
        if (GLExtensionSupported(TEXT("WGL_EXT_swap_control")))
        {
            debugf(TEXT("XOpenGL: WGL_EXT_swap_control found."));
            SwapControlExt = true;
        }
        else
        {
            debugf(TEXT("XOpenGL: WGL_EXT_swap_control not found. Can't set VSync options."));
            SwapControlExt = false;
        }

        if (GLExtensionSupported(TEXT("WGL_EXT_swap_control_tear")))
        {
            debugf(TEXT("XOpenGL: WGL_EXT_swap_control_tear found."));
            SwapControlTearExt = true;
        }
        else
        {
            debugf(TEXT("WGL_EXT_swap_control_tear is not supported by device."));
            SwapControlTearExt = false;
        }
#else
        // Just some additional info to check on...
        INT r=0, g=0, b=0, a=0, db=0, srgb=0, dbu=0;
        SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &r);
        SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &g);
        SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &b);
        SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &a);
        debugf(TEXT("XOpenGL: SDL_GL RED_SIZE:%i GREEN_SIZE:%i BLUE_SIZE:%i ALPHA_SIZE:%i"),r,g,b,a);

        SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &db);
        debugf(TEXT("XOpenGL: SDL_GL_DEPTH_SIZE DesiredDepthBits: %i, provided: %i"),DesiredDepthBits, db);

        SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &dbu);
        debugf(TEXT("XOpenGL: SDL_GL_DOUBLEBUFFER: %i"),dbu);

        if (UseSRGBTextures)
        {
            SDL_GL_GetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, &srgb);
            debugf(TEXT("XOpenGL: SDL_GL_FRAMEBUFFER_SRGB_CAPABLE: %i"),srgb);
        }
        CHECK_GL_ERROR();
#endif

        INT MaxTextureImageUnits = 0;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MaxTextureImageUnits);
        debugf(TEXT("XOpenGL: MaxTextureImageUnits: %i"), MaxTextureImageUnits);

        INT MaxVertexTextureImageUnits = 0;
        glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &MaxVertexTextureImageUnits);
        debugf(TEXT("XOpenGL: MaxVertexTextureImageUnits: %i"), MaxVertexTextureImageUnits);

        INT MaxImageUnits = 0;
        glGetIntegerv(GL_MAX_IMAGE_UNITS, &MaxImageUnits);
        debugf(TEXT("XOpenGL: MaxImageUnits: %i"), MaxImageUnits);
	#endif

    if (GLExtensionSupported(TEXT("GL_KHR_debug")) && UseOpenGLDebug)
    {
        GWarn->Logf(TEXT("XOpenGL: OpenGL debugging extension found!"));
    }
    else if (UseOpenGLDebug)
    {
        GWarn->Logf(TEXT("XOpenGL: OpenGL debugging extension not found! Disabling UseOpenGLDebug"));
        UseOpenGLDebug = 0;
    }

	INT MaxCombinedTextureImageUnits = 0;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MaxCombinedTextureImageUnits);
	debugf(TEXT("XOpenGL: MaxCombinedTextureImageUnits: %i"), MaxCombinedTextureImageUnits);

	INT MaxElementsVertices = 0;
	glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &MaxElementsVertices);
	debugf(TEXT("XOpenGL: MaxElementsVertices: %i"), MaxElementsVertices);

	INT MaxUniformBufferBindings = 0;
	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &MaxUniformBufferBindings);
	debugf(TEXT("XOpenGL: MaxUniformBufferBindings: %i"), MaxUniformBufferBindings);

	if (OpenGLVersion == GL_Core)
	{
		INT MaxDualSourceDrawBuffers;
		glGetIntegerv(GL_MAX_DUAL_SOURCE_DRAW_BUFFERS, &MaxDualSourceDrawBuffers);
		debugf(TEXT("XOpenGL: MaxDualSourceDrawBuffers: %i"), MaxDualSourceDrawBuffers);
	}

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &MaxTextureSize);
	debugf(TEXT("XOpenGL: MaxTextureSize: %i"), MaxTextureSize);

	if (!GLExtensionSupported(TEXT("GL_EXT_texture_lod_bias")))
	{
		debugf(TEXT("XOpenGL: Texture lod bias extension not found!"));
		LODBias = 0;
	}

    if (!GLExtensionSupported(TEXT("GL_EXT_texture_compression_s3tc")))
	{
		debugf(TEXT("XOpenGL: GL_EXT_texture_compression_s3tc extension not found!"));
		Compression_s3tcExt = false;
	}

    if (UseSRGBTextures && !GLExtensionSupported(TEXT("GL_EXT_texture_sRGB")) && !GLExtensionSupported(TEXT("GL_EXT_sRGB"))) //GL_EXT_sRGB for ES
	{
		debugf(TEXT("XOpenGL: GL_EXT_texture_sRGB extension not found, UseSRGBTextures disabled!"));
		UseSRGBTextures = 0;
	}

    if (!GLExtensionSupported(TEXT("GL_EXT_texture_filter_anisotropic")))
	{
		debugf(TEXT("XOpenGL: Anisotropic filter extension not found!"));
		MaxAnisotropy = 0.f;
	}
	if (MaxAnisotropy > 0.f)
	{
		FLOAT tmp;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &tmp);

		if (tmp < 0.f)
			tmp = 0.f; // seems in Linux ARM with ODROID-XU4 the extension check fails. Better safe than sorry.

		debugf(TEXT("XOpenGL: MaxAnisotropy = (%f/%f)"), MaxAnisotropy, tmp);

		if (MaxAnisotropy > tmp)
			MaxAnisotropy = tmp;

		UseTrilinear = true; // Anisotropic filtering doesn't make much sense without trilinear filtering
	}

	if (UseAA)
	{
        if (NumAASamples < 2)
        {
            debugf(TEXT("XOpenGL: NumAASamples was set < 2 but UseAA enabled, increasing to minimum value of 2"));
            NumAASamples = 2;
        }

        INT MaxAASamples = 0;
        glGetIntegerv(GL_MAX_SAMPLES, &MaxAASamples);
        if (NumAASamples>MaxAASamples)
        {
            debugf(TEXT("XOpenGL: NumAASamples was set > maximum samples supported, setting to %i"),MaxAASamples);
			NumAASamples = MaxAASamples;
        }

		INT NumberOfAASamples = 0, AABuffers = 0;
	#ifdef SDL2BUILD
        SDL_GL_GetAttribute( SDL_GL_MULTISAMPLEBUFFERS, &AABuffers );
        SDL_GL_GetAttribute( SDL_GL_MULTISAMPLESAMPLES, &NumberOfAASamples );
        debugf(TEXT("XOpenGL: SDL_GL_MULTISAMPLEBUFFERS: %i, requested NumAASamples: %i, provided NumAASamples/MaxSamples: (%i/%i)"), AABuffers, NumAASamples, NumberOfAASamples, MaxAASamples);
    #else
        glGetIntegerv(GL_MAX_SAMPLES, &MaxAASamples);
		glGetIntegerv(GL_SAMPLES, &NumberOfAASamples);
		debugf(TEXT("XOpenGL: NumAASamples: (%i/%i)"), NumberOfAASamples, MaxAASamples);
    #endif
	}

	if (GenerateMipMaps && !UsePrecache)
	{
		debugf(TEXT("XOpenGL: Enabling UsePrecache for GenerateMipMaps."));
		UsePrecache = 1;
	}

    // Extensions
    NumberOfExtensions = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &NumberOfExtensions);
    debugf(TEXT("XOpenGL: GL_NUM_EXTENSIONS found: %i"), NumberOfExtensions);

    // Clipping Planes
    MaxClippingPlanes = 0;
    glGetIntegerv(GL_MAX_CLIP_DISTANCES, &MaxClippingPlanes);
    debugf(TEXT("XOpenGL: GL_MAX_CLIP_DISTANCES found: %i"), MaxClippingPlanes);

    INT MaxUniformBlockSize = 0;
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &MaxUniformBlockSize); //Check me!!! For whatever reason this appears to return on (some?) AMD drivers a value of 572657868
    debugf(TEXT("XOpenGL: MaxUniformBlockSize: %i"), MaxUniformBlockSize);

    if (UseBindlessTextures)
    {
        if (MaxBindlessTextures == 0)
        {
            MaxBindlessTextures = MaxUniformBlockSize / 16;
            debugf(TEXT("XOpenGL: Initializing MaxBindlessTextures to %i"), MaxBindlessTextures);
        }
        else if (MaxUniformBlockSize < MaxBindlessTextures * 16)
        {
            debugf(TEXT("XOpenGL: UseBindlessTextures is enabled but MaxBindlessTextures is too high. Reducing from %i to %i"), MaxBindlessTextures, MaxUniformBlockSize / 16);
            MaxBindlessTextures = MaxUniformBlockSize / 16;
        }
    }
	CHECK_GL_ERROR();

	unguard;
}
