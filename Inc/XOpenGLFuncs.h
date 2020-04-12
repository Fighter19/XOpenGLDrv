#ifndef GL_EXT
#define GL_EXT(ext)
#endif

#ifndef GL_PROC
#define GL_PROC(ext,fntype,fnname)
#endif

GL_PROC(GL,PFNGLGETERRORPROC,glGetError)
GL_PROC(GL,PFNGLACTIVETEXTUREPROC,glActiveTexture)
GL_PROC(GL,PFNGLATTACHSHADERPROC,glAttachShader)
GL_PROC(GL,PFNGLBINDBUFFERPROC,glBindBuffer)
GL_PROC(GL,PFNGLBINDBUFFERRANGEPROC,glBindBufferRange)
GL_PROC(GL,PFNGLBINDSAMPLERPROC,glBindSampler)
GL_PROC(GL,PFNGLBINDTEXTUREPROC,glBindTexture)
GL_PROC(GL,PFNGLBINDVERTEXARRAYPROC,glBindVertexArray)
GL_PROC(GL,PFNGLBLENDCOLORPROC,glBlendColor)
GL_PROC(GL,PFNGLBLENDFUNCPROC,glBlendFunc)
GL_PROC(GL,PFNGLBUFFERDATAPROC,glBufferData)
GL_PROC(GL,PFNGLBUFFERSUBDATAPROC,glBufferSubData)
GL_PROC(GL,PFNGLCLEARPROC,glClear)
GL_PROC(GL,PFNGLCLEARCOLORPROC,glClearColor)
GL_PROC(GL,PFNGLCOLORMASKPROC,glColorMask)
GL_PROC(GL,PFNGLCOMPILESHADERPROC,glCompileShader)
GL_PROC(GL,PFNGLCOMPRESSEDTEXIMAGE2DPROC,glCompressedTexImage2D)
GL_PROC(GL,PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC,glCompressedTexSubImage2D)
GL_PROC(GL,PFNGLCREATEPROGRAMPROC,glCreateProgram)
GL_PROC(GL,PFNGLCREATESHADERPROC,glCreateShader)
GL_PROC(GL,PFNGLDELETEBUFFERSPROC,glDeleteBuffers)
GL_PROC(GL,PFNGLDELETESAMPLERSPROC,glDeleteSamplers)
GL_PROC(GL,PFNGLDELETETEXTURESPROC,glDeleteTextures)
GL_PROC(GL,PFNGLDELETEVERTEXARRAYSPROC,glDeleteVertexArrays)
GL_PROC(GL,PFNGLDEPTHFUNCPROC,glDepthFunc)
GL_PROC(GL,PFNGLDEPTHMASKPROC,glDepthMask)
GL_PROC(GL,PFNGLDISABLEPROC,glDisable)
GL_PROC(GL,PFNGLDISABLEVERTEXATTRIBARRAYPROC,glDisableVertexAttribArray)
GL_PROC(GL,PFNGLDRAWARRAYSPROC,glDrawArrays)
GL_PROC(GL,PFNGLENABLEPROC,glEnable)
GL_PROC(GL,PFNGLENABLEVERTEXATTRIBARRAYPROC,glEnableVertexAttribArray)
GL_PROC(GL,PFNGLFINISHPROC,glFinish)
GL_PROC(GL,PFNGLFLUSHPROC,glFlush)
GL_PROC(GL,PFNGLGENBUFFERSPROC,glGenBuffers)
GL_PROC(GL,PFNGLGENSAMPLERSPROC,glGenSamplers)
GL_PROC(GL,PFNGLGENTEXTURESPROC,glGenTextures)
GL_PROC(GL,PFNGLGENVERTEXARRAYSPROC,glGenVertexArrays)
GL_PROC(GL,PFNGLGENERATEMIPMAPPROC,glGenerateMipmap)
GL_PROC(GL,PFNGLGETFLOATVPROC,glGetFloatv)
GL_PROC(GL,PFNGLGETINTEGERVPROC,glGetIntegerv)
GL_PROC(GL,PFNGLGETSHADERINFOLOGPROC,glGetShaderInfoLog)
GL_PROC(GL,PFNGLGETSHADERIVPROC,glGetShaderiv)
GL_PROC(GL,PFNGLGETSTRINGPROC,glGetString)
GL_PROC(GL,PFNGLGETSTRINGIPROC,glGetStringi)
GL_PROC(GL,PFNGLGETUNIFORMBLOCKINDEXPROC,glGetUniformBlockIndex)
GL_PROC(GL,PFNGLGETUNIFORMLOCATIONPROC,glGetUniformLocation)
GL_PROC(GL,PFNGLLINKPROGRAMPROC,glLinkProgram)
GL_PROC(GL,PFNGLMAPBUFFERRANGEPROC,glMapBufferRange)
GL_PROC(GL,PFNGLPOLYGONOFFSETPROC,glPolygonOffset)
GL_PROC(GL,PFNGLREADPIXELSPROC,glReadPixels)
GL_PROC(GL,PFNGLSAMPLERPARAMETERFPROC,glSamplerParameterf)
GL_PROC(GL,PFNGLSAMPLERPARAMETERIPROC,glSamplerParameteri)
GL_PROC(GL,PFNGLSHADERSOURCEPROC,glShaderSource)
GL_PROC(GL,PFNGLTEXIMAGE2DPROC,glTexImage2D)
GL_PROC(GL,PFNGLTEXPARAMETERIPROC,glTexParameteri)
GL_PROC(GL,PFNGLTEXSTORAGE2DPROC,glTexStorage2D)
GL_PROC(GL,PFNGLTEXSUBIMAGE2DPROC,glTexSubImage2D)
GL_PROC(GL,PFNGLUNIFORM1FPROC,glUniform1f)
GL_PROC(GL,PFNGLUNIFORM1IPROC,glUniform1i)
GL_PROC(GL,PFNGLUNIFORM1IVPROC,glUniform1iv)
GL_PROC(GL,PFNGLUNIFORM1UIPROC,glUniform1ui)
GL_PROC(GL,PFNGLUNIFORM2FPROC,glUniform2f)
GL_PROC(GL,PFNGLUNIFORM3FPROC,glUniform3f)
GL_PROC(GL,PFNGLUNIFORM4FPROC,glUniform4f)
GL_PROC(GL,PFNGLUNIFORM4FVPROC,glUniform4fv)
GL_PROC(GL,PFNGLUNIFORMBLOCKBINDINGPROC,glUniformBlockBinding)
GL_PROC(GL,PFNGLUNMAPBUFFERPROC,glUnmapBuffer)
GL_PROC(GL,PFNGLUSEPROGRAMPROC,glUseProgram)
GL_PROC(GL,PFNGLVERTEXATTRIBPOINTERPROC,glVertexAttribPointer)
GL_PROC(GL,PFNGLVIEWPORTPROC,glViewport)

GL_PROC(GLCORE,PFNGLDEPTHRANGEPROC,glDepthRange)
GL_PROC(GLCORE,PFNGLCLEARDEPTHPROC,glClearDepth)

GL_PROC(GLES,PFNGLDEPTHRANGEFPROC,glDepthRangef)
GL_PROC(GLES,PFNGLCLEARDEPTHFPROC,glClearDepthf)

// This is only in GLES3 (not 2), but Emscripten exposes it.
GL_PROC(GL,PFNGLFENCESYNCPROC,glFenceSync)
GL_PROC(GL,PFNGLDELETESYNCPROC,glDeleteSync)
GL_PROC(GL,PFNGLCLIENTWAITSYNCPROC,glClientWaitSync)

GL_EXT(GL_EXT_texture_sRGB)
GL_EXT(GL_EXT_texture_filter_anisotropic)
GL_EXT(GL_EXT_texture_lod_bias)
GL_EXT(GL_EXT_texture_compression_s3tc)
GL_EXT(GL_WEBGL_compressed_texture_s3tc)
GL_EXT(GL_ANGLE_texture_compression_dxt)
GL_EXT(GL_EXT_texture_compression_dxt1)
GL_EXT(GL_EXT_texture_compression_dxt3)
GL_EXT(GL_EXT_texture_compression_dxt5)

#undef GL_EXT
#undef GL_PROC
