

#pragma once
#ifndef GLOBALS_H
#define GLOBALS_H

// ................................................................
#ifdef _DEBUG
extern std::ofstream bug_os;//"test.txt"
#define BUG_OS( x ) {  bug_os << x; }
#else
#define BUG_OS( x ) __noop;
#endif

#ifdef USE_WINDOWS_EX
//Rich Stuff ..................................................................
class _afxRichEditStreamCookie
{
public:
	CArchive& m_ar;
	DWORD m_dwError;
	_afxRichEditStreamCookie(CArchive& ar) : m_ar(ar) {m_dwError=0;}
};

DWORD CALLBACK EditStreamCallBack( DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb );
BOOL SetRichText( CRichEditCtrl& re, char* buf, int format= SF_RTF );

#endif // USE_WINDOWS_EX

// ...................................................................
// AStrTFlag new 8-3-7 use where type is enum as flags
/*
For use with:
enum Types {...}
AFX_STATIC_DATA const LPCTSTR lpsTypes[]= { "str", "str"...}
#define TYPES_SIZE countof( lpsTypes )

Declare in class or global
static AStrTFlag< Types > typeSet;

Define as:
AStrTFlag< Types > SomeClass::typeSet( (LPCTSTR*)lpsTypes, PROJECT_FILETYPE_SIZE );

or:
AStrTFlag< Types > aTypeSet( ASTRTYPE_DEF( lpsTypes ) );
*/

template < typename T, typename Char = TCHAR >
class AStrTFlag
{
	typedef std::map<long, const Char*> map_type;
	typedef typename map_type::iterator It;
	typedef std::pair<long, const Char*> Pair;
	typedef std::basic_string< Char > astr_type;
	map_type map;

public:
	AStrTFlag(const Char** strTypes, long const sizeTypes)
	{
		for (long i = 0; i < sizeTypes; ++i)
			map.insert(Pair(T(i), strTypes[i]));
	}
	//CString GetFlagsStr( long inFlags )
	//{
	//	CString strT;
	//	It it;
	//	if( ! inFlags )//check for null
	//	{
	//		it= map.find( inFlags );
	//		if( it != map.end( ) )
	//			strT= it->second;
	//		return strT;
	//	}
	//	long mask= 1;
	//	for( ; mask; mask= mask << 1 )
	//	{
	//		if( inFlags & mask ) //null ignored
	//		{
	//			it= map.find( ( inFlags & mask ) );
	//			if( it != map.end( ) )
	//			{
	//				if( ! strT.IsEmpty( ) )
	//					strT+= _T(',');
	//				strT+= it->second;
	//			}
	//		}
	//	}
	//	return strT;
	//}
	const Char* GetFlagsPointer(T inFlags)
	{
		It it;
		if (!inFlags)//check for null
		{
			it = map.find(inFlags);
			ASSERT(it != map.end());
			return it->second;
		}
		long mask = 1;
		for (; mask; mask = mask << 1)
		{
			if (inFlags & mask)
			{
				it = map.find(inFlags);
				ASSERT(it != map.end());
				return it->second;
			}
		}
		ASSERT(false);
		return NULL;
	}
	T GetFlags(const Char* pstrInFlags)
	{
		astr_type strT(pstrInFlags);
		long pos = 0;
		long flags = 0;
		for (; pos != -1; )
		{
			long to = strT.find(',', pos);
			astr_type strE;
			if (to == astr_type::npos)
			{
				strE = strT.substr(pos);
				--to;
			}
			else
				strE = strT.substr(pos, to - pos);

			for (It it = map.begin(); it != map.end(); ++it)
			{
				if (strE == it->second)
				{
					flags |= it->first;
					break;
				}
			}
			pos = to + 1;
		}
		return static_cast<T>(flags);
	}
};

#endif // GLOBALS_H
