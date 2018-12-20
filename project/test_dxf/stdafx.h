// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <iostream>
#include <vector>
#include <set>


#include <boost/algorithm/string.hpp>    
namespace ba= boost::algorithm;

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

#include <boost/filesystem.hpp>
namespace bfs= boost::filesystem;

#include <boost/lexical_cast.hpp>

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

// Optional includes to handle c-arrays as points, std::vectors as linestrings
//#include <boost/geometry/geometries/adapted/c_array_cartesian.hpp>
//#include <boost/geometry/geometries/adapted/std_as_linestring.hpp>
namespace bg= boost::geometry;
namespace bgstrat = bg::strategy;



#include <dxflib/dl_dxf.h>
#include <dxflib/dl_writer_ascii.h>


#define PI ( boost::math::constants::pi<double>( ) )//PI;
bool StartDXF_write( DL_Dxf* dxf, DL_WriterA* dw );
bool EndDXF_write( DL_Dxf* dxf, DL_WriterA* dw );


#include <Geometry.h>

// ......................................
typedef unsigned char       BYTE;
typedef unsigned int WORD;
typedef unsigned long DWORD;
typedef DWORD   COLORREF;
#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
