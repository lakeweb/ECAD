#pragma once

/*
DrawingObect
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

	//sp_layer_set_type layers;
	DrawingObect drawobj;

	bg_box draw_extents;
	//test from menu........

	//const SP_cad_layer GetLayer(size_t pos) const { assert(pos >= 0 && pos < drawobj.layers.size()); return drawobj.GetLayers().at(pos);}

	virtual void test( );
	virtual DrawingObect& GetExportDrawObject( ) { return drawobj; }
public:
	DrawingObect& GetDrawingObjects( ) { return drawobj; }
	sp_layer_set_type& GetLayers() { return drawobj.GetLayers(); }
public:
	virtual ~CADDoc( );
	virtual BOOL OnNewDocument( );
	virtual void Serialize( CArchive& ar );
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();

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
};
