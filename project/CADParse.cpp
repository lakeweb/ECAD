
#include "stdafx.h"
#include "debug.h"
#include "globals.h"
#include "cad_full.h"
#include "Share.h"
//#include "CADParseID.h"
#include "CADParse.h"
//#include <boost/type_traits/common_type.hpp>

// ...........................................................................
using namespace debug_;

#define DO_CAD
#if defined( DO_CAD ) && defined( _DEBUG )
std::wofstream cados( ".\\trace_parse.log" );
#define OT_CAD( x ) cados << x << std::endl;
#define D_KICAD D_WOUT
#else
#define OT_CAD( x )
#define D_KICAD( x )
#endif

// variant helpers ...................................................................
enum variant_pos
{
	var_uint_pos= 0,
	var_double_pos,
	var_str_pos,
};

inline const unsigned int& get_uint( values const& v, size_t pos ) { return boost::get< unsigned int >( v.at( pos ) ); }
//some strs will parse as ints
inline const std::tstring& get_str( values const& v, size_t pos )
{
	if( v.at( pos ).which( ) == var_uint_pos )
	{
		//TODO !!
		static std::tstring t= boost::lexical_cast< std::tstring >( boost::get< unsigned int >( v.at( pos ) ) );
		return t;
	}

	return boost::get< std::tstring >( v.at( pos ) );
}

//some doubles will parse as ints
inline const double& get_double( values const& v, size_t pos )
{
	if( v.at( pos ).which( ) == var_uint_pos )
	{
		unsigned int t= boost::get< unsigned int >( v.at( pos ) );
		value& tv=  const_cast< value& >( v.at( pos ) );
		tv= (double)t;
		return boost::get< double >( v.at( pos ) );
	}
	else // is double
	{
		assert( v.at( pos ).which( ) == var_double_pos ); //or deal with string here
		double& t= const_cast< double& >( boost::get< double >( v.at( pos ) ) );
		return t;
	}
}

// ...........................................................................
namespace CadParser
{
	// ..........................................................................
	tag_lookup_type tag_lookup( type_strs, type_size );
	// ..........................................................................
	void set_bg_box( bg_box& box, const values& data )
	{
		if( data.size( ) < 4 )
			return;
		box.min_corner( ).set( bg_point( get_double( data, 0 ), get_double( data, 1 ) ) );
		box.max_corner( ).set( bg_point( get_double( data, 2 ), get_double( data, 3 ) ) );
	}
	// ..........................................................................
	void set_bg_line( bg_line& line, const values& data )
	{
		if( data.size( ) < 4 )
			return;
		line.first.set< 0 >( get_double( data, 0 ) );
		line.first.set< 1 >( get_double( data, 1 ) );
		line.second.set< 0 >( get_double( data, 2 ) );
		line.second.set< 1 >( get_double( data, 3 ) );
	}
	// ..........................................................................
	inline bg_point get_bg_point( const values& data, size_t at= 0 )
	{
		return bg_point( get_double( data, at ), get_double( data, at + 1 ) );
	}
	// ..........................................................................
	// ..........................................................................
	SP_BaseItem get_line( const values& data )
	{
		if( data.size( ) == 4 )
		{
			bg_line bgline;
			set_bg_line( bgline, data );
			return LineItem(bgline);// .get_SP();
		}
		return LineItem( ).get_SP( );
	}
	// ..........................................................................
	SP_BaseItem get_arc( const values& data )
	{
		if( data.size( ) == 6 )
			return ArcItem(
				get_bg_point( data, 0 ),
				get_bg_point( data, 2 ),
				get_bg_point( data, 4 ) ).get_SP( );

		return ArcItem( ).get_SP( );
	}
	// ..........................................................................
	SP_BaseItem get_poly( const values& data )
	{
		if( data.size( ) > 3 )
		{
			PolyItem pitem;
			for( size_t pos= 0; pos < data.size( ); pos+= 2 )
				pitem+= get_bg_point( data, pos );

			return pitem.get_SP( );
		}
		return PolyItem( ).get_SP( );
	}
	// ..........................................................................
	// ..........................................................................
	class ParseGeneral : public ParseBase
		,public boost::enable_shared_from_this< ParseGeneral > //item
	{
	public:
		ParseGeneral( wrapper* pw, CHAR_PT name )
			:ParseBase( pw, name )
		{ }
		typedef boost::shared_ptr< ParseBase > SP_ParseGeneral;
		virtual SP_ParseGeneral new_sp( ) { return boost::make_shared< ParseGeneral >( *this ); }
		virtual void insert( const pair_type& pair );
	};
	// ..........................................................................
	class ParseObject : public ParseBase
		,public boost::enable_shared_from_this< ParseGeneral > //item
	{
		//auto& tmp= [ ](const wchar_t* l, const wchar_t* r) mutable { return wcscmp( l, r ) < 0; };
		struct comp {
			bool operator ( ) ( const wchar_t* l, const wchar_t* r ) const { return wcscmp( l, r ) < 0; }
		};
		typedef std::map< CHAR_PT, boost::function< SP_BaseItem ( const values& ) >,
			comp >omap_type;
		omap_type map;

	public:
		ParseObject( wrapper* pw, CHAR_PT name )
			:ParseBase( pw, name )
		{
			//should be static as several object could come along ?
			boost::assign::insert( map )
				( L"line", get_line )
				( L"arc", get_arc )
				( L"polyline", get_poly )
				;
			pw->psw_project->push_back( ItemSet( ) );
			pset= pw->psw_project->back_ptr( );
		}
		typedef boost::shared_ptr< ParseBase > SP_ParseObject;
		virtual SP_ParseObject new_sp( ) { return boost::make_shared< ParseObject >( *this ); }
		virtual void insert( const pair_type& pair );

	private:
		ItemSet* pset;
	};
	// ..........................................................................
	void ParseGeneral::insert( const pair_type& pair )
	{
		//general data is just one time so this is good enough
		if( pair.first == L"area" )
			set_bg_box( p_wrapper->psw_project->get_viewport( ), pair.second );
		else if( pair.first == L"line_width" )
			;//testing it does not exist
	}
	// ..........................................................................
	void ParseObject::insert( const pair_type& pair )
	{
		auto it= map.find( pair.first.c_str( ) );
		if( it == map.end( ) )
			return;
		SP_BaseItem sp_item= it->second( pair.second );
		pset->push_back( sp_item );
	}

	void wrapper::Init( )
	{
		using namespace boost::assign;
		insert( main )
			( tag_lookup.ttos( e_unknown ), ParseBase( this, tag_lookup.ttos( e_unknown ) ).new_sp( ) )
			( tag_lookup.ttos( e_general ), ParseGeneral( this, tag_lookup.ttos( e_general ) ).new_sp( ) )
			( tag_lookup.ttos( e_object ), ParseObject( this, tag_lookup.ttos( e_object ) ).new_sp( ) )
		;
	}
//	target dummy( std::tstring( L"dummy" ), 0, nullptr );

// ...........................................................................
//top end, from parser, 4 calls
// ...........................................................................
	//a push means there is more object data on the way, use the stack
	void wrapper::pspirit_push_dest( pair_type const& pair )
	{
		OT_CAD( "push_dest - stack_size: " << targets.size( ) << " vals: " << pair )

//		if( ! targets.size( ) )// must be in main
		{
			auto it= main.find( pair.first );
			if( it == main.end( ) )
			{
				it= main.find( tag_lookup.ttos( e_unknown ) );
				OT_CAD( " UNKNOWN from main: " << pair.first )
				targets.push( it->second ); //need to eat past unknowns
			}
			else
			{
				OT_CAD( "    push to main vals: " << pair )
				target t( it->second );
				targets.push( t );
			}
			//main has no data
			return;
		}

	}

// ...........................................................................
	//a post means a complete object is avaliable, no stack needed
	void wrapper::pspirit_post_dest( pair_type const& pair )
	{
		OT_CAD( "post_dest - stack_size: " << targets.size( ) << " vals: " << pair )
		OT_CAD( "    target name: " << targets.top( ).get_name( ) )
		assert( targets.size( ) );
		targets.top( ).p_target->insert( pair );
		D_KICAD( " post: " << pair << std::endl );
	}

// ...........................................................................
	//right prin ')' notification, check out the object
	void wrapper::pspirit_pop_dest( )
	{
		if( ! targets.size( ) ) //should be eof
		{
			return;
		}
		OT_CAD( "pop_dest - stack_size: " << targets.size( ) << " name: " << targets.top( ).get_name( ) )
		targets.pop( );
	}

// ...........................................................................
	void wrapper::pspirit_word_dest( const std::tstring& s )
	{
		OT_CAD( "word_dest - stack_size: " << targets.size( ) << " string: " << s )
			//todo, there is a dangling word in pcnew...
		D_KICAD( "dangling word: " << s << std::endl );
	}
// ...........................................................................
	//end top four calls
// ...........................................................................

}//namespace CadParser

//warning C4244
bool PostKicadParse( DrawingObects* pBoard )
{
	return true;
}
