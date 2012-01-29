/*********************************************************\
 *  File: StackIterator.inl                              *
 *      Stack iterator template implementation
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
#include "PLCore/Container/Stack.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace PLCore {


//[-------------------------------------------------------]
//[ Private functions                                     ]
//[-------------------------------------------------------]
/**
*  @brief
*    Constructor
*/
template <class ValueType>
StackIterator<ValueType>::StackIterator(const Stack<ValueType> &lstStack, uint32 nIndex) :
	m_plstStack(&lstStack)
{
	// Start at first element?
	if (!nIndex) {
		m_pNextElement     = m_plstStack->m_pTop;
		m_pPreviousElement = nullptr;

	// Ok, we start somewhere between the start/end element
	} else {
		// Start with the first element
		m_pPreviousElement = m_plstStack->m_pTop;
		m_pNextElement     = m_pPreviousElement->pNextElement;
		uint32 nCurrent = 1;
		while (m_pNextElement && nCurrent != nIndex) {
			m_pPreviousElement = m_pNextElement;
			m_pNextElement     = m_pNextElement->pNextElement;
			nCurrent++;
		}
	}
}

/**
*  @brief
*    Constructor
*/
template <class ValueType>
StackIterator<ValueType>::StackIterator(const Stack<ValueType> &lstStack) :
	m_plstStack(&lstStack),
	m_pNextElement(nullptr),
	m_pPreviousElement(nullptr)
{
	// Start with the first element
	typename Stack<ValueType>::StackElement *pElement = lstStack.m_pTop;
	while (pElement) {
		// Next, please
		m_pPreviousElement = pElement;
		pElement		   = pElement->pNextElement;
	}
}

/**
*  @brief
*    Copy constructor
*/
template <class ValueType>
StackIterator<ValueType>::StackIterator(const StackIterator<ValueType> &cSource) :
	m_plstStack(cSource.m_plstStack),
	m_pNextElement(cSource.m_pNextElement),
	m_pPreviousElement(cSource.m_pPreviousElement)
{
}

/**
*  @brief
*    Destructor
*/
template <class ValueType>
StackIterator<ValueType>::~StackIterator()
{
}


//[-------------------------------------------------------]
//[ Private virtual IteratorImpl functions                ]
//[-------------------------------------------------------]
template <class ValueType>
IteratorImpl<ValueType> *StackIterator<ValueType>::Clone() const
{
	return new StackIterator<ValueType>(*this);
}

template <class ValueType>
bool StackIterator<ValueType>::HasNext() const
{
	return (m_pNextElement != nullptr);
}

template <class ValueType>
ValueType &StackIterator<ValueType>::Next()
{
	if (!m_pNextElement)
		return Stack<ValueType>::Null; // There's no next element
	m_pPreviousElement = m_pNextElement;
	m_pNextElement     = m_pNextElement->pNextElement;
	return m_pPreviousElement->Data;
}

template <class ValueType>
bool StackIterator<ValueType>::HasPrevious() const
{
	return (m_pPreviousElement != nullptr);
}

template <class ValueType>
ValueType &StackIterator<ValueType>::Previous()
{
	if (!m_pPreviousElement)
		return Stack<ValueType>::Null; // There's no previous element

	// Set next element
	m_pNextElement = m_pPreviousElement;

	// Start with the first element
	typename Stack<ValueType>::StackElement *pElement = m_plstStack->m_pTop;
	m_pPreviousElement = nullptr;
	while (pElement && m_pNextElement != pElement) {
		// Next, please
		m_pPreviousElement = pElement;
		pElement		   = pElement->pNextElement;
	}

	// Return data
	return m_pNextElement->Data;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLCore