
#include "stdafx.h"

#include "cad_full.h"
#include "app_share.h"
#include "CADDoc.h"
#include "TestCADDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace ObjectSpace;
// ..........................................................................
IMPLEMENT_DYNCREATE(TestCADDoc, CADDoc)
BEGIN_MESSAGE_MAP(TestCADDoc, CADDoc)
END_MESSAGE_MAP()

// ..........................................................................
TestCADDoc::TestCADDoc()
{
}

#define TEST_DRAW_TEXT
//#define TEST_STYLE

BOOL TestCADDoc::OnOpenDocument(LPCTSTR lpszPathName)
{

	ItemSet ob;

#ifdef TEST_DRAW_TEXT
	font_ref font(1,20,10);
	drawobj.insert_font(font);
	
	draw_style fill(RGB(0, 0, 255), psSolid, 1, RGB(0, 255, 0), bsHollow); //red and blue
	bg_point a(0, 0); bg_point b(150, 0); bg_point c(300, 0);
	bg_point d(0, 100); bg_point e(150, 100); bg_point f(300, 100);
	bg_point g(0, 200); bg_point h(150, 200); bg_point i(300, 200);
	RectItem recti(a, i);
	recti.sp_style = fill;
	ob.push_back(recti);
	LineItem aline(d, f);
	ob.push_back(aline);

	TextItem LLtext(std::string("LowerLeft"), a, font);
	LLtext.just = jLowerLeft;
	ob.push_back(LLtext);

	TextItem Lotext(std::string("Lower"), b, font);
	Lotext.just = jLower;
	ob.push_back(Lotext);

	TextItem LRtext(std::string("LowerRight"), c, font);
	LRtext.just = jLowerRight;
	ob.push_back(LRtext);

	TextItem Ltext(std::string("Left"), d, font);
	Ltext.just = jLeft;
	ob.push_back(Ltext);

	TextItem Rtext(std::string("Right"), f, font);
	Rtext.just = jRight;
	ob.push_back(Rtext);

	TextItem ULtext(std::string("UpperLeft"), g, font);
	ULtext.just = jUpperLeft;
	ob.push_back(ULtext);

	TextItem URtext(std::string("UpperRight"), i, font);
	URtext.just = jUpperRight;
	ob.push_back(URtext);

	TextItem Utext(std::string("Upper"), h, font);
	Utext.just = jUpper;
	ob.push_back(Utext);

#endif //TEST_DRAW_TEXT

#ifdef TEST_STYLE
	/*
	12/29/2018
	The draw_style is in place. Drawing will be done in the order composed.
		No atempt to optimize the likes of reducing pen and brush changes will be made.
		But if the 'draw_style' pointers match for consecutive objects, even checking
		of style selections will not happen. Optimization would have to be done when
		the object sets are composed if it is desired.
		Note that layers are not a
	*/
	draw_style fill(RGB(0, 0, 255), psSolid, 3, RGB(0, 255, 0), bsHatched); //red and blue
//The style can be set for a group of objects.
	ItemSet a_group;
	a_group.sp_style = fill;
	//Note that this is way more effecient as a poly group; Using PointItem.
	//the Lines are pushed back without styles so the 'a_group' style will be used.
	a_group.push_back(LineItem(bg_point(0, 0), bg_point(0, 100)));
	a_group.push_back(LineItem(bg_point(0, 100), bg_point(100, 100)));
	a_group.push_back(LineItem(bg_point(100, 100), bg_point(100, 0)));
	a_group.push_back(LineItem(bg_point(100, 0), bg_point(0, 0)));

	drawobj.push_back(a_group);

//...................
	//styles assigned to objects......
	draw_style sa(RGB(0, 0, 255), psSolid, 3, RGB(200, 255, 0), bsHatched); //red and blue
	draw_style sb(RGB(0, 220, 255), psSolid, 3, RGB(0, 255, 0), bsHatched); //red and blue
	draw_style sc(RGB(220, 0, 255), psSolid, 3, RGB(0, 255, 0), bsHatched); //red and blue
	ItemSet b_group; // no style assigned
	LineItem line_a(bg_point(150, 150), bg_point(180, 180));
	line_a.sp_style = sa;
	b_group.push_back(line_a);
	LineItem line_b(bg_point(180, 180), bg_point(180, 150));
	line_b.sp_style = sb;
	b_group.push_back(line_b);
	LineItem line_c(bg_point(180, 150), bg_point(150, 150));
	line_c.sp_style = sc;
	b_group.push_back(line_c);

	auto sd(sa);//a copy of the draw_style
	//but this will not create a second pen of the same style
	//sa will be found as a duplicate and that will be selected into the DC.
	LineItem line_d(bg_point(0, 200), bg_point(200, 200));
	line_d.sp_style = sd;
	b_group.push_back(line_d);
	//and no style assigned so the last style is in effect.
	LineItem line_e(bg_point(0, 220), bg_point(200, 220));
	b_group.push_back(line_e);

	drawobj.push_back(b_group);
#endif

	drawobj.push_back(ob);

	bg_box bound(get_bound_rect(drawobj.get_objects_set()));
	inflate(bound, 20);
	draw_extents = bound;

	//Hmmmmmmmmmmmmmmmm.......
	ItemSet the_box;
	the_box.sp_style= draw_style(RGB(255, 330, 0), psSolid, 1, CLR_BLK,bsSolid,1);
	the_box.push_back(RectItem(bound));
	drawobj.insert(drawobj.begin(), the_box);

	UpdateAllViews(nullptr, 0, nullptr);
	return TRUE;
}