#pragma once

class CBXLDoc : public CADDoc
{
protected:
	CBXLDoc();
	DECLARE_DYNCREATE(CBXLDoc)
	virtual void test();

public:
	BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//layer_set_t layers;
	//   DrawingObects drawobj;

/*
	bool bAnimate;

	//experimental, not used yet
	gc_process gc;

	std::string gear_info;
	DrawingObects export_drawobj;

	//void gear( );
	void test();
	void test(pgear_params_t pg);
	virtual DrawingObects& GetExportDrawObject();

	void gear_org();

public:
	DrawingObects & GetDrawingObjects() { return drawobj; }
	const std::string& GetInfoStr() const { return gear_info; }
	const layer_set_t& GetLayers() const { return layers; }
public:
	virtual ~CGearsDoc();
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
protected:
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
*/
protected:
	DECLARE_MESSAGE_MAP()
	/*
	afx_msg void OnFileExport();
	afx_msg LRESULT OnLayerEnable(WPARAM, LPARAM);
	afx_msg void OnFileGCode();
	afx_msg void OnToolsAnimate();
	afx_msg void OnUpdateToolsAnimate(CCmdUI *pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	*/
};
