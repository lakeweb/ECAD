


//mouse wheel
#define MWTRACE TRACE
//#define MWTRACE __noop

//show center of window
#define SHOW_CENTER

#define DRAWTRACE __noop

//xml settings store
#define XMLSTRACE TRACE
//#define XMLSTRACE __noop

//geometry
//#define GBTRACE TRACE
#define GBTRACE __noop

//pcb view
#define VIEWTRACE TRACE
//#define VIEWTRACE __noop

#define MPTRACE TRACE
//#define MPTRACE __noop

void Create_STD_OUT_Console( );

#ifdef _DEBUG
#define D_WOUT( x ) ( std::wcout << x )
#define USING_CONSOLE
#else
#define D_WOUT( x ) __noop
#endif
