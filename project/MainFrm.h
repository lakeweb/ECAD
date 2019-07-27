
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "resource.h"
#include "FileView.h"
#include "LayerView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "GearDialog.h"
#include "TinyCADView.h"

//#include "Ruler.h"

class CMainFrame : public CMDIFrameWndEx
{
	
protected: // create from serialization only
public:
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

	//FrmRuler hRule;
	//FrmRuler vRule;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// Implementation
public:
	virtual ~CMainFrame();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);

protected:  // control bar embedded members

	CMFCMenuBar			m_wndMenuBar;
	CMFCToolBar			m_wndToolBar;
	CMFCStatusBar		statusBar;
	CMFCToolBarImages	m_UserImages;

	CFileView			m_wndFileView;
	CPropertiesWnd		m_wndProperties;
	//
	LayerTreeView		layer_tree_view;
	CGearsDialog		gear_dlg;
	TinyLibView			tinylib_yiew;
//
	COutputWnd			m_wndOutput;

// Generated message map functions
protected:
DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnInfoNotify( WPARAM wp, LPARAM lp );
	afx_msg LRESULT OnDockableMsg(WPARAM, LPARAM);
	afx_msg LRESULT OnTinyWnd(WPARAM, LPARAM);
	afx_msg void OnUpdateStatusText(CCmdUI *pCmdUI);
};


