

#pragma once
#include <boost/locale.hpp>

// ..........................................................................
//http://stackoverflow.com/questions/2055849/visual-c-migrating-traditional-c-and-c-string-code-to-a-unicode-world
namespace std
{

#ifdef _MSC_VER

#ifdef UNICODE
typedef             wstring                         tstring ;
typedef             wistream                        tistream ;
// etc.
#else // Not UNICODE
typedef             string                          tstring ;
typedef             istream                         tistream ;
// etc.
#endif

#endif

#ifdef  _AFX //using MFC
inline std::ostream& operator << ( std::ostream& s, const CString& str ) { s << (LPCTSTR)str; return s; }

#endif

} // namespace std

//when mixing..........................

// !!!!! this is not thread safe !!!!!
#ifdef USING_UNI_CONVERTER
inline const wchar_t* utf8_to_uni( const char* pIn )
{
	static std::wstring hold;
	hold= boost::locale::conv::utf_to_utf<wchar_t>( pIn );
	return hold.c_str( );
}

// !!!!! this is not thread safe !!!!!
inline const char* uni_t0_utf8( const wchar_t* pIn )
{
	static std::string hold;
	hold= boost::locale::conv::utf_to_utf<char>( pIn );
	return hold.c_str( );
}
#endif


