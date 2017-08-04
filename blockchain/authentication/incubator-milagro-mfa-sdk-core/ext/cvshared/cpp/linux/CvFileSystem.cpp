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
/*! \file  CvFileSystem.cpp
    \brief C++ classes providing portable File System operations.

*-  Project     : SkyKey SDK
*-  Authors     : Mony Aladjem
*-  Company     : Certivox
*-  Created     : April 2, 2013
*-  Last update : April 3, 2013
*-  Platform    : Linux / MacOS
*-  Dependency  : 

*/

#include "CvFileSystem.h"

#include "CvString.h"

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

namespace CvShared
{

CvFileAbs::CvList::~CvList()
{
	while( !empty() )
	{
		CvFileAbs* pFile = back();
		pop_back();
		delete pFile;
	}
}

CvFileAbs::CvFileAbs( const String& aPath ) :
	m_path( aPath )
{
	FixPath();
}

CvFileAbs::CvFileAbs( const Wstring& aPath ) :
	m_path( WstringToString(aPath) )
{
	FixPath();
}

void CvFileAbs::FixPath()
{
	size_t pos = m_path.find_last_not_of( "/" );
	if ( pos != PATH_TYPE::npos && pos < m_path.length()-1 )
		m_path.resize( pos+1 );
}

String CvFileAbs::GetPath() const
{
	return m_path; 
}

Wstring CvFileAbs::GetPathW() const
{
	return StringToWstring( m_path ); 
}

String CvFileAbs::GetName() const
{
	size_t pos = m_path.find_last_of( "/" );
	if ( pos == Wstring::npos )
		return "";

	return String( m_path, pos+1 );
}

Wstring CvFileAbs::GetNameW() const
{
	return StringToWstring( GetName() );	
}

CvFile::~CvFile()
{
}

bool CvFile::Exists() const
{
	struct stat st;
	
	if ( stat( m_path.c_str(), &st ) != 0 )
		return false;
	
	return !S_ISDIR(st.st_mode);
}

bool CvFile::Create( bool abCreateParents )
{
	if ( Exists() )
		return true;

	if ( abCreateParents )
	{
		//Check if the parent directory exists
		size_t pos = m_path.find_last_of( "/" );
		if ( pos != PATH_TYPE::npos )
		{
			if ( !CvDirectory( m_path.substr(0,pos) ).Create(true) )
				return false;
		}
	}
	
	FILE* fd = fopen( m_path.c_str(), "a+" );
	if( fd == NULL )
		return false;

	fclose( fd );
			
	return true;
}

bool CvFile::Delete()
{
	return ( remove( m_path.c_str() ) == 0 );
}

long long CvFile::GetSize() const
{
	struct stat st;
	
	if ( stat( m_path.c_str(), &st ) != 0 )
		return 0;
	
	return st.st_size;
}

CvDirectory::~CvDirectory()
{
}

bool CvDirectory::Exists() const
{
	struct stat st;
	
	if ( stat( m_path.c_str(), &st ) != 0 )
		return false;
	
	return S_ISDIR(st.st_mode);
}

bool CvDirectory::Create( bool abCreateParents )
{
	if ( Exists() )
		return true;

	if ( abCreateParents )
	{
		//Check if the parent directory exists
		size_t pos = m_path.find_last_of( "/" );
		if ( pos != PATH_TYPE::npos )
		{
			if ( !CvDirectory( m_path.substr(0,pos) ).Create(true) )
				return false;
		}
	}

	return ( mkdir( m_path.c_str(), S_IRWXU|S_IWGRP|S_IWOTH ) == 0 );
}

bool CvDirectory::Delete( bool abRecursive )
{
	if ( remove( m_path.c_str() ) != 0 )
	{
		if ( !abRecursive || errno != ENOTEMPTY )
			return false;

		CvList list;

		if ( !List( list ) )
			return false;

		for ( CvList::iterator itr = list.begin(); itr != list.end(); ++itr )
			if ( !(*itr)->Delete() )
				return false;

		return ( remove( m_path.c_str() ) == 0 );
	}

	return true;
}

bool CvDirectory::List( OUT CvList& aListFiles, enType_t aFilter )
{
	DIR* dir = opendir( m_path.c_str() );

	if( NULL == dir )
		return false;

	struct dirent entry;
	struct dirent* result = &entry;
	
	int rc = readdir_r( dir, &entry, &result );
	
	while( rc == 0 && result != NULL )
	{
		bool bDirectory = ( entry.d_type == DT_DIR );

		if ( ( bDirectory && aFilter != enType_File ) || ( !bDirectory && aFilter != enType_Directory ) )
		{
			String fileName = entry.d_name;
			if( fileName != "." && fileName != ".." )
			{
				String fullPath = m_path;
				fullPath += '/';
				fullPath += fileName;

				if ( bDirectory )
					aListFiles.push_back( new CvDirectory(fullPath) );
				else
					aListFiles.push_back( new CvFile(fullPath) );
			}
		}
	
		rc = readdir_r( dir, &entry, &result );
	}

	closedir( dir );

	return ( rc == 0 );
}

}	// namespace CvShared