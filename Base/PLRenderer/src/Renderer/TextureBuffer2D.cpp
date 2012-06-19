/*********************************************************\
 *  File: TextureBuffer2D.cpp                            *
 *
 *  Copyright (C) 2002-2012 The PixelLight Team (http://www.pixellight.org/)
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


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <PLMath/Math.h>
#include "PLRenderer/Renderer/TextureBuffer2D.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLCore;
using namespace PLMath;
namespace PLRenderer {


//[-------------------------------------------------------]
//[ Public functions                                      ]
//[-------------------------------------------------------]
/**
*  @brief
*    Destructor
*/
TextureBuffer2D::~TextureBuffer2D()
{
}

/**
*  @brief
*    Returns the texture buffer size
*/
Vector2i TextureBuffer2D::GetSize(uint32 nMipmap) const
{
	// Check the given parameter
	if (nMipmap > m_nNumOfMipmaps)
		return Vector2i::Zero; // Error!

	// Get the size of the mipmap level
	Vector2i vSize = m_vSize;
	uint32 nCurrentMipmap = 0;
	while (nCurrentMipmap < nMipmap) {
		vSize.x >>= 1;
		vSize.y >>= 1;
		nCurrentMipmap++;
	}
	if (vSize.x < 1)
		vSize.x = 1;
	if (vSize.y < 1)
		vSize.y = 1;

	// Return the size
	return vSize;
}


//[-------------------------------------------------------]
//[ Protected functions                                   ]
//[-------------------------------------------------------]
/**
*  @brief
*    Constructor
*/
TextureBuffer2D::TextureBuffer2D(Renderer &cRenderer, uint32 nFlags) : TextureBuffer(cRenderer, TypeTextureBuffer2D, nFlags)
{
}


//[-------------------------------------------------------]
//[ Private functions                                     ]
//[-------------------------------------------------------]
/**
*  @brief
*    Copy constructor
*/
TextureBuffer2D::TextureBuffer2D(const TextureBuffer2D &cSource) : TextureBuffer(cSource.GetRenderer(), cSource.GetType(), TypeTextureBuffer2D),
	m_vSize(cSource.m_vSize)
{
	// No implementation because the copy constructor is never used
}

/**
*  @brief
*    Copy operator
*/
TextureBuffer2D &TextureBuffer2D::operator =(const TextureBuffer2D &cSource)
{
	// No implementation because the copy operator is never used
	return *this;
}


//[-------------------------------------------------------]
//[ Public virtual TextureBuffer functions                ]
//[-------------------------------------------------------]
bool TextureBuffer2D::IsPowerOfTwo() const
{
	return (Math::IsPowerOfTwo(m_vSize.x) && Math::IsPowerOfTwo(m_vSize.y));
}

uint32 TextureBuffer2D::GetNumOfPixels(uint32 nMipmap) const
{
	// Check the given parameter
	if (nMipmap <= m_nNumOfMipmaps) {
		// Get the size of the mipmap level
		const Vector2i vSize = GetSize(nMipmap);

		// Return the number of pixels of the mipmap level
		return vSize.x*vSize.y;
	}

	// Error!
	return 0;
}

uint32 TextureBuffer2D::GetNumOfBytes(uint32 nMipmap, EPixelFormat nFormat) const
{
	// Check the given parameter
	if (nMipmap <= m_nNumOfMipmaps) {
		// Get the size of the mipmap level
		const Vector2i vSize = GetSize(nMipmap);

		// Calculate the number of bytes
		if (nFormat == Unknown)
			nFormat = m_nFormat;
		if (IsCompressedFormat(nFormat)) {
			// DXT formats divide the texture buffer into 4x4 blocks, each block has a fixed number of bytes
			switch (nFormat) {
				case DXT1:
				case LATC1:
					return ((vSize.x+3)/4)*((vSize.y+3)/4)*8;

				case DXT3:
				case DXT5:
				case LATC2:
					return ((vSize.x+3)/4)*((vSize.y+3)/4)*16;

				case L8:
				case L16:
				case A8:
				case L4A4:
				case L8A8:
				case D16:
				case D24:
				case D32:
				case R3G3B2:
				case R5G6B5:
				case R5G5B5A1:
				case R4G4B4A4:
				case R8G8B8:
				case R8G8B8A8:
				case R10G10B10A2:
				case R16G16B16A16:
				case L16F:
				case L32F:
				case R16G16B16A16F:
				case R32G32B32A32F:
				case Unknown:
				default:
					// Those cases are handled below
					break;
			}
		} else {
			return GetBytesPerPixel(nFormat)*vSize.x*vSize.y;
		}
	}

	// Error!
	return 0;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLRenderer
