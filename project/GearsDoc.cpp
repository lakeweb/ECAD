
// GearsDoc.cpp : implementation of the CGearsDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.


#include "cad_full.h"
#include "dxf.h"
#include "sgv.h"
#include "gear_gen.h"
// ............................................................
ItemSet gear_gen_one( const gear_params_t& gp );
ItemSet gear_generate( const ItemSet& tooth, const gear_params_t& gp );

#ifndef SHARED_HANDLERS
#include "CAD.h"
#endif

#include "CADDoc.h"
#include "GearsDoc.h"

//#include <propkey.h>

#include <boost/math/constants/constants.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ..........................................................................
IMPLEMENT_DYNCREATE( CGearsDoc, CADDoc )
BEGIN_MESSAGE_MAP( CGearsDoc, CADDoc )
	ON_COMMAND( ID_FILE_EXPORT, &CGearsDoc::OnFileExport )
	ON_COMMAND( ID_FILE_G_CODE, &CGearsDoc::OnFileGCode )
//	ON_NOTIFY( ID_GEAR_PARAM_CHANGED, 1, &CGearsDoc::OnCmdMsg )
END_MESSAGE_MAP( )

// ............................................................................
//void CGearsDoc::OnGearParamChanged( NMHDR*, LRESULT* )
//{
//}

// ..........................................................................
BOOL CGearsDoc::OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo )
{
	if ( HIWORD(nCode) == WM_NOTIFY )
	{
		LPNMHDR pHdr= ((AFX_NOTIFY*)pExtra)->pNMHDR;
		switch( LOWORD( nCode ) )
		{
		case ID_GEAR_PARAM_CHANGED:
			auto pg= ( pgear_params_t )((PNMHDROBJECT)pHdr)->pObject;
			test( pg );
			test( );
			UpdateAllViews( nullptr, ID_UPDATE_VIEW_TOFIT );
			NMHDR* pnm= ((AFX_NOTIFY*)pExtra)->pNMHDR;
			//TRACE( "doc: %x - %x - %x low: %x\n", nID, nCode, pnm->code, LOWORD( nCode ) );
		}
	}
	return CDocument::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}

DrawingObect& CGearsDoc::GetExportDrawObject( )
{
	export_drawobj.get_set( ).clear( );
	export_drawobj.push_back( drawobj.get_set( ).front( ) );
	return export_drawobj;
}

// ..........................................................................
CGearsDoc::CGearsDoc( )
	:bAnimate( false )
	//,export_drawobj( layers )
{
}

// ..........................................................................
CGearsDoc::~CGearsDoc( )
{
}

// ..........................................................................
void CGearsDoc::OnFileGCode( )
{
	//testing g-code............
	//sp_gcitem_t test( boost::make_shared< gc0 >( ) );
	//gc.push_back( test );
	//gc.push_back( sp_gcitem_t( new gc0 ) );
}

template< typename Type >
inline SP_BaseItem  make_sp( Type& obj ) { return boost::make_shared< Type >( obj ); }
// ..........................................................................
void CGearsDoc::test( )
{
//	bg_box box= get_rect_hull( drawobj.get_set( ) );
//	RectItem ri( box );
//	GEO_NUM bx= ri.width( ) / 4;
//	ri.get_min_point( ).x-= bx;
//	ri.get_max_point( ).x+= bx;
//	GEO_NUM by= ri.height( ) / 4;
//	ri.get_min_point( ).y-= by;
//	ri.get_max_point( ).y+= by;
//	draw_extents= ri.get_bg_box( );
////	drawobj.get_set( ).back( ).push_back( RectItem( ri ).get_SP( ) );
}

// ..........................................................................
void CGearsDoc::test( pgear_params_t pg )
{
	drawobj.get_set( ).clear( );
	////create a tooth
	auto tooth= gear_gen_one( *pg );

	//generate gear from tooth
	auto gear= gear_generate( tooth, *pg );
	if( pg->cb )
		gear.push_back( ArcItem( bg_point( 0, pg->cb ), bg_point( 0, pg->cb ), bg_point( 0, 0 ) ).get_SP( ) );
	gear.set_id( 1 );
	drawobj.push_back( gear );

	//copy gear with offset
	ItemSet gear2;//= boost::make_shared< ItemSet >( ItemSet( ) );
	gear2.set_offset( bg_point( pg->pd, 0 ) );
	double rotate= pg->tc % 2 ? pg->of + PI / pg->tc : pg->of; 
	for( const auto& item : gear )
	{
		auto hold= transform_object( item, bg_point( pg->pd, 0 ) );
		hold= rotate_object( rotate, hold, bg_point( pg->pd, 0 ) );
		gear2.push_back( hold );
	}
//	visuals.insert( gear2.begin( ), gear2.end( ) );
	gear2.set_id( 2 );
	drawobj.push_back( gear2 );

	// working ..........................................
	//layers.insert(cad_layer( 0, L"Zero Layer" ) );
	//layers.insert(cad_layer( 1, L"Milling" ) );

	test( );
}

// ..........................................................................
BOOL CGearsDoc::OnNewDocument( )
{
	if( ! CDocument::OnNewDocument( ) )
		return FALSE;

#if 1
	gear_params_t params(
		12,				//tooth count
		TO_RAD( 20 ),	//presure angle in radians
		8,				//diametrical pitch
		.25				//center bore......
	);
	test( &params );
	test( );
	UpdateAllViews( nullptr, ID_UPDATE_VIEW_TOFIT );

#endif
	return TRUE;
}

// ..........................................................................
BOOL CGearsDoc::OnSaveDocument( LPCTSTR lpszPathName )
{
	CADDoc::OnSaveDocument( lpszPathName );
	return TRUE;
}

// ..........................................................................
bool StartDXF_write( DL_Dxf* dxf, DL_WriterA* dw );
bool EndDXF_write( DL_Dxf* dxf, DL_WriterA* dw );

// ..........................................................................
LRESULT CGearsDoc::OnLayerEnable(WPARAM, LPARAM )
{
	return 0;
}

// ..........................................................................
void CGearsDoc::OnFileExport( )
{
	if( ! drawobj.size( ) )
		return;

	//DrawingObect a_gear( layers );
	auto& a_gear= GetExportDrawObject( );//.get_set( ).push_back( ItemSet( drawobj.get_set( ).front( ) ) );

	CFileDialog dlg( TRUE, NULL, NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("DXF Files (*.dxf)|*dxf|SVG Files (*.svg)|*svg||"), NULL, 0);
	dlg.m_ofn.nFilterIndex= get_user_profile( ).export_type;

	int ret= dlg.DoModal( );
	OPENFILENAME& ofn= dlg.GetOFN( );
	bfs::path path( ofn.lpstrFile );
	if( path.extension( ) == L".dxf" || path.extension( ) == L".svg" )
		path.replace_extension( L"" );
	get_user_profile( ).export_type= ofn.nFilterIndex;
	switch( ofn.nFilterIndex )
	{
	case 1: //dxf
		path+= L".dxf";
		DXF_WriteFile( path, a_gear );
		break;

	case 2: //svg
		path+= L".svg";
		SVG_WriteFile( path, a_gear );
		break;
	}
}


// ..........................................................................
void CGearsDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

// ..........................................................................
#ifdef _DEBUG
void CGearsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

// ..........................................................................
void CGearsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

#ifdef SHARED_HANDLERS

// ..........................................................................
// Support for thumbnails
void CGearsDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// ..........................................................................
// Support for Search Handlers
void CGearsDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

// ..........................................................................
void CGearsDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

