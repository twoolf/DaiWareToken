/***************************************************************************
                                                                           *
Copyright 2013 CertiVox UK Ltd.                                            *
                                                                           *
This file is part of CertiVox SkyKey SDK.                                  *
                                                                           *
The CertiVox SkyKey SDK provides developers with an                        *
extensive and efficient set of cryptographic functions.                    *
For further information about its features and functionalities please      *
refer to http://www.certivox.com                                           *
                                                                           *
* The CertiVox SkyKey SDK is free software: you can                        *
  redistribute it and/or modify it under the terms of the                  *
  GNU Affero General Public License as published by the                    *
  Free Software Foundation, either version 3 of the License,               *
  or (at your option) any later version.                                   *
                                                                           *
* The CertiVox SkyKey SDK is distributed in the hope                       *
  that it will be useful, but WITHOUT ANY WARRANTY; without even the       *
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. *
  See the GNU Affero General Public License for more details.              *
                                                                           *
* You should have received a copy of the GNU Affero General Public         *
  License along with CertiVox SkyKey SDK.                                  *
  If not, see <http://www.gnu.org/licenses/>.                              *
                                                                           *
You can be released from the requirements of the license by purchasing     *
a commercial license. Buying such a license is mandatory as soon as you    *
develop commercial activities involving the CertiVox SkyKey SDK            *
without disclosing the source code of your own applications, or shipping   *
the CertiVox SkyKey SDK with a closed source product.                      *
                                                                           *
***************************************************************************/
/*! \file  CvFileSystem.h
    \brief C++ classes providing portable File System operations.

*-  Project     : SkyKey SDK
*-  Authors     : Mony Aladjem
*-  Company     : Certivox
*-  Created     : April 2, 2013
*-  Last update : April 3, 2013
*-  Platform    : Windows XP SP3 - Windows 7
*-  Dependency  : 

*/

#ifndef CVFILESYSTEM_H
#define	CVFILESYSTEM_H

#include "CvCommon.h"

#include <vector>

namespace CvShared
{
	
#ifdef _WIN32
	#define PATH_TYPE	Wstring
#else
	#define PATH_TYPE	String
#endif

class CvFileAbs
{
public:

	enum enType_t
	{
		enType_Unknown = 0,
		enType_File,
		enType_Directory
	};

	class CvList : public std::vector<CvFileAbs*>
	{
	public:
		virtual ~CvList();
	};

	CvFileAbs( const String& aPath );
	CvFileAbs( const Wstring& aPath );

	virtual ~CvFileAbs()	{}

	virtual bool		Create( bool abCreateParents = true ) = 0;
	virtual bool		Delete() = 0;
	virtual bool		Exists() const = 0;

	virtual enType_t	GetType() const	{ return enType_Unknown; }

	String				GetPath() const;
	Wstring				GetPathW() const;
	String				GetName() const;
	Wstring				GetNameW() const;

protected:
	void				FixPath();

	PATH_TYPE	m_path;
};

class CvFile : public CvFileAbs
{
public:
	CvFile( const String& aPath ) : CvFileAbs(aPath)	{}
	CvFile( const Wstring& aPath ) : CvFileAbs(aPath)	{}

	virtual ~CvFile();

	virtual bool		Create( bool abCreateParents = true );
	virtual bool		Delete();
	virtual bool		Exists() const;

	virtual enType_t	GetType() const	{ return enType_File; }

	long long			GetSize() const;
};

class CvDirectory : public CvFileAbs
{
public:
	CvDirectory( const String& aPath ) : CvFileAbs(aPath)	{}
	CvDirectory( const Wstring& aPath ) : CvFileAbs(aPath)	{}

	virtual ~CvDirectory();

	virtual bool		Create( bool abCreateParents = true );
	virtual bool		Delete()	{ return Delete(true); }
	virtual bool		Exists() const;

	virtual enType_t	GetType() const	{ return enType_Directory; }

	bool				Delete( bool abRecursive );
	bool				List( OUT CvList& aListFiles, enType_t aFilter = enType_Unknown );
};

}	// namespace CvShared

#endif	// CVFILESYSTEM_H
