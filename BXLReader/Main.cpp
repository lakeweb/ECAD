

#include "stdafx.h"

#include "BXL_Component.h"
#include "BXL_Decompress.h"
#include "BXL_Spirit.h"

#include "testing.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//std::string get_blx_teststr();
//std::ofstream bxl_decompress_os("test_decompress.txt");

extern const char* DIODE_SOT23_A_test;
extern const char* in_Lines;

template< typename Attr, typename Parser >
bool test_parser(const Parser& p, Attr& attr, const char* str)
{
	//std::cout << "test parser: " << p.name << std::endl;
	std::string string(str);
	auto b = string.begin();
	bool r = boost::spirit::x3::phrase_parse(b, end(string), p, boost::spirit::x3::space, attr);
	std::cout << "Remain: " << std::string(b, end(string) ) << std::endl << std::endl;
	return r;
}

using namespace boost::spirit::x3;
using namespace Bridge;

#if 1
int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	auto the_parser = Bridge::bxl;

	std::ifstream in(R"(..\project\test_files\LT1218CN8&hash_PBF.bxl)", std::ios::binary);
	std::string bxl_text;

	//decompress the bxl file
	{
		BXLReader::Buffer reader( in );
		std::string result = reader.decode_stream( );

		//visual dump
		std::ofstream os("./bxl_output.txt");
		os.write(result.c_str(), result.size());

		bxl_text = result;
	}

	std::cout << "\n\n" << "parse the BXL\n\n";

	using namespace boost::spirit::x3;
	typedef std::string::const_iterator iterator_type;

	Component::ComponentStore comp;
	iterator_type begin = bxl_text.begin();
	iterator_type end = bxl_text.end();
	bool r = phrase_parse(begin, end, the_parser, space, comp);
	//std::cout << std::boolalpha << "pass: " << r << std::endl;

//..............................
	//dump the compoment
	std::cout << std::endl << "dump:\n" << comp.symbol.name << "\n\ntext:\n";

	for (auto& i : comp.text_set)
		std::cout << i.first << " " << i.second.width << std::endl;

	std::cout << '\n' << "pins:\n";
	for (auto& i : comp.symbol.pins)
		std::cout << i.number << " " << i.name << " " << i.name.name << std::endl;

	std::cout << '\n' << "lines:\n";
	for (auto& i : comp.symbol.draws)
	{
		//this only works because we know there are only lines in MSP430G2253_PW_20.bxl
		Line* pline = dynamic_cast<Line*>(i.get( ));
		std::cout << "a: " << pline->a << " b: " << pline->b << " width: " << pline->width << std::endl;
	}
	return 0;
}
#endif

std::string get_blx_teststr()
{
	//std::ifstream in(R"(.\test_files\MC68HC05F12FU80.bxl)", std::ios::binary);
	//std::ifstream in(R"(.\test_files\LM10CLH.bxl)", std::ios::binary);
	std::ifstream in(R"(.\test_files\MSP430G2253_PW_20.bxl)", std::ios::binary);
	std::string bxl_text;

	//decompress the bxl file
	BXLReader::Buffer reader(in);
	std::string result = reader.decode_stream();

	return result;
}

#if 0
int main()
{
	{
		Attribute attr;
		test_parser(attribute_rule, attr,
			R"(Attribute (Origin 0, 0) (Attr "C" "Copyright (C) 2015 Accelerated Designs. All rights reserved") (Justify Center) (TextStyleRef "COPYRIGHT") )");

		DrawContainer dc;
		test_parser(*(line_rule | arc_rule), dc, in_Lines);

		ScSymbol sym;
		test_parser(symbol_rule, sym, &*std::string(DIODE_SOT23_A_test).begin());
	}
	return 0;
}
#endif

#if 0
int main()
{
	std::string str(in_Lines);
	//std::string str(get_blx_teststr());

	using namespace boost::spirit::x3;
	using namespace Bridge;

	//ScSymbol con;
	//DrawContainer dc;
	//bool r = phrase_parse(begin(str), end(str), *(sch_pin_rule | line_rule | arc_rule | attribute_rule), space, dc);
	//std::cout << std::boolalpha << "pass: " << r << std::endl;

}

#elif 0 //testing just line rule

int main()
{
	//	std::string test_str(DIODE_SOT23_A_test);
	std::string test_str(in_Lines);

	using namespace boost::spirit::x3;
	using namespace Bridge;

	std::cout << "sequence_size: " << x3::detail::sequence_size<decltype(Bridge::symbol_rule), x3::unused_type>::value << std::endl;
	std::cout << boost::fusion::result_of::size<x3::traits::attribute_of<decltype(Bridge::symbol_rule), x3::unused_type>::type>::value << std::endl;
	typedef std::string::const_iterator iterator_type;
	Component::ComponentStore comp;

	Component::DrawContainer draws;
	Component::ScPinVect pins;
	auto attr = std::tie(draws, pins);

	x3::variant<Component::DrawContainer, Component::ScPinVect>vars;

	iterator_type begin = test_str.begin();
	iterator_type end = test_str.end();
	bool r = phrase_parse(begin, end, *line_rule, space, draws);
	std::cout << std::boolalpha << "pass: " << r << std::endl;

}
#endif