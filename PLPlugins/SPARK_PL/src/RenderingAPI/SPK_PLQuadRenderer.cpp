/*********************************************************\
 *  File: SPK_PLQuadRenderer.cpp                         *
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


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <PLRenderer/Renderer/Renderer.h>
#include <PLRenderer/Renderer/IndexBuffer.h>
#include <PLRenderer/Renderer/VertexBuffer.h>
#include <PLRenderer/Renderer/FixedFunctions.h>
#include <PLRenderer/Texture/TextureHandler.h>
#include "SPARK_PL/RenderingAPI/SPK_PLBuffer.h"
#include "SPARK_PL/RenderingAPI/SPK_PLQuadRenderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLGeneral;
using namespace PLMath;
using namespace PLGraphics;
using namespace PLRenderer;
namespace SPARK_PL {


//[-------------------------------------------------------]
//[ Private definitions                                   ]
//[-------------------------------------------------------]
const uint32	  SPK_PLQuadRenderer::NumOfVerticesPerParticle = 4;
const uint32	  SPK_PLQuadRenderer::NumOfIndicesPerParticle  = 6;
const std::string SPK_PLQuadRenderer::PLBufferName("SPK_PLQuadRenderer_Buffer");


//[-------------------------------------------------------]
//[ Public static functions                               ]
//[-------------------------------------------------------]
SPK_PLQuadRenderer *SPK_PLQuadRenderer::Create(PLRenderer::Renderer &cRenderer, float fScaleX, float fScaleY)
{
	SPK_PLQuadRenderer *pSPK_PLQuadRenderer = new SPK_PLQuadRenderer(cRenderer, fScaleX, fScaleY);
	registerObject(pSPK_PLQuadRenderer);
	return pSPK_PLQuadRenderer;
}


//[-------------------------------------------------------]
//[ Public functions                                      ]
//[-------------------------------------------------------]
/**
*  @brief
*    Constructor of SPK_PLQuadRenderer
*/
SPK_PLQuadRenderer::SPK_PLQuadRenderer(PLRenderer::Renderer &cRenderer, float fScaleX, float fScaleY) : SPK_PLRenderer(cRenderer), QuadRendererInterface(fScaleX, fScaleY), Oriented3DRendererInterface(),
	m_pSPK_PLBuffer(nullptr),
	m_pTextureHandler(new TextureHandler()),
	m_nTextureBlending(FixedFunctions::TextureEnvironment::Modulate),
	m_nCurrentVertexSize(0),
	m_pfCurrentPosition(nullptr),
	m_pfCurrentTexCoord(nullptr),
	m_pCurrentVertexBuffer(nullptr),
	m_nCurrentVertex(0)
{
}

/**
*  @brief
*    Destructor of SPK_PLQuadRenderer
*/
SPK_PLQuadRenderer::~SPK_PLQuadRenderer()
{
	delete m_pTextureHandler;
}

/**
*  @brief
*    Sets the texture of this SPK_PLQuadRenderer
*/
Texture *SPK_PLQuadRenderer::GetTexture() const
{
	return m_pTextureHandler->GetResource();
}

/**
*  @brief
*    Gets the texture of this SPK_PLQuadRenderer
*/
void SPK_PLQuadRenderer::SetTexture(Texture *pTexture)
{
	m_pTextureHandler->SetResource(pTexture);
}

/**
*  @brief
*    Gets the texture blending function of this SPK_PLQuadRenderer
*/
FixedFunctions::TextureEnvironment::Enum SPK_PLQuadRenderer::GetTextureBlending() const
{
	return m_nTextureBlending;
}

/**
*  @brief
*    Sets the texture blending function of this SPK_PLQuadRenderer
*/
void SPK_PLQuadRenderer::SetTextureBlending(FixedFunctions::TextureEnvironment::Enum nTextureBlending)
{
	m_nTextureBlending = nTextureBlending;
}


//[-------------------------------------------------------]
//[ Public virtual SPK::Renderer functions                ]
//[-------------------------------------------------------]
void SPK_PLQuadRenderer::render(const SPK::Group &group)
{
	if (prepareBuffers(group)) {
		// Is there a valid m_pSPK_PLBuffer instance?
		if (m_pSPK_PLBuffer && m_pSPK_PLBuffer->GetVertexBuffer()) {
			// Setup render states
			InitBlending();
			InitRenderingHints();
			void (SPK_PLQuadRenderer::*pRenderParticle)(const SPK::Particle&);	// Pointer to the right render method
			switch (texturingMode) {
				case SPK::TEXTURE_2D:
					m_pTextureHandler->Bind();
					if (group.getModel()->isEnabled(SPK::PARAM_TEXTURE_INDEX))
						pRenderParticle = group.getModel()->isEnabled(SPK::PARAM_ANGLE) ? &SPK_PLQuadRenderer::Render2DAtlasRot : &SPK_PLQuadRenderer::Render2DAtlas;
					else
						pRenderParticle = group.getModel()->isEnabled(SPK::PARAM_ANGLE) ? &SPK_PLQuadRenderer::Render2DRot : &SPK_PLQuadRenderer::Render2D;

					{ // Get the fixed functions interface
						FixedFunctions *pFixedFunctions = GetPLRenderer().GetFixedFunctions();
						if (pFixedFunctions)
							pFixedFunctions->SetTextureStageState(0, FixedFunctions::TextureStage::ColorTexEnv, GetTextureBlending());
					}
					break;

				case SPK::TEXTURE_3D:
					m_pTextureHandler->Bind();
					pRenderParticle = group.getModel()->isEnabled(SPK::PARAM_ANGLE) ? &SPK_PLQuadRenderer::Render3DRot : &SPK_PLQuadRenderer::Render3D;

					{ // Get the fixed functions interface
						FixedFunctions *pFixedFunctions = GetPLRenderer().GetFixedFunctions();
						if (pFixedFunctions)
							pFixedFunctions->SetTextureStageState(0, FixedFunctions::TextureStage::ColorTexEnv, GetTextureBlending());
					}
					break;

				case SPK::TEXTURE_NONE:
					GetPLRenderer().SetTextureBuffer();
					pRenderParticle = group.getModel()->isEnabled(SPK::PARAM_ANGLE) ? &SPK_PLQuadRenderer::Render2DRot : &SPK_PLQuadRenderer::Render2D;
					break;

				default:
					pRenderParticle = nullptr;
					break;
			}

			// Get the vertex buffer instance from m_pSPK_PLBuffer and lock it
			VertexBuffer *pVertexBuffer = m_pSPK_PLBuffer->GetVertexBuffer();
			if (pVertexBuffer->Lock(Lock::WriteOnly)) {
				// Get the inverse of the current view matrix
				FixedFunctions *pFixedFunctions = GetPLRenderer().GetFixedFunctions();
				const Matrix4x4 &mView   = pFixedFunctions ? pFixedFunctions->GetTransformState(FixedFunctions::Transform::View)  : Matrix4x4::Identity;
				const Matrix4x4 &mWorld  = pFixedFunctions ? pFixedFunctions->GetTransformState(FixedFunctions::Transform::World) : Matrix4x4::Identity;
				const Matrix4x4 mViewInv = (mView*mWorld).GetInverted();

				// Get current vertex buffer data
				m_nCurrentVertexSize	= pVertexBuffer->GetVertexSize();
				m_pfCurrentPosition		= static_cast<float*>(pVertexBuffer->GetData(0, VertexBuffer::Position));
				m_pfCurrentTexCoord		= static_cast<float*>(pVertexBuffer->GetData(0, VertexBuffer::TexCoord));
				m_pCurrentVertexBuffer	= pVertexBuffer;
				m_nCurrentVertex		= 0;

				// Calculate the current orientation
				const bool bGlobalOrientation = precomputeOrientation3D(
					group,
					SPK::Vector3D(-mViewInv.fM[8],  -mViewInv.fM[9],  -mViewInv.fM[10]),
					SPK::Vector3D( mViewInv.fM[4],   mViewInv.fM[5],   mViewInv.fM[6]),
					SPK::Vector3D( mViewInv.fM[12],  mViewInv.fM[13],  mViewInv.fM[14]));
				if (pRenderParticle) {
					if (bGlobalOrientation) {
						computeGlobalOrientation3D();
						for (size_t i=0; i<group.getNbParticles(); i++)
							(this->*pRenderParticle)(group.getParticle(i));
					} else {
						for (size_t i=0; i<group.getNbParticles(); i++) {
							const SPK::Particle &cParticle = group.getParticle(i);
							computeSingleOrientation3D(cParticle);
							(this->*pRenderParticle)(cParticle);
						}
					}
				}

				// Unlock the vertex buffer
				pVertexBuffer->Unlock();
			}

			// Get the fixed functions interface
			FixedFunctions *pFixedFunctions = GetPLRenderer().GetFixedFunctions();
			if (pFixedFunctions) {
				// Make the index buffer to the current renderer index buffer
				IndexBuffer *pIndexBuffer = m_pSPK_PLBuffer->GetIndexBuffer();
				if (pIndexBuffer)
					GetPLRenderer().SetIndexBuffer(pIndexBuffer);

				// Make the vertex buffer to the current renderer vertex buffer
				pFixedFunctions->SetVertexBuffer(pVertexBuffer);

				// Draw
				GetPLRenderer().DrawIndexedPrimitives(Primitive::TriangleList, 0, group.getNbParticles()*NumOfVerticesPerParticle-1, 0, group.getNbParticles()*NumOfIndicesPerParticle);
			}
		}
	}
}


//[-------------------------------------------------------]
//[ Public virtual SPK::QuadRendererInterface functions   ]
//[-------------------------------------------------------]
bool SPK_PLQuadRenderer::setTexturingMode(SPK::TexturingMode mode)
{
	// We can only use 3D textures if they are available
	if ((mode != SPK::TEXTURE_3D) || GetPLRenderer().GetCapabilities().bTextureBuffer3D) {
		// *thumbs up* - all is fine
		texturingMode = mode;

		// Done
		return true;
	} else {
		// *thumbs down* - the renderer is not capable of everything we need
		return false;
	}
}


//[-------------------------------------------------------]
//[ Public virtual SPK::BufferHandler functions           ]
//[-------------------------------------------------------]
void SPK_PLQuadRenderer::createBuffers(const SPK::Group &group)
{
	// Create the SPK_PLBuffer instance
	m_pSPK_PLBuffer = static_cast<SPK_PLBuffer*>(group.createBuffer(PLBufferName, PLBufferCreator(GetPLRenderer(), NumOfVerticesPerParticle, NumOfIndicesPerParticle, texturingMode), 0U, false));

	// Is there a valid m_pSPK_PLBuffer instance?
	if (m_pSPK_PLBuffer && m_pSPK_PLBuffer->GetVertexBuffer()) {
		switch (texturingMode) {
			case SPK::TEXTURE_2D:
				if (!group.getModel()->isEnabled(SPK::PARAM_TEXTURE_INDEX)) {
					// Get the vertex buffer instance from m_pSPK_PLBuffer and lock it
					VertexBuffer *pVertexBuffer = m_pSPK_PLBuffer->GetVertexBuffer();
					if (pVertexBuffer->Lock(Lock::WriteOnly)) {
						// Set correct data
						const uint32 nVertexSize = pVertexBuffer->GetVertexSize();
						float *pfTexCoord = static_cast<float*>(pVertexBuffer->GetData(0, VertexBuffer::TexCoord));
						for (size_t i=0; i<group.getParticles().getNbReserved(); i++) {
							// Top right vertex
							pfTexCoord[0] = 1.0f;
							pfTexCoord[1] = 0.0f;
							pfTexCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(pfTexCoord) + nVertexSize);	// Next, please!

							// Top left vertex
							pfTexCoord[0] = 0.0f;
							pfTexCoord[1] = 0.0f;
							pfTexCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(pfTexCoord) + nVertexSize);	// Next, please!

							// Bottom left
							pfTexCoord[0] = 0.0f;
							pfTexCoord[1] = 1.0f;
							pfTexCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(pfTexCoord) + nVertexSize);	// Next, please!

							// Bottom right
							pfTexCoord[0] = 1.0f;
							pfTexCoord[1] = 1.0f;
							pfTexCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(pfTexCoord) + nVertexSize);	// Next, please!
						}

						// Unlock the vertex buffer
						pVertexBuffer->Unlock();
					}
				}
				break;

			case SPK::TEXTURE_3D:
			{
				// Get the vertex buffer instance from m_pSPK_PLBuffer and lock it
				VertexBuffer *pVertexBuffer = m_pSPK_PLBuffer->GetVertexBuffer();
				if (pVertexBuffer->Lock(Lock::WriteOnly)) {
					// Set correct data
					const uint32 nVertexSize = pVertexBuffer->GetVertexSize();
					float *pfTexCoord = static_cast<float*>(pVertexBuffer->GetData(0, VertexBuffer::TexCoord));
					for (size_t i=0; i<group.getParticles().getNbReserved(); i++) {
						// Top right vertex
						pfTexCoord[0] = 1.0f;
						pfTexCoord[1] = 0.0f;
						pfTexCoord[2] = 0.0f;
						pfTexCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(pfTexCoord) + nVertexSize);	// Next, please!

						// Top left vertex
						pfTexCoord[0] = 0.0f;
						pfTexCoord[1] = 0.0f;
						pfTexCoord[2] = 0.0f;
						pfTexCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(pfTexCoord) + nVertexSize);	// Next, please!

						// Bottom left
						pfTexCoord[0] = 0.0f;
						pfTexCoord[1] = 1.0f;
						pfTexCoord[2] = 0.0f;
						pfTexCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(pfTexCoord) + nVertexSize);	// Next, please!

						// Bottom right
						pfTexCoord[0] = 1.0f;
						pfTexCoord[1] = 1.0f;
						pfTexCoord[2] = 0.0f;
						pfTexCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(pfTexCoord) + nVertexSize);	// Next, please!
					}

					// Unlock the vertex buffer
					pVertexBuffer->Unlock();
				}
				break;
			}
		}
	}

	// Is there a valid m_pSPK_PLBuffer instance?
	if (m_pSPK_PLBuffer && m_pSPK_PLBuffer->GetIndexBuffer()) {
		// Get the index buffer instance from m_pSPK_PLBuffer and lock it
		IndexBuffer *pIndexBuffer = m_pSPK_PLBuffer->GetIndexBuffer();
		if (pIndexBuffer && pIndexBuffer->Lock(Lock::WriteOnly)) {
			// Fill the index buffer with valid data
			for (uint32 i=0, nVertices=4, nIndex=0; i<group.getParticles().getNbReserved(); i++, nVertices+=4) {
				// Triangle 1
				pIndexBuffer->SetData(nIndex++, nVertices-4);
				pIndexBuffer->SetData(nIndex++, nVertices-3);
				pIndexBuffer->SetData(nIndex++, nVertices-2);

				// Triangle 2
				pIndexBuffer->SetData(nIndex++, nVertices-4);
				pIndexBuffer->SetData(nIndex++, nVertices-2);
				pIndexBuffer->SetData(nIndex++, nVertices-1);
			}

			// Unlock the index buffer
			pIndexBuffer->Unlock();
		}
	}
}

void SPK_PLQuadRenderer::destroyBuffers(const SPK::Group &group)
{
	group.destroyBuffer(PLBufferName);
}


//[-------------------------------------------------------]
//[ Private functions                                     ]
//[-------------------------------------------------------]
inline void SPK_PLQuadRenderer::CallColorAndVertex(const SPK::Particle &cParticle)
{
	const float x = cParticle.position().x;
	const float y = cParticle.position().y;
	const float z = cParticle.position().z;
	const Color4 cColor(cParticle.getR(), cParticle.getG(), cParticle.getB(), cParticle.getParamCurrentValue(SPK::PARAM_ALPHA));

	// Quads are drawn in a counter clockwise order:
	{ // Top right vertex
		// Position
		m_pfCurrentPosition[0] = x + quadSide().x + quadUp().x;
		m_pfCurrentPosition[1] = y + quadSide().y + quadUp().y;
		m_pfCurrentPosition[2] = z + quadSide().z + quadUp().z;
		m_pfCurrentPosition = reinterpret_cast<float*>(reinterpret_cast<char*>(m_pfCurrentPosition) + m_nCurrentVertexSize);	// Next, please!

		// Color
		m_pCurrentVertexBuffer->SetColor(m_nCurrentVertex, cColor);
		m_nCurrentVertex++;	// Next, please!
	}

	{ // Top left vertex
		// Position
		m_pfCurrentPosition[0] = x - quadSide().x + quadUp().x;
		m_pfCurrentPosition[1] = y - quadSide().y + quadUp().y;
		m_pfCurrentPosition[2] = z - quadSide().z + quadUp().z;
		m_pfCurrentPosition = reinterpret_cast<float*>(reinterpret_cast<char*>(m_pfCurrentPosition) + m_nCurrentVertexSize);	// Next, please!

		// Color
		m_pCurrentVertexBuffer->SetColor(m_nCurrentVertex, cColor);
		m_nCurrentVertex++;	// Next, please!
	}

	{ // Bottom left
		// Position
		m_pfCurrentPosition[0] = x - quadSide().x - quadUp().x;
		m_pfCurrentPosition[1] = y - quadSide().y - quadUp().y;
		m_pfCurrentPosition[2] = z - quadSide().z - quadUp().z;
		m_pfCurrentPosition = reinterpret_cast<float*>(reinterpret_cast<char*>(m_pfCurrentPosition) + m_nCurrentVertexSize);	// Next, please!

		// Color
		m_pCurrentVertexBuffer->SetColor(m_nCurrentVertex, cColor);
		m_nCurrentVertex++;	// Next, please!
	}

	{ // Bottom right
		// Position
		m_pfCurrentPosition[0] = x + quadSide().x - quadUp().x;
		m_pfCurrentPosition[1] = y + quadSide().y - quadUp().y;
		m_pfCurrentPosition[2] = z + quadSide().z - quadUp().z;
		m_pfCurrentPosition = reinterpret_cast<float*>(reinterpret_cast<char*>(m_pfCurrentPosition) + m_nCurrentVertexSize);	// Next, please!

		// Color
		m_pCurrentVertexBuffer->SetColor(m_nCurrentVertex, cColor);
		m_nCurrentVertex++;	// Next, please!
	}
}

inline void SPK_PLQuadRenderer::CallTexture2DAtlas(const SPK::Particle &cParticle)
{
	computeAtlasCoordinates(cParticle);

	// Top right vertex
	m_pfCurrentTexCoord[0] = textureAtlasU1();
	m_pfCurrentTexCoord[1] = textureAtlasV0();
	m_pfCurrentTexCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(m_pfCurrentTexCoord) + m_nCurrentVertexSize);	// Next, please!

	// Top left vertex
	m_pfCurrentTexCoord[0] = textureAtlasU0();
	m_pfCurrentTexCoord[1] = textureAtlasV0();
	m_pfCurrentTexCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(m_pfCurrentTexCoord) + m_nCurrentVertexSize);	// Next, please!

	// Bottom left
	m_pfCurrentTexCoord[0] = textureAtlasU0();
	m_pfCurrentTexCoord[1] = textureAtlasV1();
	m_pfCurrentTexCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(m_pfCurrentTexCoord) + m_nCurrentVertexSize);	// Next, please!

	// Bottom right
	m_pfCurrentTexCoord[0] = textureAtlasU1();
	m_pfCurrentTexCoord[1] = textureAtlasV1();
	m_pfCurrentTexCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(m_pfCurrentTexCoord) + m_nCurrentVertexSize);	// Next, please!
}

inline void SPK_PLQuadRenderer::CallTexture3D(const SPK::Particle &cParticle)
{
	// Only the third component is manipulated
	const float fTextureIndex = cParticle.getParamCurrentValue(SPK::PARAM_TEXTURE_INDEX);

	// Top right vertex
	m_pfCurrentTexCoord[2] = fTextureIndex;
	m_pfCurrentTexCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(m_pfCurrentTexCoord) + m_nCurrentVertexSize);	// Next, please!

	// Top left vertex
	m_pfCurrentTexCoord[2] = fTextureIndex;
	m_pfCurrentTexCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(m_pfCurrentTexCoord) + m_nCurrentVertexSize);	// Next, please!

	// Bottom left
	m_pfCurrentTexCoord[2] = fTextureIndex;
	m_pfCurrentTexCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(m_pfCurrentTexCoord) + m_nCurrentVertexSize);	// Next, please!

	// Bottom right
	m_pfCurrentTexCoord[2] = fTextureIndex;
	m_pfCurrentTexCoord = reinterpret_cast<float*>(reinterpret_cast<char*>(m_pfCurrentTexCoord) + m_nCurrentVertexSize);	// Next, please!
}

void SPK_PLQuadRenderer::Render2D(const SPK::Particle &cParticle)
{
	scaleQuadVectors(cParticle, scaleX, scaleY);
	CallColorAndVertex(cParticle);
}

void SPK_PLQuadRenderer::Render2DRot(const SPK::Particle &cParticle)
{
	rotateAndScaleQuadVectors(cParticle, scaleX, scaleY);
	CallColorAndVertex(cParticle);
}

void SPK_PLQuadRenderer::Render3D(const SPK::Particle &cParticle)
{
	scaleQuadVectors(cParticle, scaleX, scaleY);
	CallColorAndVertex(cParticle);
	CallTexture3D(cParticle);
}

void SPK_PLQuadRenderer::Render3DRot(const SPK::Particle &cParticle)
{
	rotateAndScaleQuadVectors(cParticle, scaleX, scaleY);
	CallColorAndVertex(cParticle);
	CallTexture3D(cParticle);
}

void SPK_PLQuadRenderer::Render2DAtlas(const SPK::Particle &cParticle)
{
	scaleQuadVectors(cParticle, scaleX, scaleY);
	CallColorAndVertex(cParticle);
	CallTexture2DAtlas(cParticle);
}

void SPK_PLQuadRenderer::Render2DAtlasRot(const SPK::Particle &cParticle)
{
	rotateAndScaleQuadVectors(cParticle, scaleX, scaleY);
	CallColorAndVertex(cParticle);
	CallTexture2DAtlas(cParticle);
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // SPARK_PL