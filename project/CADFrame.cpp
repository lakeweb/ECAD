
#include "stdafx.h"
#include "Resource.h"

#include "MemDC.h"
#include "Scroller.h"

#include "Resource.h"
#include "cad_full.h"
#include "Scroller.h"
#include "CADDoc.h"
#include "CADView.h"
#include "CADFrame.h"

// ............................................................................
//for status bar
static UINT indicators[] =
{
	ID_STATUSBAR_LABEL,		// text label
							//ID_INDICATOR_ICON,		// status icon
	ID_SEPARATOR,           // status line indicator
							//ID_INDICATOR_PROGRESS,	// progress bar
							//ID_INDICATOR_ANIMATION,	// animation pane
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// ............................................................................
IMPLEMENT_DYNCREATE(CADFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CADFrame, CMDIChildWndEx)
	ON_WM_CREATE( )
	ON_MESSAGE( ID_POST_CADVIEW_FRAME, &OnViewPost )
	ON_UPDATE_COMMAND_UI( ID_STATUSBAR_LABEL, &OnUpdateStatusText )
END_MESSAGE_MAP()

// ............................................................................
CADFrame::CADFrame( )
//	:pView( nullptr )
{
}

// ............................................................................
CADFrame::~CADFrame()
{
}

// ............................................................................
BOOL CADFrame::PreCreateWindow( CREATESTRUCT& cs )
{
	if( ! CMDIChildWndEx::PreCreateWindow( cs ) )
		return FALSE;

	return TRUE;
}

// ............................................................................
#define RULER_DEPTH 24
int CADFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if( CMDIChildWndEx::OnCreate(lpCreateStruct ) == -1 )
		return -1;

	if( ! hRule.Create( this, 11023, RULER_DEPTH ) )
	{
		TRACE( "Failed to create horz rule bar\n" );
		return -1;      // fail to create
	}
	DockPane( &hRule );

	if( ! vRule.Create( this, 11024, RULER_DEPTH, false ) )
	{
		TRACE( "Failed to create vert rule bar\n" );
		return -1;      // fail to create
	}
	DockPane( &vRule );

	if( ! statusBar.Create( this ) )
	{
		TRACE( "Failed to create status bar\n" );
		return -1;      // fail to create
	}
	statusBar.SetIndicators( indicators, sizeof(indicators) / sizeof(UINT) );
	statusBar.SetPaneStyle( statusBar.CommandToIndex( ID_STATUSBAR_LABEL ), SBPS_NOBORDERS );
	statusBar.SetPaneTextColor( statusBar.CommandToIndex( ID_STATUSBAR_LABEL ) );

	//::ShowWindow( *this, SW_SHOWMAXIMIZED );
	return 0;
}

// ............................................................................
void CADFrame::OnUpdateStatusText( CCmdUI *pCmdUI )
{
	pCmdUI->Enable( );
	CADView* pView= dynamic_cast< CADView* >( GetActiveView( ) );
	if (!pView)
		return;//warn?

	ASSERT_VALID(pView);
	pCmdUI->SetText( pView->GetStatusInfo( ) );
	//TRACE( "in OnUpdateStatusText\n" );
	if( pView->d_ext.bHorz )
		hRule.Invalidate( );
	else
		hRule.set_marker( );

	if( pView->d_ext.bVert )
		vRule.Invalidate( );
	else
		vRule.set_marker( );

	pView->d_ext.clear_all( );
}

// ............................................................................
// not used but leave it here for now.........
//ID_POST_CADVIEW_FRAME
LRESULT CADFrame::OnViewPost( WPARAM pData, LPARAM )
{
	//only setting rulers for now
	DrawExtent* pde= reinterpret_cast< DrawExtent* >( pData );
	if( pde->bHorz && hRule )
		hRule.Invalidate( );
	pde->clear_all( );
	return 0;
}


// ............................................................................
// ............................................................................
#ifdef _DEBUG
void CADFrame::AssertValid( ) const
{
	CMDIChildWndEx::AssertValid( );
}

void CADFrame::Dump( CDumpContext& dc ) const
{
	CMDIChildWndEx::Dump( dc );
}
#endif //_DEBUG


// ...........................................................................
//FrmRuler
BEGIN_MESSAGE_MAP( FrmRuler, CWnd )
	ON_WM_PAINT( )
END_MESSAGE_MAP( )

// ...........................................................................
BOOL FrmRuler::Create( CWnd* pParentWnd, UINT nID, long size, bool bIsHorz )
{
	winSize= size;
	bHorz= bIsHorz;
	ASSERT_VALID( pParentWnd );   // must have a parent

	int dwStyle= WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE | CBRS_HIDE_INPLACE;
	if( bIsHorz )
		dwStyle|= CBRS_ALIGN_TOP;
	else
		dwStyle|= CBRS_ALIGN_LEFT;

	CRect rect;

	LPCTSTR lpszClass= AfxRegisterWndClass(0, ::LoadCursor( NULL, IDC_ARROW ),
		(HBRUSH)( COLOR_BTNFACE + 1 ), NULL );

	if( ! CMFCBaseToolBar::Create( lpszClass, dwStyle, rect, pParentWnd, nID ) )
		return FALSE;

	return TRUE;
}

// ...........................................................................
void FrmRuler::OnPaint( )
{
	CRect rect;
	CPaintDC dcm( this );
	CNewMemDC dc( &dcm );
	int save_index= dc.SaveDC( );

	GetClientRect( rect );
	dc.DrawEdge( rect, EDGE_SUNKEN, BF_TOPLEFT | BF_BOTTOM | BF_MIDDLE );

	dc.SetBkMode( TRANSPARENT );
	dc.SetBkColor( GetSysColor( COLOR_ACTIVEBORDER ) );
	dc.SetTextColor( RGB( 0, 0, 0 ) );
	dc.SelectStockObject( BLACK_PEN );

	CADFrame* parent= dynamic_cast< CADFrame* >( GetParent( ) );
	ASSERT_VALID( parent );
	CADView* view= dynamic_cast< CADView* >( parent->GetActiveView( ) );
	ASSERT_VALID( view );

	CFont aFont;
	aFont.CreatePointFont( 50, _T("Courier"), NULL);
	dc.SelectObject( aFont );
	CSize text( dc.GetTextExtent( _T("2"), 1 ) );
	text.cx/= 2;
	text.cy/= 2;
	//if( ! bHorz )
	//{
	//	LOGFONT lf;
	//	aFont.GetLogFont( &lf );
	//	lf.lfEscapement= 600;
	//	aFont.DeleteObject( );
	//	aFont.CreateFontIndirect( &lf );

	//}

	int nFrom= (int)( view->offsetx / view->winScale );
	//TRACE( "FrmRuler::OnPaint nFrom: %d\n", nFrom );
	int pitch= 48;
	int ticks= 6; //24 / 8

	if( bHorz )
	{
		for( auto pos= RULER_DEPTH; pos < rect.right; pos+= ticks )
		{
			bool bBtick= ! ( pos % ( pitch / 2 ) );
			if( !( pos % pitch ) )
			{
				TCHAR buf[ 40 ];
				_itot_s( nFrom++, buf, 40, 10 );
				size_t dcnt= _tcslen( buf );
				dc.DrawText( buf, 2, CRect( rect.left + pos - text.cx * dcnt, rect.top, rect.right, rect.bottom ), DT_LEFT | DT_NOPREFIX );
			}
			//			else
			{
				dc.MoveTo( rect.left + pos, rect.top + bBtick ? 15 : 20 );
				dc.LineTo( rect.left + pos, rect.bottom );
			}
		}
		nFrom;
	}
	else
	{
		auto pos= rect.bottom + rect.bottom % ticks;
		for( ; pos > rect.top; pos-= ticks )
		{
			bool bBtick= ! ( pos % ( pitch / 2 ) );
			if( !( pos % pitch ) )
			{
				TCHAR buf[ 12 ];
				_itot_s( nFrom++, buf, 12, 10 );
				size_t dcnt= _tcslen( buf );
				dc.DrawText( buf, 2, CRect( rect.left + text.cx * dcnt, rect.bottom - pos - text.cy, rect.right, rect.bottom ), DT_LEFT | DT_NOPREFIX );
			}
			//			else
			{
				dc.MoveTo( rect.left + bBtick ? 15 : 20, rect.bottom - pos );
				dc.LineTo( rect.right, rect.bottom - pos );
			}
		}
		nFrom;
	}
	dc.RestoreDC( save_index );

	last_x= 0;
	last_y= 0;
	set_marker( );

	dc.SetROP2( R2_BLACK ); 
	auto pos= view->GetLastMousePos( );
	dc.MoveTo(  pos.x, rect.top );
	dc.LineTo( pos.x, rect.bottom );
}

// ...........................................................................
void FrmRuler::set_marker( )
{
	CADFrame* parent= dynamic_cast< CADFrame* >( GetParent( ) );
	ASSERT_VALID( parent );
	CADView* view= dynamic_cast< CADView* >( parent->GetActiveView( ) );
	ASSERT_VALID( view );
	auto pos= view->GetLastMousePos( );

	if( bHorz && pos.x == last_x )
		return;

	if( ! bHorz && pos.y == last_y )
		return;

	CRect rect;
	GetClientRect( rect );
	CClientDC dc( this );
	//TRACE( "set_marker: %d - %d\n", pos.x, pos.y );

	if( bHorz )
	{
		dc.SetROP2( R2_NOT ); 
		dc.MoveTo(  last_x, rect.top );
		dc.LineTo( last_x, rect.bottom );
		dc.MoveTo(  pos.x, rect.top );
		dc.LineTo( pos.x, rect.bottom );
		last_x= pos.x;
	}
	else
	{
		dc.SetROP2( R2_NOT ); 
		dc.MoveTo(  rect.left, last_y );
		dc.LineTo( rect.right, last_y );
		dc.MoveTo(  rect.left, pos.y );
		dc.LineTo( rect.right, pos.y );
		last_y= pos.y;
	}
}