
//F isolated precompiled header for compile time cost

#define BOOST_SPIRIT_UNICODE
#define BOOST_SPIRIT_USE_PHOENIX_V3 1
#ifdef _DEBUG
#define BOOST_SPIRIT_DEBUG
#define BOOST_SPIRIT_DEBUG_OUT std::cout

#endif
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/adapted/struct.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include <boost/phoenix/function/adapt_function.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include <stdio.h>
#include <tchar.h>
#include <fstream>

//for boost
#include <string>
#include <memory>
#include <locale>
#include <iostream>
#include <exception>

#include <vector>
#include <list>
#include <map>
#include <stack>

//#define BOOST_SPIRIT_UNICODE
//#define BOOST_SPIRIT_USE_PHOENIX_V3 1
//#ifdef _DEBUG
//#define BOOST_SPIRIT_DEBUG
//#endif
//#include <boost/fusion/include/boost_tuple.hpp>
//#include <boost/spirit/include/qi.hpp>
//#include <boost/spirit/include/karma.hpp>
//#include <boost/spirit/include/phoenix.hpp>
//#include <boost/spirit/include/phoenix_function.hpp>
//#include <boost/spirit/include/support_istream_iterator.hpp>
//
//#include <boost/phoenix/function/adapt_function.hpp>
//#include <boost/fusion/adapted/struct.hpp>
//#include <boost/fusion/include/std_pair.hpp>
//#include <boost/detail/lightweight_test.hpp>
//
//using debug
//#include <boost/spirit/home/support/unused.hpp>
//#include <boost/spirit/home/qi/nonterminal/rule.hpp>
//#include <boost/spirit/home/qi/nonterminal/debug_handler_state.hpp>
//#include <boost/spirit/home/qi/operator/expect.hpp>

// ..........................................................................
//http://stackoverflow.com/questions/2055849/visual-c-migrating-traditional-c-and-c-string-code-to-a-unicode-world
namespace std
{

#ifdef _MSC_VER

#ifdef UNICODE
typedef             wstring                         tstring ;
typedef             wistream                        tistream ;
// etc.
#else // Not UNICODE
typedef             string                          tstring ;
typedef             istream                         tistream ;
// etc.
#endif

#endif

} // namespace std

