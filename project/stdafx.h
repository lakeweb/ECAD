
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once
#define _SCL_SECURE_NO_WARNINGS

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#define _INTSAFE_H_INCLUDED_
#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#else
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars
#include <Richedit.h>

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <list>
#include <stack>
#include <unordered_map>
#include <thread>

//for boost
#include <string>
#include <memory>
#include <locale>
#include <exception>

// ................
#include <typeinfo>

// ..........................................................................

#include <boost\unordered_map.hpp>
#include <boost/system/error_code.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/smart_ptr/detail/sp_nullptr_t.hpp>

#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;
using bfs_vect_type = std::vector<bfs::path>;

#include <boost/algorithm/string.hpp>

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
#include <boost/geometry/geometries/multi_polygon.hpp>
#include <boost/geometry/geometries/adapted/boost_tuple.hpp>
#pragma warning(pop)

// Optional includes to handle c-arrays as points, std::vectors as linestrings
//#include <boost/geometry/geometries/adapted/c_array_cartesian.hpp>
//#include <boost/geometry/geometries/adapted/std_as_linestring.hpp>
namespace bg= boost::geometry;
namespace bgstrat = bg::strategy;

#include <boost/any.hpp>
typedef std::list<boost::any> boost_many;
#include <boost/variant.hpp>
#include <boost/tokenizer.hpp>

#include <boost/program_options.hpp>
namespace po= boost::program_options;

#include <boost/algorithm/string.hpp>    
namespace ba= boost::algorithm;

#include <boost/unordered_map.hpp>
#include <boost/assign/list_inserter.hpp> // for 'insert()'

#include <boost/range/algorithm/find_if.hpp>

#include "typeswitch.h"
#include <boost/variant.hpp>

//server access
//#include <boost/asio.hpp>
//#include <boost/bind.hpp>
//#include <boost/exception/exception.hpp>


#define PI ( boost::math::constants::pi<double>( ) )//PI;

#define USE_WINDOWS_EX
//#include <ctrlext.h>

//#include <std_share.h>

#include <xml/xml.h>
using namespace XML;
#define to_utf8 pugi::as_utf8

#include <BXL_Component.h>
//#include "../BXLReader/BXL_Decompress.h"
//#include "../BXLReader/BXL_Spirit.h"


#include <globals.h>
#include "gear_gen.h"

#include <dxflib/dl_dxf.h>
#include <dxflib/dl_writer_ascii.h>

//this is here so avaliable globaly app and xmlsettingsstore
AFX_STATIC_DATA LPCTSTR DUMMY_REGISTRY_PATH= _T("DummyPCBRegistryPath");
AFX_STATIC_DATA LPCTSTR USER_SETTINGS_FILENAME= _T("settings.xml");

//#ifdef _DEBUG
inline std::ostream& operator <<(std::ostream& os, const CSize& r) { return os << r.cx << " " << r.cy; }
inline std::ostream& operator <<(std::ostream& os, const CRect& r) { return os << r.left << " " << r.top << " " << r.right << " " << r.bottom; }
//#endif

