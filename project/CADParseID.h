#pragma once
#include <AStrType.h>

// ...........................................................................

	//#include  <boost/preprocessor.hpp>

	/*	this produces the like of:

namespace CadParser
{
	enum enumnbers
	{
		null = 0,
		en_general =( 1 << ( 2 - 2 ) ), en_objects =( 1 << ( 3 - 2 ) ), en_text =( 1 << ( 4 - 2 ) ),  
	};
	static const char *type_strs[ ]=
	{
		"unknown",
		"general", "objects", "text",  
	};
	static size_t type_size= ( 3 );

	typedef AStrType< enumnbers > tag_lookup_type;
};

*/

#ifndef FLAGS64
typedef __int64 FLAGS64;
#endif //FLAGS64

	//by now, I can see putting this inside a single macro just for flag_str...
#define SEQ\
	( general )\
	( objects )\
	( text )

// ....
#define TO_STR( unused, data, elem )\
	BOOST_PP_STRINGIZE( elem ),

	// ....
#define TO_ENUM( counter, data, elem )\
	BOOST_PP_CAT( en_, BOOST_PP_CAT( elem =, ( 1 << ( counter - 2 ) ) ) )BOOST_PP_COMMA( ) 

	// ....
#define TO_VAL( counter, data, elem )\
	elem BOOST_PP_COMMA( ) 

// ....
namespace CadParser
{
	enum enumnbers
	{
		null = 0,
		BOOST_PP_SEQ_FOR_EACH( TO_ENUM, ~, SEQ )
	};
	static const char *type_strs[ ]=
	{
		"unknown",
		BOOST_PP_SEQ_FOR_EACH( TO_STR, ~, SEQ )
	};
	static size_t type_size= ( BOOST_PP_SEQ_SIZE( SEQ ) );

	typedef AStrType< enumnbers > tag_lookup_type;
};

#undef SEQ
#undef TO_STR
#undef TO_ENUM
#undef TO_VAL

