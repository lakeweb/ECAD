// ..........................................................................
#pragma once

//A String to Type where Type is consectutive enum
#ifdef UNICODE
typedef const wchar_t* CHAR_PT;
#else
typedef char* CHAR_PT;
#endif
template < typename T >
class AStrType
{
private:
	size_t count;
	CHAR_PT* list;

public:
	AStrType( CHAR_PT* aList, int aCount )
	{
		ASSERT_POINTER( aList, CHAR_PT );
		list= aList;
		count= aCount;
	}
	//String to Type
	T stot( CHAR_PT name, bool bSetToZero= false )
	{
		for( size_t i= 0; i < count; ++i )
			if( !_tcsncmp( name, list[i], _tcslen( name ) ) )
				return (T)i;

		return bSetToZero ? (T)0 : (T)-1;
	}
	//Type to String
	CHAR_PT ttos( T i )
	{
		CString t;
		if( (unsigned int)i < count && (int)i >= 0 )
			return list[i];

		return _T("");
	}
	long GetSize( ) { return count; }

	bool ValidName( CHAR_PT name ) { return stot( name ) != -1; }

#ifdef _DEBUG //Used in ASSERT(..)
	bool ValidIndex( T i ) { return (unsigned int)i < count; }
#endif
};
#define countof( a ) ( sizeof( a )/ sizeof( a[0] ) )
#define ASTRTYPE_DEF( a ) (CHAR_PT*)a, countof( a )

