
#pragma once

//#include "ViewTree.h"

class TinyCADViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*)GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class TinyLibView : public CDockablePane
{
	// Construction
public:
	TinyLibView();

	void AdjustLayout();
	void OnChangeVisualStyle();
	CTreeCursor GetRootItem() { return wndTinyLibTreeView.GetRootItem(); }

	// Attributes
protected:

	CViewTreeEx wndTinyLibTreeView;
	CImageList m_FileViewImages;
	CFileViewToolBar m_wndToolBar;
	//
	CTreeCursor curSelected;

protected:
	void FillFileView();

	// Implementation
public:
	virtual ~TinyLibView();

protected:
DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnProperties();
	afx_msg void OnFileOpen();
	afx_msg void OnFileOpenWith();
	afx_msg void OnDummyCompile();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

