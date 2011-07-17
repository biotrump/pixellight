/*********************************************************\
 *  File: SPRTTShaders_GLSL.h                            *
 *
 *  Copyright (C) 2002-2011 The PixelLight Team (http://www.pixellight.org/)
 *
 *  This file is part of PixelLight.
 *
 *  PixelLight is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  PixelLight is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with PixelLight. If not, see <http://www.gnu.org/licenses/>.
\*********************************************************/


// GLSL (OpenGL 2.0 ("#version 110") and OpenGL ES 2.0 ("#version 100")) vertex shader source code, "#version" is added by hand
static const PLGeneral::String sVertexShaderSourceCodeGLSL = "\
// Attributes\n\
attribute highp vec3 VertexPosition;			// Object space vertex position input\n\
attribute highp vec2 VertexTextureCoordinate;	// Vertex texture coordinate input\n\
varying   highp vec2 VertexTextureCoordinateVS;	// Vertex texture coordinate output\n\
attribute lowp  vec4 VertexColor;				// Vertex color input\n\
varying   lowp  vec4 VertexColorVS;				// Vertex color output\n\
\n\
// Uniforms\n\
uniform highp mat4  ObjectSpaceToClipSpaceMatrix;	// Object space to clip space matrix\n\
uniform highp float ColorFactor;					// Color factor\n\
\n\
// Programs\n\
void main()\n\
{\n\
	// Calculate the clip space vertex position, lower/left is (-1,-1) and upper/right is (1,1)\n\
	gl_Position = ObjectSpaceToClipSpaceMatrix*vec4(VertexPosition, 1.0);\n\
\n\
	// Pass through the texture coordinate\n\
	VertexTextureCoordinateVS = VertexTextureCoordinate;\n\
\n\
	// Write out the vertex color\n\
	VertexColorVS = mix(vec4(1.0, 1.0, 1.0, 1.0), VertexColor, ColorFactor);\n\
}";


// GLSL (OpenGL 2.0 ("#version 110") and OpenGL ES 2.0 ("#version 100")) fragment shader source code, "#version" is added by hand
static const PLGeneral::String sFragmentShaderSourceCodeGLSL = "\
// Attributes\n\
varying highp vec2 VertexTextureCoordinateVS;	// Vertex texture coordinate input from vertex shader\n\
varying lowp  vec4 VertexColorVS;				// Vertex color input from vertex shader\n\
\n\
// Uniforms\n\
uniform lowp sampler2D DiffuseMap;	// Diffuse map\n\
\n\
// Programs\n\
void main()\n\
{\n\
	// Set fragment color\n\
	gl_FragColor = texture2D(DiffuseMap, VertexTextureCoordinateVS)*VertexColorVS;\n\
}";


//[-------------------------------------------------------]
//[ Scene program                                         ]
//[-------------------------------------------------------]
// GLSL (OpenGL 2.1 ("#version 120") and OpenGL ES 2.0 ("#version 100")) vertex shader source code, "#version" is added by hand
static const PLGeneral::String sSceneVertexShaderSourceCodeGLSL = "\
// Attributes\n\
attribute highp vec4 VertexPosition;	// Object space vertex position input\n\
attribute highp vec3 VertexNormal;		// Object space vertex normal input\n\
varying   highp vec3 VertexNormalVS;	// World space vertex normal output\n\
\n\
// Uniforms\n\
uniform highp mat4 ObjectSpaceToClipSpaceMatrix;	// Object space to clip space matrix\n\
uniform highp mat4 ObjectSpaceToWorldSpaceMatrix;	// Object space to world space matrix\n\
\n\
// Programs\n\
void main()\n\
{\n\
	// Calculate the clip space vertex position, lower/left is (-1,-1) and upper/right is (1,1)\n\
	gl_Position = ObjectSpaceToClipSpaceMatrix*VertexPosition;\n\
\n\
	// Calculate the world space vertex normal\n\
	VertexNormalVS = mat3(ObjectSpaceToWorldSpaceMatrix)*VertexNormal;\n\
}";


// GLSL (OpenGL 2.1 ("#version 120") and OpenGL ES 2.0 ("#version 100")) fragment shader source code, "#version" is added by hand
static const PLGeneral::String sSceneFragmentShaderSourceCodeGLSL = "\
// Attributes\n\
varying highp vec3 VertexNormalVS;	// World space vertex normal input from vertex shader\n\
\n\
// Uniforms\n\
uniform highp vec3 LightDirection;	// World space light direction\n\
\n\
// Programs\n\
void main()\n\
{\n\
	// Set fragment color by using primitiv directional lighting\n\
	lowp float intensity = clamp(dot(LightDirection, VertexNormalVS), 0.0, 1.0);\n\
	gl_FragData[0] = vec4(intensity, intensity, intensity, intensity);\n\
\n\
	// Color 1 only red component\n\
	gl_FragData[1] = vec4(1.0, 0.0, 0.0, 0.0);\n\
\n\
	// Color 2 only green component\n\
	gl_FragData[2] = vec4(0.0, 1.0, 0.0, 0.0);\n\
\n\
	// Color 3 only blue component\n\
	gl_FragData[3] = vec4(0.0, 0.0, 1.0, 0.0);\n\
}";