
#pragma once

//#include "ViewTree.h"

class LayerToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class LayerTreeView : public CDockablePane
{
public:
	LayerTreeView();
	virtual ~LayerTreeView();

	void AdjustLayout();
	void OnChangeVisualStyle();

protected:
	LayerToolBar m_wndToolBar;
	CViewTreeEx layer_tree_view;
	CImageList m_ClassViewImages;
	UINT m_nCurrSort;

	operator CTreeCtrlEx* ( ) { return &layer_tree_view; }
	CTreeCtrlEx* GetCtrl( ) { return &layer_tree_view; }

public:
	void LoadLayerView( layer_set_t& layers, bool bClear = true );

// Overrides
public:
	virtual BOOL PreTranslateMessage( MSG* pMsg );

protected:
DECLARE_MESSAGE_MAP( )
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize( UINT nType, int cx, int cy );
	afx_msg void OnContextMenu( CWnd* pWnd, CPoint point );
	afx_msg void OnClassAddMemberFunction( );
	afx_msg void OnClassAddMemberVariable( );
	afx_msg void OnClassDefinition( );
	afx_msg void OnClassProperties( );
	afx_msg void OnNewFolder( );
	afx_msg void OnPaint( );
	afx_msg void OnSetFocus( CWnd* pOldWnd );
	afx_msg LRESULT OnChangeActiveTab( WPARAM, LPARAM );
	afx_msg void OnSort( UINT id );
	afx_msg void OnUpdateSort( CCmdUI* pCmdUI );

	afx_msg void OnLvnItemchangedEventlist( NMHDR *pNMHDR, LRESULT *pResult );
	afx_msg void OnLVNLButtonDown( NMHDR *pNMHDR, LRESULT *pResult );
};

