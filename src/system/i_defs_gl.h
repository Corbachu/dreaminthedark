//----------------------------------------------------------------------------
//  EDGE2 System Specific Header for OpenGL
//----------------------------------------------------------------------------
//
//  Copyright (c) 2007-2009  The EDGE2 Team.
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//----------------------------------------------------------------------------
//
//  Based on the DOOM source code, released by Id Software under the
//  following copyright:
//
//    Copyright (C) 1993-1996 by id Software, Inc.
//
//----------------------------------------------------------------------------

#ifndef __SYSTEM_SPECIFIC_DEFS_OPENGL__
#define __SYSTEM_SPECIFIC_DEFS_OPENGL__

#define USING_GL_TYPES 1
#define USE_GL 1
#ifdef DREAMCAST
#include "GLdc/include/GL/gl.h"
#include "GLdc/include/GL/glu.h"
#include "GLdc/include/GL/glext.h"
#include "GLdc/include/GL/glkos.h"
#include <dc/vec3f.h>
#include <dc/matrix.h> 
#include <dc/pvr.h>
#elif  LINUX
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#ifdef MACOSX
#include <GL/glew.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#endif

//#include "GLdc/include/GL/gl.h"
/* struct GLUquadric
{
    GLint       normals;
    GLboolean   textureCoords;
    GLint       orientation;
    GLint       drawStyle;
    void        (* errorCallback)( GLint );
};

typedef struct GLUquadric GLUquadric;
typedef GLUquadric GLUquadricObj;
/* Internal convenience typedefs
typedef void* (_GLUfuncptr)();

GLUquadric* gluNewQuadric(void);
void gluDeleteQuadric(GLUquadric*);
void gluQuadricCallback(GLUquadric*, GLenum, _GLUfuncptr);
void gluQuadricNormals(GLUquadric*, GLenum);
void gluQuadricTexture(GLUquadric*, GLboolean);
void gluQuadricOrientation(GLUquadric*, GLenum);
void gluQuadricDrawStyle(GLUquadric*, GLenum);
void gluCylinder(GLUquadric*, GLfloat, GLfloat, GLfloat, GLint, GLint);
void gluDisk(GLUquadric*, GLfloat, GLfloat, GLint, GLint);
void gluPartialDisk(GLUquadric*, GLfloat, GLfloat, GLint, GLint, GLfloat, GLfloat);
void gluSphere(GLUquadric*, GLfloat, GLint, GLint); */

#endif /* __SYSTEM_SPECIFIC_DEFS_OPENGL__ */

//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
