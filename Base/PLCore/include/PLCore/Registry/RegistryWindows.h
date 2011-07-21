/*********************************************************\
 *  File: RegistryWindows.h                              *
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


#ifndef __PLCORE_REGISTRY_WINDOWS_H__
#define __PLCORE_REGISTRY_WINDOWS_H__
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "PLCore/PLCoreWindowsIncludes.h"
#include "PLCore/Registry/RegistryImpl.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace PLCore {


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Windows 'Registry' implementation
*/
class RegistryWindows : public RegistryImpl {


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
	friend class Registry;


	//[-------------------------------------------------------]
	//[ Private functions                                     ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Constructor
		*/
		RegistryWindows();

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~RegistryWindows();


	//[-------------------------------------------------------]
	//[ Private virtual RegistryImpl functions                ]
	//[-------------------------------------------------------]
	private:
		virtual Registry::ERegistry GetRegistryType() const;
		virtual bool Open(Registry::EKey nKey, const String &sSubKey, uint32 nAccess);
		virtual bool Create(Registry::EKey nKey, const String &sSubKey, uint32 nAccess);
		virtual bool Delete();
		virtual void Close();
		virtual Registry::EKey GetOpenKey() const;
		virtual String GetOpenSubKey() const;
		virtual uint32 GetOpenAccessMode() const;
		virtual uint32 GetNumOfSubKeys() const;
		virtual String GetSubKey(uint32 nIndex) const;
		virtual uint32 GetNumOfValues() const;
		virtual String GetValue(uint32 nIndex) const;
		virtual Registry::EType GetValueType(const String &sName) const;
		virtual String GetValueString(const String &sName) const;
		virtual uint32 GetValueDWord(const String &sName) const;
		virtual uint32 GetValueBinary(const String &sName, uint8 *pBuffer, uint32 nSize) const;
		virtual bool SetValueString(const String &sName, const String &sValue);
		virtual bool SetValueDWord(const String &sName, uint32 nValue);
		virtual bool SetValueBinary(const String &sName, const uint8 *pBuffer, uint32 nSize);
		virtual void Flush();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		Registry::EKey	m_nKey;		/**< Registry key */
		String			m_sSubKey;	/**< Sub key */
		uint32			m_nAccess;	/**< Access modes */
		HKEY			m_hKey;		/**< Windows registry key handle */


};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLCore


#endif // __PLCORE_REGISTRY_WINDOWS_H__