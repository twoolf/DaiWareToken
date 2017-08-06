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
*-  Platform    : Windows XP SP3 - Windows 7
*-  Dependency  : 

*/

#include "CvFileSystem.h"

#include "CvString.h"

#include <windows.h>

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

CvFileAbs::CvFileAbs( const string& aPath ) :
	m_path( StringToWstring(aPath) )
{
	FixPath();
}

CvFileAbs::CvFileAbs( const wstring& aPath ) :
	m_path( aPath )
{
	FixPath();
}

void CvFileAbs::FixPath()
{
	size_t pos = m_path.find_last_not_of( L"\\/" );
	if ( pos != PATH_TYPE::npos && pos < m_path.length()-1 )
		m_path.resize( pos+1 );
}

string CvFileAbs::GetPath() const
{
	return WstringToString( m_path ); 
}

wstring CvFileAbs::GetPathW() const
{
	return m_path; 
}

string CvFileAbs::GetName() const
{
	return WstringToString( GetNameW() );
}

wstring CvFileAbs::GetNameW() const
{
	size_t pos = m_path.find_last_of( L"\\/" );
	if ( pos == wstring::npos )
		return L"";

	return wstring( m_path, pos+1 );
}

CvFile::~CvFile()
{
}

bool CvFile::Exists() const
{
	DWORD dwAttrib = GetFileAttributes( m_path.c_str() );
	
	return ( dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY) );
}

bool CvFile::Create( bool abCreateParents )
{
	if ( Exists() )
		return true;

	if ( abCreateParents )
	{
		//Check if the parent directory exists
		size_t pos = m_path.find_last_of( L"\\/" );
		if ( pos != PATH_TYPE::npos )
		{
			if ( !CvDirectory( m_path.substr(0,pos) ).Create(true) )
				return false;
		}
	}

	HANDLE hFile = CreateFile( m_path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL );

	if ( hFile == INVALID_HANDLE_VALUE )
		return false;

	CloseHandle( hFile );

	return true;
}

bool CvFile::Delete()
{
	return ( DeleteFile( m_path.c_str() ) != FALSE );
}

long long CvFile::GetSize() const
{
	WIN32_FILE_ATTRIBUTE_DATA attrData;
	if ( GetFileAttributesEx( m_path.c_str(), GetFileExInfoStandard, &attrData ) == FALSE )
		return 0;

	return ((long long)attrData.nFileSizeHigh << 32) | attrData.nFileSizeLow;
}

CvDirectory::~CvDirectory()
{
}

bool CvDirectory::Exists() const
{
	DWORD dwAttrib = GetFileAttributes( m_path.c_str() );
	
	return ( dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) );
}

bool CvDirectory::Create( bool abCreateParents )
{
	if ( Exists() )
		return true;

	if ( abCreateParents )
	{
		//Check if the parent directory exists
		size_t pos = m_path.find_last_of( L"\\/" );
		if ( pos != PATH_TYPE::npos )
		{
			if ( !CvDirectory( m_path.substr(0,pos) ).Create(true) )
				return false;
		}
	}

	return ( CreateDirectory( m_path.c_str(), NULL ) != FALSE );
}

bool CvDirectory::Delete( bool abRecursive )
{
	if ( RemoveDirectory( m_path.c_str() ) == FALSE )
	{
		if ( !abRecursive || GetLastError() != ERROR_DIR_NOT_EMPTY )
			return false;

		CvList list;

		if ( !List( list ) )
			return false;

		for ( CvList::iterator itr = list.begin(); itr != list.end(); ++itr )
			if ( !(*itr)->Delete() )
				return false;

		return ( RemoveDirectory( m_path.c_str() ) != FALSE );
	}

	return true;
}

bool CvDirectory::List( OUT CvList& aListFiles, enType_t aFilter )
{
	wstring dir = m_path;
	dir += TEXT("\\*");

	WIN32_FIND_DATA ffd;
	HANDLE hFind = FindFirstFile( dir.c_str(), &ffd );

	if( INVALID_HANDLE_VALUE == hFind )
		return false;

	do
	{
		bool bDirectory = ( (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 );

		if ( ( bDirectory && aFilter != enType_File ) || ( !bDirectory && aFilter != enType_Directory ) )
		{
			wstring fileName = ffd.cFileName;
			if( fileName != L"." && fileName != L".." )
			{
				wstring fullPath = m_path;
				fullPath += L'\\';
				fullPath += fileName;

				if ( bDirectory )
					aListFiles.push_back( new CvDirectory(fullPath) );
				else
					aListFiles.push_back( new CvFile(fullPath) );
			}
		}
	}
	while( FindNextFile( hFind, &ffd ) != 0 );

	FindClose( hFind );

	return true;
}

}	// namespace CvShared