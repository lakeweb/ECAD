
#include "stdafx.h"
#ifndef SHARED_HANDLERS
#include "CAD.h"
#endif

#include "cad_full.h"
#include "CADDoc.h"

//so as to save view state
#include "Scroller.h"
#include "CADView.h"

#include "dxf.h"
//#include "CADParse.h"

// ..........................................................................
IMPLEMENT_DYNCREATE( CADDoc, CDocument )
BEGIN_MESSAGE_MAP( CADDoc, CDocument )
	ON_COMMAND( ID_FILE_EXPORT, &CADDoc::OnFileExport )
	ON_COMMAND( ID_FILE_G_CODE, &CADDoc::OnFileGCode )
END_MESSAGE_MAP( )

// ..........................................................................
BOOL CADDoc::OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo )
{
	auto cmd = LOWORD(nCode);
	if (HIWORD(nCode) == WM_NOTIFY)
	{
		DocNotifyObject* pHdr = (DocNotifyObject*)((AFX_NOTIFY*)pExtra)->pNMHDR;
		switch (cmd)
		{
		case ID_LAYERTREE_CHANGED:
		{
			pHdr->id;
			auto it = std::find_if(GetLayers().begin(), GetLayers().end(),
				[&pHdr](const cad_layer& layer) {return layer.tree_id == pHdr->id; });
			if (it != GetLayers().end())
			{
				const_cast<cad_layer&>(*it).enabled = pHdr->bVal;
				UpdateAllViews(nullptr, ID_INVALIDATE);
			}
			return TRUE;
			break;
		}
		default:
			break;
		}
	}
	//if ( HIWORD(nCode) == WM_NOTIFY )
	//{
	//	LPNMHDR pHdr= ((AFX_NOTIFY*)pExtra)->pNMHDR;
	//	switch( LOWORD( nCode ) )
	//	{
	//	case ID_GEAR_PARAM_CHANGED:
	//		auto pg= ( pgear_params_t )((PNMHDROBJECT)pHdr)->pObject;
	//		test( );
	//		UpdateAllViews( nullptr, ID_GEAR_PARAM_CHANGED );
	//		NMHDR* pnm= ((AFX_NOTIFY*)pExtra)->pNMHDR;
	//		TRACE( "doc: %x - %x - %x low: %x\n", nID, nCode, pnm->code, LOWORD( nCode ) );
	//	}
	//}
	return CDocument::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}

// ..........................................................................
CADDoc::CADDoc( )
//	:drawobj( layers )
{
}

// ..........................................................................
CADDoc::~CADDoc( )
{
}

// ..........................................................................
void CADDoc::OnFileGCode( )
{
	//testing g-code............
	//sp_gcitem_t test( boost::make_shared< gc0 >( ) );
	//gc.push_back( test );
	//gc.push_back( sp_gcitem_t( new gc0 ) );
}

template< typename Type >
inline SP_BaseItem  make_sp( Type& obj ) { return boost::make_shared< Type >( obj ); }
// ..........................................................................
void CADDoc::test( )
{
	//ItemSet set;
	//SP_BaseItem pt( PointItem( bg_point( 1, 1 ) ).get_SP( ) );
	//set.push_back( pt );
	//SP_BaseItem lt( LineItem( bg_point( -1, -.5 ), bg_point( 0, 0 ) ).get_SP( ) );
	//set.push_back( lt );
	//bg_box box= get_rect_hull( set.get_set( ) );
	//set.push_back( RectItem( box ).get_SP( ) );
	//RectItem ri( box );
	//GEO_NUM bx= ri.width( ) / 4;
	//ri.set_min_point(bx);
	//ri.set_max_point(bx);
	//GEO_NUM by= ri.height( ) / 4;
	//ri.set_min_point(by);
	//ri.set_max_point(by);
	//draw_extents= ri.get_bg_box( );
	//// the view will get the 'draw_extents'
	//UpdateAllViews( nullptr, ID_UPDATE_VIEW_TOFIT );
}

// ..........................................................................
BOOL CADDoc::OnNewDocument( )
{
	if( ! CDocument::OnNewDocument( ) )
		return FALSE;

	test( );

	return TRUE;
}

// ..........................................................................
void CADDoc::OnCloseDocument()
{
	CXML xml;
	xml.Open(get_user_folder()/"test_view_state.xml");//same as create new doc

	//Though the doc may not be modified, the state of the view(s) will be....
	auto pos = GetFirstViewPosition();
	for (; pos; )
	{
		CADView* pv = dynamic_cast<CADView*>(GetNextView(pos));
		ASSERT_VALID(pv);
		pv->SaveXML(xml.GetNode("/root"));
	}
	xml.Close();
	CDocument::OnCloseDocument();
}

// ..........................................................................
BOOL CADDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	return TRUE;
	//Ok, so far this is not going to work
	//too much doc/view baggage....
	CXML xml;
	xml.Open(get_user_folder() / "test_view_state.xml");//same as create new doc
	//and this won't work very well
	auto pos = GetFirstViewPosition();
	for (; pos; )
	{
		CADView* pv = dynamic_cast<CADView*>(GetNextView(pos));
		ASSERT_VALID(pv);
		pv->ReadXML(xml.GetNode("/root"));
	}
		//std::ifstream in( lpszPathName, std::ios_base::in );
		//if( ! in.is_open( ) )
		//	assert( false );

		////if( ! CadParse( in, &drawobj ) )
		////	return FALSE;

		//// TODO:  Add your specialized creation code here

		return TRUE;
}

// ..........................................................................
void expr_stream( std::wostream& os, const SP_BaseItem& in )
{
	typecase( *in,
		[ & ]( PointItem& pa ) {
//		os << " " << pa.get_bg_point( ).get_x( ) << " " << pa.get_bg_point( ).get_y( );
	},
		[ & ]( LineItem& pa ) {
		os << L"(line " << pa.get_bg_line( ).first.get_x( ) << " "
			<< pa.get_bg_line( ).first.get_y( ) << " "
			<< pa.get_bg_line( ).second.get_x( ) << " "
			<< pa.get_bg_line( ).second.get_y( ) << ")"
		;
	},
		[ & ]( const ArcItem& pa ) {
		os << "(arc "
			<< " " << pa.get_a( ).get_x( ) << " "
			<< " " << pa.get_a( ).get_y( ) << " "
			<< " " << pa.get_b( ).get_x( ) << " "
			<< " " << pa.get_b( ).get_y( ) << " "
			<< " " << pa.get_o( ).get_x( ) << " "
			<< " " << pa.get_o( ).get_y( ) << ")"
		;
	},
		[ & ]( const RectItem& pa ) {
	},
		[ & ]( const EllipesItem& pa ) {
	},
		[ & ]( const BezierCubeItem& pa ) {
	},
		[ & ]( const PolyItem& pa ) {
		os << L"(polyline ";
		for( auto l : pa )
			os << l.get_x( ) << L" " << l.get_y( ) << L" ";
		os << L")";
	});
}

// ..........................................................................
class expr_obj
{
public:
	expr_obj( const SP_BaseItem& in ) : obj( in ) {}
	friend std::wostream& operator<<( std::wostream& os, const expr_obj& sobj ) 
	{
		//os << "M ";
		os << std::fixed;
		expr_stream( os, sobj.obj );
		return os;
	}
private:
	const SP_BaseItem& obj;
};

// ..........................................................................
BOOL CADDoc::OnSaveDocument( LPCTSTR lpszPathName )
{
	std::wofstream os( lpszPathName );
	//doing it here for testing
	os << L"\
(HE_CAD (version 1)\n\
   (general\n\
      (area -1.0 -1.0 3.0 3.0)\n\
      (line_width .1)\n\
   )\n";

	for( auto& prims : drawobj )
	{
		os <<L"\
   (object (layer test_layer)\
";
		for( auto & item : prims )
		{
			os << L"         " << expr_obj( item ) << L"\n";
		}
		os << L"      )\n";
	}
	os << L")\n";
	return TRUE;
}

// ..........................................................................
bool StartDXF_write( DL_Dxf* dxf, DL_WriterA* dw );
bool EndDXF_write( DL_Dxf* dxf, DL_WriterA* dw );

// ..........................................................................
LRESULT CADDoc::OnLayerEnable(WPARAM, LPARAM )
{
	return 0;
}

// ..........................................................................
void CADDoc::OnFileExport( )
{
	DXF_WriteFile( bfs::path( "test2.dxf"), drawobj );
	return;
}

// ..........................................................................
void CADDoc::Serialize(CArchive& ar)
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
void CADDoc::AssertValid() const
{
	CDocument::AssertValid();
}

// ..........................................................................
void CADDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

#ifdef SHARED_HANDLERS

// ..........................................................................
// Support for thumbnails
void CADDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
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
void CADDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

// ..........................................................................
void CADDoc::SetSearchContent(const CString& value)
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


