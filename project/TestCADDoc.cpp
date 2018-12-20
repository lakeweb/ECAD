
#include "stdafx.h"

#include "cad_full.h"
#include "app_share.h"
#include "CADDoc.h"
#include "TestCADDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ..........................................................................
IMPLEMENT_DYNCREATE(TestCADDoc, CADDoc)
BEGIN_MESSAGE_MAP(TestCADDoc, CADDoc)
END_MESSAGE_MAP()

// ..........................................................................
TestCADDoc::TestCADDoc()
{
}

#define TEST_DRAW_TEXT

BOOL TestCADDoc::OnOpenDocument(LPCTSTR lpszPathName)
{

#ifdef TEST_DRAW_TEXT
	font_ref font(1,20);
	drawobj.insert_font(font);
	
	ItemSet ob;
	bg_point a(0, 0); bg_point b(150, 0); bg_point c(300, 0);
	bg_point d(0, 100); bg_point e(150, 100); bg_point f(300, 100);
	bg_point g(0, 200); bg_point h(150, 200); bg_point i(300, 200);
	RectItem recti(a, i);

	TextItem LLtext(std::string("LowerLeft"), a);
	LLtext.just = ObjectSpace::jLowerLeft;
	ob.push_back(LLtext);

	TextItem Lotext(std::string("Lower"), b);
	Lotext.just = ObjectSpace::jLower;
	ob.push_back(Lotext);

	TextItem LRtext(std::string("LowerRight"), c);
	LRtext.just = ObjectSpace::jLowerRight;
	ob.push_back(LRtext);

	LineItem aline(d, f);
	ob.push_back(aline);

	TextItem Ltext(std::string("Left"), d);
	Ltext.just = ObjectSpace::jLeft;
	ob.push_back(Ltext);

	TextItem Rtext(std::string("Right"), f);
	Rtext.just = ObjectSpace::jRight;
	ob.push_back(Rtext);

	TextItem ULtext(std::string("UpperLeft"), g);
	ULtext.just = ObjectSpace::jUpperLeft;
	ob.push_back(ULtext);

	TextItem URtext(std::string("UpperRight"), i);
	URtext.just = ObjectSpace::jUpperRight;
	ob.push_back(URtext);

	//TextItem LLtext(std::string("LowerLeft"), bg_point(0,9));
	//LLtext.just = ObjectSpace::jLowerLeft;
	//ob.push_back(LLtext);

	RectItem rect_targ(bg_point(-99, -99), bg_point(499, 599));

	ob.push_back(recti);
	ob.push_back(rect_targ);
#endif //TEST_DRAW_TEXT

	drawobj.push_back(ob);

//#ifdef _DEBUG
//	std::stringstream os;
//	os << "target size: " << scroller.GetTargetSize().cx << " " << scroller.GetTargetSize().cy << std::endl;
//	os << "page size: " << scroller.GetPageSize().cx << " " << scroller.GetPageSize().cy << std::endl;
//	os << "winscale: " << winScale << std::endl;
//	info_notify notice(info_notify::output_info);
//	notice.str = os.str();
//	notice.bClear = true;
//	SetInfoWnd(notice);
//#endif //_DEBUG

	draw_extents.min_corner().set(bg_point(0, 0));
	draw_extents.max_corner().set(bg_point(600, 600));
	//UpdateAllViews(nullptr, 0, nullptr);
	return TRUE;
}