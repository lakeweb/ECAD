
// CADView.cpp : implementation of the CADView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.

#include "MemDC.h"
#include "cad_full.h"
#include "dxf.h"

#include "Scroller.h"

#ifndef SHARED_HANDLERS
#include "CAD.h"
#endif

#include "CADDoc.h"
#include "CADView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define SHOW_PARAMS

// ..................................................................
IMPLEMENT_DYNCREATE( CADView, CView )
BEGIN_MESSAGE_MAP( CADView, CView )
	// Standard printing commands
	ON_COMMAND( ID_FILE_PRINT, &CView::OnFilePrint )
	ON_COMMAND( ID_FILE_PRINT_DIRECT, &CView::OnFilePrint )
	ON_COMMAND( ID_FILE_PRINT_PREVIEW, &CADView::OnFilePrintPreview )
	ON_MESSAGE( VIEW_INFO_NOTIFY, &CADView::OnFrmNotify )
	ON_COMMAND(ID_TOOLS_ANIMATE,&CADView::OnToolsAnimate)
	ON_WM_CONTEXTMENU( )
	ON_WM_RBUTTONUP( )
	ON_WM_MOUSEMOVE( )
	ON_WM_MOUSEWHEEL( )
	ON_WM_VSCROLL( )
	ON_WM_HSCROLL( )
	ON_WM_SIZE( )
	ON_WM_CREATE()
END_MESSAGE_MAP( )

// ..................................................................
void CADView::OnToolsAnimate()
{
}

// ..................................................................
void CADView::OnUpdateToolsAnimate( CCmdUI *pCmdUI )
{
	pCmdUI->Enable( );
	pCmdUI->SetCheck( bAnimate );
}

// ..................................................................
CADView::CADView( )
	:winScale( 1 )
	,offsetx( 0 )
	,offsety( 0 )
	,rectTarget( 0,0,0,0 )
	,d_ext(rectTarget, offsetx, offsety, winScale )
	,bStatusMouseChanged( true )
	,bAnimate( false )
{
}

// ..................................................................
CADView::~CADView( )
{
}

LRESULT CADView::OnFrmNotify( WPARAM wp, LPARAM lp )
{
	typedef dock_notify_t dn;
	dock_notify_t& note= *reinterpret_cast< dock_notify_t* >( wp );

	switch( note.the_docker )
	{
	case dn::layer:
	{
		layer_set_t& ls= GetDocument( )->layers;
		auto it= ls.find(cad_layer( note.info, NULL ) );
		if( it != ls.end( ) )
		{
			cad_layer t= *it;
			ls.erase( it );
			t.enabled= ! t.enabled;
			ls.insert( t );
		}
		Invalidate( );
		break;
	}
	}
	return FALSE;
}

// ............................................................................
//When frame needs status info
CString CADView::GetStatusInfo( )
{
	if( bStatusMouseChanged )
	{
		strStatus.Format( _T(" x: %.4f y: %.4f")
			,(double)( ptLastMouse.x - offsetx ) / winScale
			,(double)( ptLastMouse.y - offsety ) / winScale
		);
		bStatusMouseChanged= false;
	}
	return strStatus;
}

// ..................................................................
void CADView::OnDraw( CDC* poDC )
{
	CADDoc* pDoc = GetDocument( );
	ASSERT_VALID( pDoc );
	if( ! pDoc )
		return;

	offsetx= -scroller.GetScrollPos( ).cx - d_ext.rect_target.left;
	offsety= -scroller.GetScrollPos( ).cy - d_ext.rect_target.top;
	CNewMemDC pDC( poDC );
	SetBkMode(pDC, TRANSPARENT);
	CRect crect;
	GetClientRect( crect );

	//the drawer....................
	const DrawingObects& objs= pDoc->GetDrawingObjects( );
	DrawArea drawer(pDC, d_ext);
	drawer.SetFonts(objs.get_fontset());
	drawer.DrawObjects( objs );

#ifdef _DEBUG
	std::stringstream os;
	os << "view size: " << scroller.GetTargetSize( ).cx << " " << scroller.GetTargetSize( ).cy << std::endl;
	os << "page size: " << scroller.GetPageSize( ).cx << " " << scroller.GetPageSize( ).cy << std::endl;
	os << "winscale: " << winScale << std::endl;
	info_notify notice( info_notify::output_info );
	notice.str= os.str( );
	notice.bClear= true;
	SetInfoWnd(notice);
#endif //_DEBUG
}

// ..................................................................
void CADView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

// ..................................................................
void CADView::OnContextMenu( CWnd* /* pWnd */, CPoint point )
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager( )->ShowPopupMenu( IDR_POPUP_EDIT, point.x, point.y, this, TRUE );
#endif
}

// ............................................................................
void CADView::OnMouseMove( UINT nFlags, CPoint point )
{
	if( ptLastMouse != point )
		bStatusMouseChanged= true;
	else
		return;

	ptLastMouse= point;
	CView::OnMouseMove( nFlags, point );
}

// ..................................................................
BOOL CADView::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	double prevWinScale= winScale;

	if( zDelta < 0 )
		winScale-= winScale / 4;
	else
		winScale+= winScale / 4;

	//TRACE( "winScale: %f\n", winScale );
	CSize ratio( (long)( prevWinScale * 1000 ), (long)( winScale * 1000) );
	scroller.OnMouseWheel(ratio,pt);

	Invalidate( );
	return FALSE;
	//	return CView::OnMouseWheel( nFlags, zDelta, pt );
}

// ............................................................................
bg_box CADView::GetClientBounds()
{
	CRect rTest;
	GetClientRectSB(this, rTest);
	auto size = scroller.GetPageSize();
	return bg_box(bg_point(rTest.left,rTest.top), bg_point(rTest.right,rTest.bottom));
}

// ............................................................................
void CADView::OnUpdate( CView* pSender, LPARAM hint, CObject* pHint )
{
	switch( hint )
	{
	case ID_GEAR_PARAM_CHANGED://TODO this should have been in CGearsView
	{
		assert( false );
		auto container= GetDocument( )->GetDrawingObjects( ).get_set( );
		break;
	}
	case ID_UPDATE_VIEW_TOFIT:
		break;

	case ID_INVALIDATE:
		Invalidate();
		break;

	case 0://initial update
	{
		//while testing.......................... TODO
		GetParentFrame()->ShowWindow(SW_MAXIMIZE);
		//The window is valid here but the scroller may not be up to date.
		//as 'fit' will draw without scrollbars...........
		bg_box& targ= GetDocument( )->draw_extents;
		bg_box screen = GetClientBounds();

		double ast = aspect(targ);
		double ass = aspect(screen);
		if (ast > ass)
		{
			auto tw = width(targ);
			winScale = width(screen) / tw * .8;
		}
		else
		{
			auto tw = width(targ);
			winScale = height(screen) / tw * .8;
		}
		CRect displayRect( (int)(targ.min_corner().get_x()*winScale),
			(int)(targ.min_corner().get_y()*winScale),
			(int)(targ.max_corner().get_x()*winScale),
			(int)(targ.max_corner().get_y()*winScale));

		//testing TODO
		//displayRect.InflateRect(100, 100);
		SetTargetSize(displayRect);

		d_ext.rect_target = displayRect;
	}
	}//switch( .. )
}

// ..................................................................
//printing
// ..................................................................
void CADView::OnFilePrintPreview( )
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview( this );
#endif
}

// ..................................................................
BOOL CADView::OnPreparePrinting( CPrintInfo* pInfo )
{
	return DoPreparePrinting( pInfo );
}

void CADView::OnBeginPrinting( CDC* /*pDC*/, CPrintInfo* /*pInfo*/ )
{
}

// ..................................................................
void CADView::OnEndPrinting( CDC* /*pDC*/, CPrintInfo* /*pInfo*/ )
{
}

void CADView::NotifyFrame( )
{
	//::SendMessage( *GetParent( ), ID_POST_CADVIEW_FRAME, (WPARAM)&d_ext, 0 );
}

// ............................................................................
void CADView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	scroller.OnVScroll( nSBCode, nPos, pScrollBar );
	CView::OnVScroll( nSBCode, nPos, pScrollBar );
	d_ext.bVert = true;
}

// ............................................................................
void CADView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	scroller.OnHScroll( nSBCode, nPos, pScrollBar );
	CView::OnHScroll( nSBCode, nPos, pScrollBar );
	d_ext.bHorz= true;
//	NotifyFrame( );
}

// ............................................................................
void CADView::OnSize( UINT nType, int cx, int cy )
{
	CView::OnSize( nType, cx, cy );
	scroller.OnSize( nType, cx, cy );
	d_ext.set_all( );
	std::stringstream sstr;
	sstr << " target box: " << d_ext.rect_target << std::endl
		<< " offset x: " << offsetx << " y: " << offsety << std::endl
		<< " zoom: " << d_ext.zoom << std::endl;
	info_notify notice(info_notify::other_info);
	notice.str = sstr.str();
	notice.bClear = true;
	SetInfoWnd(notice);
}

// ............................................................................
void CADView::SetTargetSize(LPRECT rect)
{
	scroller.SetTargetSize( rect->right - rect->left, rect->bottom - rect->top );
	Invalidate( );
}

// ..................................................................
BOOL CADView::PreCreateWindow(CREATESTRUCT& cs)
{
#ifdef USING_CONSOLE
	Create_STD_OUT_Console();
#endif
	cs.style |= WS_HSCROLL | WS_VSCROLL;
	scroller.AttachWnd(this);

	return CView::PreCreateWindow(cs);
}

// ..................................................................
#ifdef _DEBUG
void CADView::AssertValid( ) const
{
	CView::AssertValid();
}

// ..................................................................
void CADView::Dump( CDumpContext& dc ) const
{
	CView::Dump(dc);
}

// ..................................................................
CADDoc* CADView::GetDocument( ) const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CADDoc)));
	return (CADDoc*)m_pDocument;
}
#endif //_DEBUG




BOOL CADView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	//auto cmd = LOWORD(nCode);
	//assert(cmd != ID_LIB_TREE_SELECTED);
	//if (HIWORD(nCode) == WM_NOTIFY)
	//{
	//	std::cout << "** " << "Got WM_NOTIFY\n";
	//	LPNMHDR pHdr = ((AFX_NOTIFY*)pExtra)->pNMHDR;
	//	std::cout << "    ** " << pHdr->code << " " << pHdr->idFrom << " " << pHdr->hwndFrom << std::endl << std::endl;
	//	auto cmd = LOWORD(nCode);
	//	assert(cmd != ID_LIB_TREE_SELECTED);
	//	//	switch( LOWORD( nCode ) )
	//	//	{
	//	//	case ID_GEAR_PARAM_CHANGED:
	//	//		auto pg= ( pgear_params_t )((PNMHDROBJECT)pHdr)->pObject;
	//	//		test( );
	//	//		UpdateAllViews( nullptr, ID_GEAR_PARAM_CHANGED );
	//	//		NMHDR* pnm= ((AFX_NOTIFY*)pExtra)->pNMHDR;
	//	//		TRACE( "doc: %x - %x - %x low: %x\n", nID, nCode, pnm->code, LOWORD( nCode ) );
	//	//	}
	//}

	return CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//
//BOOL CADView::CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpParam)
//{
//	// TODO: Add your specialized code here and/or call the base class
//
//	return CView::CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, lpParam);
//}


int CADView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	//CRect rect;
	//GetClientRect(rect);

	return 0;
}


void CADView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
}
