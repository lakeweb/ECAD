#pragma once

// ..........................................................................
struct tread_info {
	tread_info() :bDone(false), bBreak(false) {}
	bool bDone;
	bool bBreak;
};

// ..........................................................................
class TinyCADDoc : public CADDoc
{
protected:
	TinyCADDoc();
	DECLARE_DYNCREATE(TinyCADDoc)

	doc_lib_list lib_list;
	tread_info sema;

	tinylib_item OpenTinyLib(uint64_t id);

public:
	BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//sp_layer_set_type layers;
	//   DrawingObect drawobj;

	std::thread thread;

	virtual void OnCloseDocument();

	void DisplaySymbol(tinylib_item& symbol);

	//void LoadTinyLibs();

	/*
	bool bAnimate;

	//experimental, not used yet
	gc_process gc;

	std::string gear_info;
	DrawingObect export_drawobj;

	//void gear( );
	void test();
	void test(pgear_params_t pg);
	virtual DrawingObect& GetExportDrawObject();

	void gear_org();

	public:
	DrawingObect & GetDrawingObjects() { return drawobj; }
	const std::string& GetInfoStr() const { return gear_info; }
	const sp_layer_set_type& GetLayers() const { return layers; }
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
public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnOpenTinyLibItem();
	/*
	afx_msg void OnFileExport();
	afx_msg LRESULT OnLayerEnable(WPARAM, LPARAM);
	afx_msg void OnFileGCode();
	afx_msg void OnToolsAnimate();
	afx_msg void OnUpdateToolsAnimate(CCmdUI *pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	*/
};
