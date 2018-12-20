

//string_types.h

#pragma once

#ifndef _VECTOR_
#include <vector>
#endif
#include <iostream>


// .........
// assume UTF8 exclusively for now std::string can be replaced with the likes of std::tstring later
typedef char CSVCHAR_T;

// ..........................................................................
//http://stackoverflow.com/questions/2055849/visual-c-migrating-traditional-c-and-c-string-code-to-a-unicode-world
namespace std
{

#ifdef _MSC_VER

#ifdef UNICODE
	typedef             wstring                         tstring;
	typedef             wistream                        tistream;
	typedef				wostream						tostream;
	typedef				wstringstream					tstringstream;
//#define to_tstring						to_wstring;

	// etc.
#else // Not UNICODE
	typedef             string                          tstring;
	typedef             istream                         tistream;
	typedef				ostream							tostream;
	typedef				stringstream					tstringstream;
	// etc.
#endif

#endif

} // namespace std


/// .....................................


typedef std::tstring str_t;
typedef std::tstring::iterator str_it_t;
typedef std::tstring::const_iterator str_cit_t;
typedef str_t::const_iterator str_cit_t;

//typedef std::string stra_t;
//typedef std::string::iterator stra_it_t;
//typedef std::string::const_iterator stra_cit_t;
//typedef stra_t::const_iterator stra_cit_t;

// ..........
typedef std::vector< str_t > str_vect_t;
//typedef std::vector< stra_t > stra_vect_t;

/// .....................................
//#ifdef UNICODE
//inline std::wostream& operator << ( std::wostream& os, const str_vect_t& atrs )
//{
//	for( auto it= atrs.begin( ); it != atrs.end( ); ++it )
//	{
//		os << L" '" << *it << L"' ";
//	}
//	return os;
//}
//#else //UNICODE
//
//// .....................................
//inline std::ostream& operator << ( std::ostream& os, const str_vect_t& atrs )
//{
//	for( auto it= atrs.begin( ); it != atrs.end( ); ++it )
//	{
//		os << " '" << *it << "' ";
//	}
//	return os;
//}
//#endif //UNICODE

#ifdef USING_UNI_CONVERTER
#include <boost/locale.hpp>
// !!!!! this is not thread safe !!!!!
// .....................................
inline const wchar_t* utf8_to_uni( const char* pIn )
{
	static std::wstring hold;
	hold= boost::locale::conv::utf_to_utf<wchar_t>( pIn );
	return hold.c_str( );
}

// .....................................
inline const char* uni_to_utf8( const wchar_t* pIn )
{
	static std::string hold;
	hold= boost::locale::conv::utf_to_utf<char>( pIn );
	return hold.c_str( );
}
#endif

typedef str_t::size_type str_size_t;
typedef boost::iterator_range< str_it_t > str_it_range_t;
typedef std::vector< str_it_range_t > str_range_vect_t;

//typedef stra_t::size_type stra_size_t;
//typedef boost::iterator_range< stra_it_t > stra_it_range_t;
//typedef std::vector< str_it_range_t > stra_range_vect_t;
