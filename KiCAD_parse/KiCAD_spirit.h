#pragma once




#if !defined(KICAD_H_2_2018)
#define KICAD_H_2_2018
#pragma once

#if defined(_DEBUG) && !defined(_LIB)
extern std::ofstream spirit_os;
#define BOOST_SPIRIT_X3_DEBUG_OUT spirit_os
#define BOOST_SPIRIT_X3_DEBUG
#endif

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/adapted/struct/adapt_struct_named.hpp>
#include <boost/fusion/include/adapt_struct_named.hpp>
namespace x3 = boost::spirit::x3;

namespace Bridge
{
	using namespace NSAssembly;
	size_t kicad_file_ver;
}

// Fusion Adapted ..........................................................
BOOST_FUSION_ADAPT_STRUCT(Bridge::Point,
	x,
	y
)

BOOST_FUSION_ADAPT_STRUCT(Bridge::PointEx,
	x,
	y,
	rot
)

BOOST_FUSION_ADAPT_STRUCT(Bridge::Line
	,a.x
	,a.y
	,b.x
	,b.y
, layer
, width
)
////Sure you can, see BOOST_FUSION_ADAPT_STRUCT_NAMED
BOOST_FUSION_ADAPT_STRUCT(Bridge::Rect,
	//Rect,  //adapted name..................
	a.x,
	a.y,
	b.x,
	b.y
)

BOOST_FUSION_ADAPT_STRUCT(Bridge::Arcc,
	origin.x,
	origin.y,
	radius,
	start_angle,
	sweep_angle,
	width
)

BOOST_FUSION_ADAPT_STRUCT(Bridge::Attribute,
	origin.x,
	origin.y,
	name,
	attr,
	bVisable,
	just,
	text_style
)

BOOST_FUSION_ADAPT_STRUCT(Bridge::TextStyle,
	width,
	height,
	char_width
)

BOOST_FUSION_ADAPT_STRUCT(Bridge::Designate,
	name,
	pos,
	text_style,
	just,
	bVisable
)

BOOST_FUSION_ADAPT_STRUCT(Bridge::ScPin,
	number,
	pos,
	length,
	rotate,
	width,
	bIsFlipped,
	bVisable,
	designate,
	name
)

BOOST_FUSION_ADAPT_STRUCT(Bridge::Layer,
	id,
	name,
	type,
	hide
)

BOOST_FUSION_ADAPT_STRUCT(Bridge::PcPad
	, number
, name
)

BOOST_FUSION_ADAPT_STRUCT(Bridge::KiPad
	, number
	, attrs
	, pos
	, size
	, drill
	, layers
	, net
)

BOOST_FUSION_ADAPT_STRUCT(Bridge::KiArc
	, center.x
	, center.y
	, radius.x
	, radius.y
	, layer
	, width
)

//BOOST_FUSION_ADAPT_STRUCT(Bridge::ScSymbol,
//	name
//	,origin.x
//	,origin.y
//	,pins
//	,draws
//	,attrs
//)

BOOST_FUSION_ADAPT_STRUCT(Bridge::CompText
	, layer
	//	,bVisable
	, ref
	, pos
	, text
	//font stuff
)

BOOST_FUSION_ADAPT_STRUCT(Bridge::PcComponent
	, name
	, layer
	, pos
	, comp_attrs
	, items
	//,textset
	//,lineset
	//, padset
)

BOOST_FUSION_ADAPT_STRUCT(Bridge::Segment
	, line.a.x
	, line.a.y
	, line.b.x
	, line.b.y
	, line.width
	, line.layer
	, line.net
)

//BOOST_FUSION_ADAPT_STRUCT_NAMED(Bridge::CompText, GR_TEXT
//	, text
//	, pos
//	, layer
//	, text
//	, size
//	, thick
//)
//

BOOST_FUSION_ADAPT_STRUCT(Bridge::GRText
	, text
	, pos
	, layer
	, size
	, thick
	, just
)

BOOST_FUSION_ADAPT_STRUCT(Bridge::KiAssemLine
	, a.x
	, a.y
	, b.x
	, b.y
	, angle
	, layer
	, width
)

BOOST_FUSION_ADAPT_STRUCT(Bridge::Zone
	, net
	, desig_poly
	, fill_polys
)

BOOST_FUSION_ADAPT_STRUCT(Bridge::KiVia
	, at
	, size
	, layers
	, net
)

BOOST_FUSION_ADAPT_STRUCT(Bridge::Assembly
	,file_ver
	,area
	,layers
	,components
	, assem_items
)

//struct qstring { std::string str; };
//BOOST_FUSION_ADAPT_STRUCT(qstring,
//	str
//)
/*
//inline std::ostream& operator << (std::ostream& os, Bridge::Point const& v) { return os << "x: " << v.x << " y: " << v.y; }
namespace boost {namespace spirit {namespace x3 { namespace traits {
inline std::ostream& operator << (std::ostream& os, Bridge::ScPin const& v) { return os << "ScPin: "<< v.name.name; }
inline std::ostream& operator << (std::ostream& os, Bridge::Attribute const& v) { return os << "Attribute: " << v.name; }
inline std::ostream& operator << (std::ostream& os, Bridge::Arcc const& v) { return os << "Arcc: " << v.origin.x << " " << v.origin.y; }
inline std::ostream& operator << (std::ostream& os, Bridge::Line const& v) { return os << "Line: " << v.a.x << " " << v.a.y; }

}}}}
*/

//traits for kleene over alternatives
//https://stackoverflow.com/questions/50125370/parse-kleene-operator-to-a-set-of-alternatives-adaptor-with-spirit-x3
namespace boost {namespace spirit {	namespace x3 {	namespace traits {
	using kicomp_subs = boost::variant<Bridge::CompText, Bridge::Line, Bridge::KiPad, Bridge::KiArc>;
	template<>
	struct container_value<Bridge::PcSubComp> {	using type = kicomp_subs; };
	template<>
	struct push_back_container<Bridge::PcSubComp> {
		using type = kicomp_subs;
		template <typename V>
		static bool call(Bridge::PcSubComp& comp, V&& v) {
			struct {
				Bridge::PcSubComp& _r;
				void operator()(Bridge::CompText const& v) const { _r.textset.push_back(v); }
				void operator()(Bridge::Line const& v) const { _r.lineset.push_back(v.get_SP()); }
				void operator()(Bridge::KiArc const& v) const { _r.lineset.push_back(v.get_SP()); }
				void operator()(Bridge::KiPad const& v) const { _r.padset.push_back(v);  }
			} vis{ comp };
			boost::apply_visitor(vis, v);
			return true;
		}
	};
	using type_kiboard = boost::variant<Bridge::Line, Bridge::KiArc, Bridge::Segment, Bridge::Zone, Bridge::GRText, Bridge::KiVia>;
	template<>
	struct container_value<Bridge::KiBoardItems> { using type = type_kiboard; };
	template<>
	struct push_back_container<Bridge::KiBoardItems> {
		using type = type_kiboard;
		template <typename V>
		static bool call(Bridge::KiBoardItems& comp, V&& v) {
			struct {
				Bridge::KiBoardItems& _r;
				void operator()(Bridge::Line const& v) const { _r.lineset.push_back(v.get_SP()); }
				void operator()(Bridge::KiArc const& v) const { _r.lineset.push_back(v.get_SP()); }
				void operator()(Bridge::Segment const& v) const { _r.segments.push_back(v); }
				void operator()(Bridge::Zone const& v) const { _r.zones.push_back(v); }
				void operator()(Bridge::GRText const& v) const { _r.texts.push_back(v); }
				void operator()(Bridge::KiVia const& v) const { _r.vias.push_back(v); }
			} vis{ comp };
			boost::apply_visitor(vis, v);
			return true;
		}
	};
}}}}

namespace Bridge {

	using namespace boost::spirit::x3;
#define DEF_RULE( RuleName, Attr ) static auto const RuleName = rule<struct Attr##_def, Attr>( #RuleName )
	//............

	struct true_false_table : x3::symbols<bool> {
		true_false_table() {
			add("False", false)
				("True", true)
				;
		}
	}const true_false;

	struct hide_table : x3::symbols<bool> {
		hide_table() {
			add("", false)
				("hide", true)
				;
		}
	}const hide;

	//not used yet?
	struct justify_table : x3::symbols<JUSTIFY> {
		justify_table() {
			add("Center", JUSTIFY::eCenter)
				("Left", JUSTIFY::ejLeft)
				("Right", JUSTIFY::ejRight)
				("Upper", JUSTIFY::ejUpper)
				("Lower", JUSTIFY::ejLower)
				("UpperLeft", JUSTIFY::ejUpperLeft)
				("UpperRight", JUSTIFY::ejUpperRight)
				("LowerLeft", JUSTIFY::ejLowerLeft)
				("LowerRight", JUSTIFY::ejLowerRight)
				("mirror", JUSTIFY::ejMirror)
				;
		}
	}const justify;

	auto const bare_word = lexeme[+char_("a-zA-Z0-9.,/_+:^*-")];

	static auto const quoted_string = rule<struct _, std::string>("quoted_string") = '"' >> lexeme[*(~char_('"'))] >> '"';
	auto const prin_word = "(" >> bare_word;
	auto const prin_skip = omit[*(char_ - ')') >> ')'];
	//auto const hide = rule<struct _, bool>("hide_true_false") = lit("hide");
	//temp for nestled skiping.....
	//https://stackoverflow.com/questions/48392993/how-to-make-a-recursive-rule-in-boost-spirit-x3-in-vs2017
	rule<struct foo_class> const foo = "foo";
	rule<struct bar_class> const bar = "bar";
	auto bar_def = omit[*~char_("()")] >> -*foo;
	auto foo_def = '(' >> bar >> ')';
	BOOST_SPIRIT_DEFINE(foo);
	BOOST_SPIRIT_DEFINE(bar);

	// ......................................................................................................
	DEF_RULE(at_pos_rot, PointEx) = "(at" >> double_ >> double_ >> -double_ >> ')';
	DEF_RULE(size, Point) = "(size" >> double_ >> double_ >> ')';

	auto skip_atr = lit('(') >> omit[*(char_ - char_(')'))] >> lit(')');
	auto const omit_prin_word = omit['(' >> bare_word];
	auto const rect = rule<_, Rect>("rect") = omit_prin_word >> double_ >> double_ >> double_ >> double_ >> ')';
	auto const list_layer = rule<struct Layer_def, Layer>("layer") = '(' >> int_ >> bare_word >> bare_word >> -hide >> ')';
	auto optional = rule<_, int>() = int_ | ("\"\"");//?? or int won[t initialize to '0' on quotes....

	auto const comp_text = rule<struct CompText_def, CompText>("comp_text")
		= lit("(fp_text") >> bare_word >> bare_word >> at_pos_rot
			>> "(layer" >> bare_word >> ')' >> -lit("hide") >> foo_def >> ')';

	auto const fp_line = rule<struct Kfp_line_def, Line>("fp_line") = lit("(fp_line") >> "(start" >> double_ >> double_
		>> ')' >> "(end" >> double_ >> double_ >> ')' >> "(layer" >> bare_word >> ')' >> "(width" >> double_ >> "))";

	// (gr_line (start 123.825 132.715) (end 168.275 132.715) (angle 90) (layer Edge.Cuts) (width 0.127)
	auto const gr_line = rule<struct gr_line_def, KiAssemLine>("gr_line") =
		lit("(gr_line") >> "(start" >> double_ >> double_ >> ')'
		>> "(end" >> double_ >> double_ >> ')' >> "(angle " >> double_ >> ')'
		>> "(layer" >> bare_word >> ')' >> "(width" >> double_ >> "))";

	auto const ki_circle = rule<struct ki_circle_def, KiArc>("fp_circle") =
		"(center" >> double_ >> double_ >> ')'
		>> "(end" >> double_ >> double_ >> ')'
		>> "(layer" >> bare_word >> ')' >> "(width" >> double_ >> ')' >> ')';
	
	auto const fp_circle = lit("(fp_circle") >> ki_circle;
	auto const gr_circle = lit("(gr_circle") >> ki_circle;

	// (gr_text GND (at 162.56 81.28) (layer Dessus) (effects(font(size 1.524 1.524) (thickness 0.381)))	)
	auto const gr_text = rule<_, GRText>("gr_text") =
		lit("(gr_text") >> (bare_word | quoted_string) >> at_pos_rot
		>> "(layer" >> bare_word >> ')' >> "(effects" >> "(font" >> size
		>> "(thickness" >> double_ >> "))" >> -("(justify" >> justify >> ')') >> ')' >> ')';

	auto net = rule<struct net_def, net_pair>() = "(net" >> int_ >> (bare_word | quoted_string) >> ')';

	auto pad = rule<struct pad_def, KiPad>("KiPad") = "(pad" >> optional//( int_ | "\"\"")
		>> *bare_word >> at_pos_rot >> size >> -("(drill" >> double_ >> ')')
		>> "(layers" >> *bare_word >> ')' >> -net
		>> ')';

	auto comp_attrs = rule<_, attr_pair>("comp_attrs") =
		'(' >> bare_word >> (bare_word | quoted_string) >> ')';

	auto comp = rule<struct comp_def, PcComponent>("component") =
		"(module" >> bare_word >> "(layer" >> bare_word
		>> *(prin_skip - "(at") >> at_pos_rot
		>> *comp_attrs
		>> *(pad | comp_text | fp_line | fp_circle)
		//>> *(fp_line | fp_circle )
		>> *foo_def >> ')'
		;

	//  (segment (start 115.57 86.36) (end 121.92 86.36) (width 0.25) (layer F.Cu) (net 1) (tstamp 5C151114) (status 800000))
	auto seg_line = rule<struct Segment_def, Segment>("segment") =
		lit("(segment") >> "(start" >> double_ >> double_ >> ')'
		>> "(end" >> double_ >> double_ >> ')'
		>> "(width" >> double_ >> ')'
		>> "(layer" >> bare_word >> ')'
		>> "(net" >> int_ >> ')'
		>> *foo_def >> ')';

	//   (via (at 177.8 81.28) (size 1.778) (layers Dessus Dessous) (net 13))
	auto via = rule<_, KiVia>("via") = 
		lit("(via") >> at_pos_rot >> "(size" >> double_ >> ')'
		>> "(layers" >> *bare_word >> ')' >> "(net" >> int_ >> "))";

	auto zone = rule<struct Zone_def, Zone>("zone") =
		lit("(zone") >> "(net" >> int_ >> ')' >> *(foo_def - "(polygon")//skip descriptors for now
		>> "(polygon" >> "(pts" >> *("(xy" >> double_ >> double_ >> ')') >> ')' >> ')'
		>> *(lit("(filled_polygon") >> "(pts" >> *("(xy" >> double_ >> double_ >> ')') >> ')' >> ')' ) >> ')'
		;

	auto dimension = rule<_>("unused")= lit("(dimension") >> omit[double_] >> bar >> ')';

	auto kicad = rule<_, Assembly>("kicad") =
		lit("(kicad_pcb") >> lit("(version")
		>> int_ >> ')' >> omit[lexeme[*(char_ - eol)]]

		//assume well formed, get area from general
		>> lit("(general") >> *(prin_skip - "(area")
		>> rect
		>> *(prin_skip - "(layers") >> "(layers" >> *list_layer
		//for now, skip the setup and net list
		>> *(prin_skip - "(module")
		>> *comp
		//and the stuff after comp, can't assume well formed....
		>> *(seg_line | gr_text | gr_line | gr_circle | zone | dimension | via)
		;

} // namespace Bridge

#endif// KICAD_H_2_2018
