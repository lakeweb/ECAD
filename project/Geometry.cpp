
#include "stdafx.h"
#include "debug.h"
#ifdef TEST_PARSER
#include <loc_globals.h>
#endif
#include "Geometry.h"


//convert two doubles as string, comma seperated, to bg_point
bg_point get_point(std::string& in)
{
	bg_point point;
	std::wstring::size_type pos = in.find(L',');
	assert(pos != std::wstring::npos);

	point.set_x(boost::lexical_cast< GEO_NUM >(std::string(in.begin(), in.begin() + pos)));
	point.set_y(boost::lexical_cast< GEO_NUM >(std::string(in.begin() + pos + 1, in.end())));
	return point;
}

