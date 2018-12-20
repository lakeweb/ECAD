
// GearsView.cpp : implementation of the CGearsView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.

#include "cad_full.h"
#include "dxf.h"

#include "Scroller.h"

#ifndef SHARED_HANDLERS
#include "CAD.h"
#endif
#include "CADDoc.h"
#include "GearsDoc.h"
#include "CADView.h"
#include "GearsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define SHOW_PARAMS

// ..................................................................
IMPLEMENT_DYNCREATE( CGearsView, CADView )
BEGIN_MESSAGE_MAP( CGearsView, CADView )
	ON_WM_TIMER( )
	//ON_COMMAND( ID_TOOLS_ANIMATE, &OnToolsAnimate )
	//ON_UPDATE_COMMAND_UI( ID_TOOLS_ANIMATE, &OnUpdateToolsAnimate )
	ON_WM_PAINT()
END_MESSAGE_MAP( )

// ..................................................................
CGearsView::CGearsView( )
	:bAnimate( false )
	,bInPaint( false )
{
}

// ..................................................................
CGearsView::~CGearsView( )
{
}

//// roll the gears............................................
//void CGearsView::OnToolsAnimate( )
//{
//	bAnimate= ! bAnimate;
//	if( bAnimate )
//	{
//		SetTimer( ID_ANIMATE_TIMER, 40, nullptr );
//	}
//	else
//		KillTimer( ID_ANIMATE_TIMER );
//}
//
//void CGearsView::OnUpdateToolsAnimate( CCmdUI *pCmdUI )
//{
//	pCmdUI->Enable( );
//	pCmdUI->SetCheck( bAnimate );
//}

void CGearsView::OnTimer( UINT nIDEvent )
{
	if( bInPaint )
		return;

	auto& set= GetDocument( )->GetDrawingObjects( );
	for( auto& gear : set.get_set( ) )
	{
		for( auto& item : gear )
		{
			if( gear.get_id( ) == 1 )
			{
				const_cast< SP_BaseItem& >( item )= rotate_object( .002, item, gear.get_offset( ) );
			}
			else if( gear.get_id( ) == 2 )
			{
				const_cast< SP_BaseItem& >( item )= rotate_object( -.002, item, gear.get_offset( ) );
			}
		}
	}
	Invalidate( );
}

// ..................................................................
#ifdef _DEBUG
void CGearsView::AssertValid( ) const
{
	CView::AssertValid();
}

// ..................................................................
void CGearsView::Dump( CDumpContext& dc ) const
{
	CView::Dump(dc);
}

// ..................................................................
CGearsDoc* CGearsView::GetDocument( ) const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGearsDoc)));
	return (CGearsDoc*)m_pDocument;
}
#endif //_DEBUG

void CGearsView::OnPaint()
{
	bInPaint= true;
	CADView::OnPaint( );
	bInPaint= false;
//	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CADView::OnPaint() for painting messages
}
