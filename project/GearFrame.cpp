
#include "stdafx.h"
#include "Resource.h"

#include "GearFrame.h"



//
//it is ok that this frame knows about its view;
#include "Resource.h"
#include "Geometry.h"
#include "objects.h"
#include "shared.h"
#include "cnc.h"
#include "Drawing.h"
#include "gear_gen.h"
#include "Scroller.h"
#include "GearsDoc.h"
#include "GearsView.h"

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
IMPLEMENT_DYNCREATE(GearFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(GearFrame, CMDIChildWndEx)
	ON_WM_CREATE( )
//	ON_MESSAGE( ID_POST_PCVIEW_FRAME, &OnViewPost )
	ON_UPDATE_COMMAND_UI( ID_STATUSBAR_LABEL, &OnUpdateStatusText )
END_MESSAGE_MAP()

// ............................................................................
GearFrame::GearFrame()
{
}

// ............................................................................
GearFrame::~GearFrame()
{
}

// ............................................................................
BOOL GearFrame::PreCreateWindow( CREATESTRUCT& cs )
{
	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

// ............................................................................
int GearFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	if( ! hRule.Create( this, 11023, 25 ) )
	{
		TRACE( "Failed to create status bar\n" );
		return -1;      // fail to create
	}
	DockPane( &hRule );

	if( ! vRule.Create( this, 11024, 25, false ) )
	{
		TRACE( "Failed to create status bar\n" );
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

	::ShowWindow( *this, SW_SHOWMAXIMIZED );
	return 0;
}

// ............................................................................
void GearFrame::OnUpdateStatusText( CCmdUI *pCmdUI )
{
	pCmdUI->Enable( );
	CGearsView* pView= dynamic_cast< CGearsView* >( GetActiveView( ) );
	ASSERT_VALID( pView );
	pCmdUI->SetText( pView->GetStatusInfo( ) );
	//TRACE( "in OnUpdateStatusText\n" );
}

// ............................................................................
// not used but leave it here for now.........
//ID_POST_PCVIEW_FRAME
LRESULT GearFrame::OnViewPost( WPARAM pos, LPARAM )
{
	return 0;
}


// ............................................................................
// ............................................................................
#ifdef _DEBUG
void GearFrame::AssertValid( ) const
{
	CMDIChildWndEx::AssertValid( );
}

void GearFrame::Dump( CDumpContext& dc ) const
{
	CMDIChildWndEx::Dump( dc );
}
#endif //_DEBUG


// ...........................................................................
//FrmRuler
BEGIN_MESSAGE_MAP( FrmRuler, CWnd )
	ON_WM_PAINT( )
END_MESSAGE_MAP( )

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

//AFX_STATIC_DATA int numSpan[ ]= { 0, 10, 100, 1000 };
//
//uint16_t DigCnt( int dig )
//{
//	std::vector< int >( numSpan );
//}
//
void FrmRuler::OnPaint( )
{
	CRect rect;
	CPaintDC dc( this );
	int save_index= dc.SaveDC( );

	GetClientRect( rect );
	dc.DrawEdge( rect, EDGE_SUNKEN, BF_TOPLEFT | BF_BOTTOM | BF_MIDDLE );

	dc.SetBkMode( TRANSPARENT );
	dc.SetBkColor( GetSysColor( COLOR_ACTIVEBORDER ) );
	dc.SetTextColor( RGB( 0, 0, 0 ) );
	dc.SelectStockObject( BLACK_PEN );

	CFont aFont;
	aFont.CreatePointFont( 50, _T("Courier"), NULL);
	dc.SelectObject( aFont );
	int chr1= dc.GetTextExtent( _T("2"), 1 ).cx / 2;
	if( ! bHorz )
	{
		LOGFONT lf;
		aFont.GetLogFont( &lf );
		lf.lfEscapement= 600;
		aFont.DeleteObject( );
		aFont.CreateFontIndirect( &lf );

	}
	int nFrom= 5;
	int pitch= 48;
	int ticks= 6; //24 / 8
	int startOffset= 12;
	if( bHorz )
	{
		for( auto pos= startOffset; pos < rect.right; pos+= ticks )
		{
			bool bBtick= ! ( pos % ( pitch / 2 ) );
			if( !( pos % pitch ) )
			{
				TCHAR buf[ 4 ];
				_itot_s( nFrom++, buf, 4, 10 );
				size_t dcnt= _tcslen( buf );
				dc.DrawText( buf, 2, CRect( rect.left + pos - chr1 * dcnt, rect.top, rect.right, rect.bottom ), DT_LEFT | DT_NOPREFIX );
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
		for( auto pos= rect.bottom; pos > rect.top; pos-= ticks )
		{
			bool bBtick= ! ( pos % ( pitch / 2 ) );
			if( !( pos % pitch ) )
			{
				TCHAR buf[ 4 ];
				_itot_s( nFrom++, buf, 4, 10 );
				size_t dcnt= _tcslen( buf );
				dc.DrawText( buf, 2, CRect( rect.left, rect.top + pos - chr1 * dcnt, rect.right, rect.bottom ), DT_LEFT | DT_NOPREFIX );
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
	//CBrush brush( RGB( 24, 123, 222 ) );
	//dc.FillRect( rect, &brush );
}
