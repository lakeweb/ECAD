

#if !defined(BXL_SPIRIT_SEPT_15_2018)
#define BXL_SPIRIT_SEPT_15_2018
#pragma once

#ifdef _DEBUGx
extern std::ofstream spirit_os;
#define BOOST_SPIRIT_X3_DEBUG_OUT spirit_os
#define BOOST_SPIRIT_X3_DEBUG
#endif

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/adapted/std_pair.hpp>

namespace x3 = boost::spirit::x3;

namespace Bridge
{
	using namespace Component;
}

// Fusion Adapted ..........................................................
BOOST_FUSION_ADAPT_STRUCT(Bridge::Point,
	x,
	y
)

BOOST_FUSION_ADAPT_STRUCT(Bridge::Line,
	a.x,
	a.y,
	b.x,
	b.y,
	width
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

//BOOST_FUSION_ADAPT_STRUCT(Bridge::ScSymbol,
//	name
//	,origin.x
//	,origin.y
//	,pins
//	,draws
//	,attrs
//)

BOOST_FUSION_ADAPT_STRUCT(Bridge::ComponentStore,
	text_set,
	symbol
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
//ScSymbol
namespace boost { namespace spirit { namespace x3 { namespace traits {
	//using namespace Bridge;
	template<>
	struct container_value<Bridge::ScSymbol> {
		using type = boost::variant<Bridge::Line, Bridge::Arcc, Bridge::ScPin,Bridge::Attribute>;
	};
	template <>
	inline void move_to(Bridge::Point&& src, Bridge::ScSymbol& dest){
		dest.origin = src;
	}
	template <>
	inline void move_to(std::string&& src, Bridge::ScSymbol& dest){
		dest.name= std::move(src);
	}
	template<>
	struct push_back_container<Bridge::ScSymbol> {
		template <typename V>
		static bool call(Bridge::ScSymbol& c, V&& v) {
			struct {
				Bridge::ScSymbol& _r;
				void operator()(Bridge::ScPin const& v) const { _r.pins.emplace_back(v); }
				void operator()(Bridge::Line const&v) const { _r.draws.push_back(v.get_SP()); }
				void operator()(Bridge::Arcc const&v) const { _r.draws.push_back(v.get_SP()); }
				void operator()(Bridge::Attribute const&v) const { _r.attrs.push_back(v); }
			} vis{ c };
			boost::apply_visitor(vis, v);
			return true;
		}
	};
}}}} //namespaces....

// Spirit ...............................................................
namespace Bridge {

	using namespace boost::spirit::x3;
#define DEF_RULE( RuleName, Attr ) static auto const RuleName = rule<struct Attr##_def, Attr>( #RuleName )
	//............
	//Utiliies
	auto const bare_word
		= lexeme[+char_("a-zA-Z0-9.,/_+*-")];

	//auto const quoted_string = '"' >> lexeme[*(~char_('"'))] >> '"';
	static auto const quoted_string = rule<struct _, std::string>("quoted_string")= '"' >> lexeme[*(~char_('"'))] >> '"';

	//(Attr "Type" "")
	auto const complex_string = quoted_string;

	struct true_false_table : x3::symbols<bool> {
		true_false_table() {
			add("False", false)
				("True", true)
				;
		}
	}const true_false;

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
				;
		}
	}const justify;

	// ..........
	//not parsing yet so omit..........
	static auto const padstack_rule =
		"PadStack" >> omit[*(bare_word - lit("EndPadStack") | +char_(":,()\""))] >> "EndPadStack";

	static auto const pattern_rule =
		"Pattern" > omit[*(bare_word - lit("EndPattern") | +char_(":,.()=*\"-"))] > "EndPattern";


	// ..........
	//Component Rules
	// ..........
	DEF_RULE(prin_bg_point, Point)
		= '(' >> int_ >> ',' >> int_ >> ')'
		;
	DEF_RULE(bare_bg_point, Point)
		= int_ >> ',' >> int_
		;
	DEF_RULE(textstyle_sub, TextStyle) =
		lit("(FontWidth") >> int_ >> ')'
		>> "(FontHeight" >> int_ >> ')'
		>> -("(FontCharWidth" >> int_ >> ')')
		;
	static auto const textstyle_rule =
		"TextStyle" >> quoted_string >> textstyle_sub
		;
	//Attribute (Origin 0, 0) (Attr "C" "Copyright (C) 2015 Accelerated Designs. All rights reserved") (Justify Center) (TextStyleRef "COPYRIGHT") 
	DEF_RULE(attribute_rule, Attribute) =
		lit("Attribute") >> "(Origin" >> int_ >> ',' >> int_ >> ')'
		>> "(Attr" >> quoted_string >> quoted_string >> ')'
		> -("(IsVisible" >> true_false >> ')')
		> "(Justify" >> justify >> ')'
		> "(TextStyleRef" >> quoted_string >> ')'
		;

	// Line (Origin -500, 100) (EndPoint -500, 100) (Width 5) 
	DEF_RULE(line_rule, Line) =
		lit("Line") >> "(Origin" >> double_ >> ',' >> double_ >> ')'
		> "(EndPoint" >> double_ >> ',' >> double_ >> ')'
		> "(Width" >> int_ >> ')'
		;

	// Arc (Origin -599.857887335384, -199.412907287256) (Radius 4) (StartAngle 293.1986) (SweepAngle 1.263279E-05) (Width 5) 
	//static auto const rule<struct attribute_rule_def, Component::Arc> arc_rule("arc_rule")=
	DEF_RULE(arc_rule, Arcc) =
		lit("Arc") >> "(Origin" >> double_ >> ',' >> double_ >> ')'
		> "(Radius" >> double_ >> ')'
		> "(StartAngle" >> double_ >> ')'
		> "(SweepAngle" >> double_ >> ')'
		> "(Width" >> int_ >> ')'
		;
	DEF_RULE(designate, Designate) =
		quoted_string
		> prin_bg_point
		> "(TextStyleRef" >> quoted_string >> ')'
		> "(Justify" >> justify >> ')'
		> -("(IsVisible" >> true_false >> ')')
		;
	DEF_RULE(pre_pin,int) = lit("Pin") >> "(PinNum" >> int_ >> ')';
	DEF_RULE(sch_pin_rule, ScPin) =
		pre_pin
		>> "(Origin" >> bare_bg_point >> ')'
		>> "(PinLength" >> double_ >> ')'
		>> -("(Rotate" >> double_ >> ')')
		>> "(Width" >> double_ >> ')'
		>> -("(IsFlipped" >> true_false >> ')')
		>> "(IsVisible" >> true_false >> ')'
		> "PinDes" >> designate
		> "PinName" >> designate
		;

//	DEF_RULE(symbol_items_rule, traits::container_value) = +(sch_pin_rule | line_rule | attribute_rule);
	DEF_RULE(symbol_rule, ScSymbol) =
		//"Symbol" > omit[quoted_string]
		"Symbol" > quoted_string
		>> "OriginPoint" >> prin_bg_point// lit('(') >> int_ >> ',' >> int_ >> ')'
		>> "OriginalName" >> (omit[lit('"') >> lit('"')] | omit[quoted_string])
		>> "Data" >> lit(":") >> omit[int_]
		>> *(line_rule | arc_rule | sch_pin_rule | attribute_rule)
		//> "EndData" > "EndSymbol"
		;

	//so far, just looking to pull out the symbol
	DEF_RULE(bxl, ComponentStore) =
		*textstyle_rule >> *padstack_rule >> *pattern_rule >> symbol_rule;
	//DEF_RULE(bxl, ComponentStore) =
	//	*textstyle_rule >> *padstack_rule >> *pattern_rule >> symbol_rule;

} // namespace Bridge

#endif// BXL_SPIRIT_SEPT_15_2018
