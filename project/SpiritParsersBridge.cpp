
//these are the interfaces between spirit and the s expression destinations

#include "spirit.h"
#define CAD_PARSE_SPIRIT
#include "CADParse.h"
#include "SpiritParsers.h"

bool CadParse( std::ifstream& in, DrawingObects* pBoard )
{
	using boost::spirit::ascii::space;
	using namespace CadParser;
	namespace spirit= boost::spirit;

	in.unsetf(std::ios::skipws);

	spirit::istream_iterator begin( in );
	spirit::istream_iterator end;

	typedef spirit::istream_iterator iterator_type;
	wrapper wrapper( pBoard );

	CadParser::skipper<iterator_type> skipper;
	typedef CadParser::parser< iterator_type > cad_parser;
	cad_parser p;

	bool r= qi::phrase_parse( begin, end, p, skipper, wrapper );

	if( r )
		return PostKicadParse( pBoard );

	return r;
}

bool Accel15Parse( std::ifstream& in, DrawingObects* pBoard )
{
	using boost::spirit::ascii::space;
	using namespace CadParser;
	namespace spirit= boost::spirit;

	in.unsetf(std::ios::skipws);

	// with istream
	spirit::istream_iterator begin( in );
	spirit::istream_iterator end;

	typedef spirit::istream_iterator iterator_type;
	wrapper wrapper( pBoard );

	CadParser::skipper<iterator_type> skipper;
	CadParser::parser< iterator_type > cad_parser;
	
	bool r= qi::phrase_parse( begin, end, cad_parser, skipper, wrapper );

	if( r )
		return PostKicadParse( pBoard );

	return r;
}

// ...........................................................................
namespace boost {

	std::wostream& operator<<(std::wostream& out, const value& v )
	{
		switch( v.which( ) )
		{
		case 0:
			out << "i:" << boost::get< unsigned int >( v );
			break;

		case 1:
			out << "d:" << boost::get< double >( v );
			break;

		case 2:
			out << "s:" << boost::get< std::tstring >( v );
		}//switch
		return out;
	}

	std::wostream& operator<<(std::wostream& out, const values& v )
	{
		out << "size: " << v.size( ) << " '";
		for(auto it= v.begin( ); it != v.end( ); ++it )
		{
			out << *it << " ";
		}
		out << "' ";
		return out;
    }

    std::wostream& operator<<(std::wostream& out, const pair_type& v)
	{
		out << " '" << v.first << "' ";
		if( v.second.size( ) )
			out << " v: " << v.second;
		else
			out << "values vector is empty";

		return out;
    }
}


