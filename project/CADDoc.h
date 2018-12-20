#pragma once

/*
DrawingObects
	owns a object_set_t called primatives 

object_set_t a vector of ItemSet;

ItemSet
	owns asp_obj_vect_t
	which is a vector of shared pointers SP_BaseItem, to BaseItem
*/

class CADDoc : public CDocument
{
protected:
	CADDoc( );
	DECLARE_DYNCREATE( CADDoc )

	layer_set_t layers;
	DrawingObects drawobj;

	bg_box draw_extents;
	//test from menu........

	virtual void test( );
	virtual DrawingObects& GetExportDrawObject( ) { return drawobj; }
public:
	DrawingObects& GetDrawingObjects( ) { return drawobj; }
	const layer_set_t& GetLayers( ) const { return layers; }
public:
	virtual ~CADDoc( );
	virtual BOOL OnNewDocument( );
	virtual void Serialize( CArchive& ar );
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent( );
	virtual void OnDrawThumbnail( CDC& dc, LPRECT lprcBounds );
protected:
	void SetSearchContent( const CString& value );
#endif // SHARED_HANDLERS

public:
#ifdef _DEBUG
	virtual void AssertValid( ) const;
	virtual void Dump( CDumpContext& dc ) const;
#endif

protected:
	DECLARE_MESSAGE_MAP( )
	afx_msg void OnFileExport( );
	afx_msg void OnFileGCode( );
	afx_msg LRESULT OnLayerEnable( WPARAM, LPARAM );
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
};
