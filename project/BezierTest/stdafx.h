// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define _SCL_SECURE_NO_WARNINGS

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <iomanip>

//for boost
#include <string>
#include <memory>
#include <locale>
#include <exception>

// ................
#include <typeinfo>

#include <vector>
#include <set>
#include <list>
#include <stack>

#include <vector>
#include <list>
#include <stack>

#include <boost/system/error_code.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_array.hpp>

#include <boost/filesystem.hpp>
namespace bfs= boost::filesystem;

#define BOOST_GEOMETRY
#ifdef BOOST_GEOMETRY
// BOOST GEOMETERY
#pragma warning(push)
#pragma warning(disable: 4244)      // we are mixing doubles and __int64
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/io/wkt/wkt.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/ring.hpp>
#include <boost/geometry/core/point_order.hpp> 
#pragma warning(pop)

namespace bg= boost::geometry;
namespace bgstrat = bg::strategy;

#endif // BOOST_GEOMETRY

// Optional includes to handle c-arrays as points, std::vectors as linestrings
//#include <boost/geometry/geometries/adapted/c_array_cartesian.hpp>
//#include <boost/geometry/geometries/adapted/std_as_linestring.hpp>

#include <boost/algorithm/string.hpp>    
namespace ba= boost::algorithm;

#define PI ( boost::math::constants::pi<double>( ) )//PI;

#define DXFLIB
#ifdef DXFLIB //using libdxf
#include <dxflib/dl_dxf.h>
#include <dxflib/dl_writer_ascii.h>

#else // not DXFLIB; using libdxfrw
#include <dxfrw/drw_entities.h>
#include <dxfrw\libdxfrw.h>
#include <dxfrw\drw_interface.h>

#endif //DXFLIB


// ......................................
typedef unsigned char       BYTE;
typedef unsigned int WORD;
typedef unsigned long DWORD;
typedef DWORD   COLORREF;
#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))



