/*********************************************************\
 *  File: OSWindowLinux.h                                *
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


#ifndef __PLFRONTENDOS_OSWINDOW_LINUX_H__
#define __PLFRONTENDOS_OSWINDOW_LINUX_H__
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
	#ifdef None
		// ... we undef these to avoid name conflicts...
		namespace OSWindowLinuxInclude {
			enum {
				X11_None = None,
				X11_Always = Always,
				X11_Above = Above,
				X11_Success = Success
			};
		}

		#undef None
		#undef Always
		#undef Above
		#undef Success

		namespace XLib {
			enum {
				None = OSWindowLinuxInclude::X11_None,
				Always = OSWindowLinuxInclude::X11_Always,
				Above = OSWindowLinuxInclude::X11_Above,
				Success = OSWindowLinuxInclude::X11_Success
			};
		}
		// ... now that the namespace is name conflict free, go on with includes...
	#endif
#include "PLFrontendOS/OSWindow.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace PLFrontendOS {


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
class Frontend;


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    OS Linux frontend implementation class
*
*  @note
*    - Implementation of the bridge design pattern, this class is the implementor of the 'System' abstraction
*/
class OSWindowLinux : public OSWindow {


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
	friend class Frontend;


	//[-------------------------------------------------------]
	//[ Private functions                                     ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] cFrontendOS
		*    Owner frontend implementation instance
		*/
		OSWindowLinux(Frontend &cFrontendOS);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~OSWindowLinux();


	//[-------------------------------------------------------]
	//[ Private virtual OSWindow functions                    ]
	//[-------------------------------------------------------]
	private:
		virtual PLCore::handle GetNativeWindowHandle() const override;
		virtual void Redraw() override;
		virtual bool Ping() override;
		virtual PLCore::uint32 GetWidth() const override;
		virtual PLCore::uint32 GetHeight() const override;
		virtual void SetFullscreenAltTab(bool bAllowed) override;
		virtual void SetFullscreen(bool bFullscreen) override;


	//[-------------------------------------------------------]
	//[ Private static functions                              ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Signal handler callback
		*
		*  @param[in] nSignal
		*    Signal
		*/
		static void SignalHandler(int nSignal);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		Frontend *m_pFrontendOS;			/**< Owner frontend implementation instance, always valid! */
		Display	 *m_pDisplay;				/**< System display, considered to be always valid */
		Atom 	  m_wmDelete;				/**< System atom for delete */
		Window	  m_nNativeWindowHandle;	/**< Native window handle, can be 0 */


};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLFrontendOS


#endif // __PLFRONTENDOS_OSWINDOW_LINUX_H__

