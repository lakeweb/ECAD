// ...........................................................................
//CadParse.h

//#include "CADParseID.h"
//and it would produce........
#include <AStrType.h>
namespace CadParser
{
	enum enumnbers
	{
		e_unknown,
		e_general,
		e_object,
		e_text,

		eg_area,
		eg_line_width,

		eo_point,
		eo_line,
	};
	static const wchar_t *type_strs[ ]=
	{
		//main
		L"unknown",
		L"general",
		L"object",
		L"text",
		//general
		L"area",
		L"line_width",
		//object
		L"point",
		L"line",
	};
	static size_t type_size= _countof( type_strs );
	typedef AStrType< enumnbers > tag_lookup_type;

	extern tag_lookup_type tag_lookup;
}
// end CADParseID.h produce

// ...........................................................................
typedef boost::variant<
	unsigned int
	,double
	,std::tstring
>value;

// ...........................................................................
typedef std::vector< value > values;
typedef std::pair< std::tstring, values > pair_type;

// ...........................................................................
template< class Ty >
class estack : public std::stack< Ty >
{
public:
	Ty& bottom( ) { return *c.begin( ); }
	Ty& down( size_t i ) { assert( i < size( ) && i > 0 ); return *( c.end( ) - i ); }
};

// ...........................................................................
//debug
namespace debug_
{
	static char* which_str[ ]= {
		"uint",
		"double",
		"string",
	};
};

// ...........................................................................
namespace boost {

	std::wostream& operator<<(std::wostream& out, const value& v );
	std::wostream& operator<<(std::wostream& out, const values& v );
    std::wostream& operator<<(std::wostream& out, const pair_type& v);
}

#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
// ...........................................................................
class DrawingObects;

// ...........................................................................
namespace CadParser
{
	extern tag_lookup_type lookup;
	struct wrapper;
	// ..........................................................................
	class ParseBase
		:public boost::enable_shared_from_this< ParseBase > //item
	{
	public:
		ParseBase( wrapper* pw, CHAR_PT name )
			:p_wrapper( pw )
			,name( name )
		{ }
		typedef boost::shared_ptr< ParseBase > SP_ParseBase;
		virtual SP_ParseBase new_sp( ) { return boost::make_shared< ParseBase >( *this ); }
		virtual void insert( const pair_type& pair ) { } //unknown does nothing
		CHAR_PT get_name( ) const { return name; }
	protected:
		wrapper* p_wrapper;
		CHAR_PT name;
	};
	typedef ParseBase::SP_ParseBase SP_ParseBase;

	// ..........................................................................
	typedef std::function<void (int)> op_func;
	typedef std::map< std::tstring, ParseBase::SP_ParseBase > op_map_type;

	// ...........................................................................
	//#ifdef CAD_PARSE_SPIRIT
	//typedef int ParseDummy;
	//typedef boost::shared_ptr< ParseDummy > SP_ParseBase;
	//#endif
	
	struct target
	{
		//target( )
		//	:id( 0 )
		//{ }
		//target( std::tstring const& name )
		//	:name( name )
		//	,id( 0 )
		//{ }

		//target( SP_ParseBase sp )
		//	:name( name )
		//	,id( id )
		//	,p_target( sp )
		//{ }

		target( SP_ParseBase sp )
			:p_target( sp )
		{ }
		CHAR_PT get_name( ) const { return p_target->get_name( ); }

		//std::tstring name;
		//size_t id;
		SP_ParseBase p_target;
	};
	typedef estack< target > s_target;
	//#endif

	// ...........................................................................
	struct wrapper
	{
		DrawingObects* psw_project;

		double version;
		//size_t count;

		s_target targets;

		op_map_type main;

		wrapper( DrawingObects* pBoard )
			:psw_project( pBoard )
			//,count( 0 )
		{
			Init( );
		}

		void Init( );
	//the real stuff
		//calls from parser
		void pspirit_pop_dest( );
		void pspirit_push_dest( const pair_type &pair );
		void pspirit_post_dest( const pair_type &pair );
		void pspirit_word_dest( const std::tstring& str );
	};
	// ...........................................................................
	//context parser interface
	inline void pspirit_pop_dest_expr( wrapper& a ){ a.pspirit_pop_dest( ); }
	inline void pspirit_push_dest_expr( wrapper& a, pair_type const& pair ){ a.pspirit_push_dest( pair ); }
	inline void pspirit_post_dest_expr( wrapper& a, pair_type const& pair ){ a.pspirit_post_dest( pair ); }
	inline void pspirit_word_dest_expr( wrapper& a, std::tstring const& str ){ a.pspirit_word_dest( str ); }

}//namespace CadParser

bool CadParse( std::ifstream& in, DrawingObects* pBoard );

bool PostKicadParse( DrawingObects* pBoard );

