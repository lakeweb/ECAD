

// ...........................................................................
// SHARED RESOURCES
// ...........................................................................
namespace SpiritShare
{
	//all reals will have a decimal point, pcnew rule
	//No exponent is done because of the likes of word: '3E15900'
	template <typename T>
	struct ts_real_policies : boost::spirit::qi::strict_real_policies<T>
	{
		// No exponent
		template <typename Iterator>
		static bool
		parse_exp( Iterator& first, Iterator const& last )
		{
			return false;
		}
	};

    namespace qi = boost::spirit::qi;
	using qi::char_;
	using qi::double_;
	using qi::lit;
	using qi::alpha;
	using qi::_val;
	using qi::omit;
	using qi::no_skip;
	using qi::int_;
	using qi::_val;
	using qi::_1;
	using qi::on_error;

}//namespace SpiritShare

// ...........................................................................
//CadParser
// ...........................................................................
BOOST_PHOENIX_ADAPT_FUNCTION( void, pspirit_pop_dest, pspirit_pop_dest_expr, 1 )
BOOST_PHOENIX_ADAPT_FUNCTION( void, pspirit_push_dest, pspirit_push_dest_expr, 2 )
BOOST_PHOENIX_ADAPT_FUNCTION( void, pspirit_post_dest, pspirit_post_dest_expr, 2 )
BOOST_PHOENIX_ADAPT_FUNCTION( void, pspirit_word_dest, pspirit_word_dest_expr, 2 )

// ...........................................................................
namespace CadParser     //  The skipper grammar
{
	using namespace SpiritShare;

    template <typename Iterator>
    struct skipper : qi::grammar<Iterator>
    {
        skipper() : skipper::base_type( skip )
        {
			qi::eol_type eol;

			comment=
				"#"
				>> *( char_ - eol )
				>> eol
                ;

			skip=
			comment
			| char_( " \x09\x0a\0x0d" )
			;
		}
private:		
        qi::rule<Iterator> skip;
        qi::rule<Iterator> comment;
    };
}

// ...........................................................................
namespace CadParser     //  The grammar
{
	using namespace SpiritShare;

	template <typename Iterator>
	struct parser : qi::grammar< Iterator, wrapper( ), skipper< Iterator > >
	{
        parser( )
            :parser::base_type( start )
		{
			//main parser
			start=
				//assume well formed top line
                lit("(HE_CAD") >> lit( "(version")
				>>  double_ [ bind( &CadParser::wrapper::version, _val )= _1 ]
				>> ')'
				//parse the rest...
				>> *( prop			[ pspirit_push_dest( _val, _1 ) ]
					| pair			[ pspirit_post_dest( _val, _1 ) ]
					| lit(')')		[ pspirit_pop_dest( _val ) ]
//					| s_skip		[ pspirit_pop_dest( _val ) ]
					| word			[ pspirit_word_dest( _val, _1 ) ] //dangling attributes are rare but exist
					)
			;
			prop= ( '(' >> word >> parse_data >> & lit('(') );
			pair= '(' >> word >> parse_data >> ')';
			word= ( quoted_string | +( char_ - char_( "() \x09\x0a\0x0d\"" ) ) );

			parse_data= *( uint_p | double_p | word );
			quoted_string= omit [ char_('\"') ] >> no_skip [ *(char_ - char_('\"') )  ] >> lit( '\"' );
			uint_p= int_ >> & ! ( alpha | char_("-,+._") );
			double_p= ts_real >> & ! alpha;
			s_skip= *( qi::char_( "() \x09\x0a\0x0d\"" ) - ')' ) >> ')';

#if 0
//			BOOST_SPIRIT_DEBUG_NODE( start );
			BOOST_SPIRIT_DEBUG_NODE( pair );
			BOOST_SPIRIT_DEBUG_NODE( prop );
			BOOST_SPIRIT_DEBUG_NODE( word );
			BOOST_SPIRIT_DEBUG_NODE( s_skip );
			BOOST_SPIRIT_DEBUG_NODE( quoted_string );
#endif
		}
private:
		//with skipper
		qi::rule< Iterator, wrapper( ), skipper< Iterator > > start;
        qi::rule< Iterator, pair_type( ), skipper< Iterator > > pair;
		qi::rule< Iterator, pair_type( ), skipper< Iterator > > prop;
		qi::rule< Iterator, values( ), skipper< Iterator > > parse_data;
		qi::rule< Iterator, skipper< Iterator > > s_skip;
		//without skipper
		qi::rule< Iterator, std::tstring( ) > word;
		qi::rule< Iterator, unsigned int( ) > uint_p;
		qi::real_parser< double, ts_real_policies< double > > ts_real;
		qi::rule< Iterator, double( ) > double_p;
		qi::rule< Iterator, std::tstring( ) > quoted_string;
	};
}

