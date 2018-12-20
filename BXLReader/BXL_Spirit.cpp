

#include "stdafx.h"
#include "BXL_Component.h"
#include "BXL_Decompress.h"
#include "BXL_Spirit.h"

#ifdef _DEBUG
std::ofstream spirit_os("spirit_os.txt");
#endif

#ifdef _DEBUG
	std::string get_blx_teststr();
	std::ofstream bxl_decompress_os("test_decompress.txt");
#endif

std::string ReadBXL( const bfs::path& path )
{
	std::ifstream in(path.string( ), std::ios::binary);
	BXLReader::Buffer reader(in);
	std::string result = reader.decode_stream();

#ifdef _DEBUG
	//visual dump
	std::ofstream os("./bxl_output.txt");
	os.write(result.c_str(), result.size());
#endif
	return result;
}

Component::ComponentStore ParseBXL(const std::string& buf)
{
	using namespace boost::spirit::x3;
	typedef std::string::const_iterator iterator_type;

	Component::ComponentStore comp;
	iterator_type begin = buf.begin();
	iterator_type end = buf.end();
	auto the_parser = Bridge::bxl;
	bool r = phrase_parse(begin, end, the_parser, space, comp);
	return comp;
}

