/*********************************************************\
 *  File: LogFormatter.cpp                               *
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
#include "PLCore/File/File.h"
#include "PLCore/File/Directory.h"
#include "PLCore/Log/LogFormatter.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace PLCore {


//[-------------------------------------------------------]
//[ Public functions                                      ]
//[-------------------------------------------------------]
/**
*  @brief
*    To activate/deactivate that the [LogLevel] prefix should be shown
*    in the log before each log message e.g. [Info] info log level message
*/
void LogFormatter::ShowLogLevelPrefix(bool bShow)
{
	m_bShowLogLevelPrefix = bShow;
}


//[-------------------------------------------------------]
//[ Protected functions                                   ]
//[-------------------------------------------------------]
/**
*  @brief
*    Default constructor
*/
LogFormatter::LogFormatter() :
	m_pFile(nullptr),
	m_bShowLogLevelPrefix(true)
{
}

/**
*  @brief
*    Destructor
*/
LogFormatter::~LogFormatter()
{
}

/**
*  @brief
*    Helper function to open the log as a file
*/
File *LogFormatter::OpenFile(const String &sFilename, String::EFormat nStringFormat)
{
	// Check filename
	if (sFilename.GetLength()) {
		// Open the file
		File *pFile = new File(sFilename);
		if (!pFile->Open(File::FileWrite | File::FileCreate, nStringFormat)) {
			delete pFile;
			pFile = nullptr;
		}

		// Done
		return pFile;
	}

	// Error!
	return nullptr;
}


//[-------------------------------------------------------]
//[ Private functions                                     ]
//[-------------------------------------------------------]
/**
*  @brief
*    Copy constructor
*/
LogFormatter::LogFormatter(const LogFormatter &cSource) :
	m_pFile(nullptr),
	m_bShowLogLevelPrefix(true)
{
	// No implementation because the copy constructor is never used
}

/**
*  @brief
*    Copy operator
*/
LogFormatter &LogFormatter::operator =(const LogFormatter &cSource)
{
	// No implementation because the copy operator is never used
	return *this;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLCore