
#include "stdafx.h"
#include <ctrlext.h>
#include "shared.h"
#include "view_share.h"
#include "MainFrm.h"
#include "LayerView.h"
#include "Resource.h"
#include "CAD.h"

// ..............................................................................
namespace {
	enum layer_img_id {
		eye_on,
		eye_off,
	};
}

// ..............................................................................
class LayerViewMenuButton : public CMFCToolBarMenuButton
{
	friend class LayerTreeView;
	DECLARE_SERIAL(LayerViewMenuButton)

public:
	LayerViewMenuButton(HMENU hMenu = NULL) : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

// ..............................................................................
IMPLEMENT_SERIAL(LayerViewMenuButton, CMFCToolBarMenuButton, 1)
BEGIN_MESSAGE_MAP(LayerTreeView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_CLASS_ADD_MEMBER_FUNCTION, OnClassAddMemberFunction)
	ON_COMMAND(ID_CLASS_ADD_MEMBER_VARIABLE, OnClassAddMemberVariable)
	ON_COMMAND(ID_CLASS_DEFINITION, OnClassDefinition)
	ON_COMMAND(ID_CLASS_PROPERTIES, OnClassProperties)
	ON_COMMAND(ID_NEW_FOLDER, OnNewFolder)

	ON_COMMAND_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnSort)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnUpdateSort)

	ON_NOTIFY(TVN_SELCHANGED, ID_LAYER_TREECTRL, OnLvnItemchangedEventlist)
	ON_NOTIFY(TVN_ITEMCHANGED, ID_LAYER_TREECTRL, OnLvnItemchangedEventlist)
	ON_NOTIFY(TCN_LMOUSEBUTTON, ID_LAYER_TREECTRL, OnLVNLButtonDown)

	//	ON_MESSAGE( ID_TVN_CHECKCHANGED, OnCheckChanged )
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// ..............................................................................
LayerTreeView::LayerTreeView( )
{
	m_nCurrSort = ID_SORTING_GROUPBYTYPE;
}

// ..............................................................................
LayerTreeView::~LayerTreeView( )
{
}

// ..............................................................................
void LayerTreeView::OnLVNLButtonDown( NMHDR *pNMHDR, LRESULT *pResult )
{
	TREECLICKA& info= *reinterpret_cast< TREECLICKA* >( pNMHDR );
	if( info.hit_info.flags & TVHT_NOWHERE )
		return;

	*pResult = 0;

	CTreeCursor tc( info );
	if( info.hit_info.flags & TVHT_ONITEMICON )
	{
		DocNotifyObject ndr;
		if( tc.GetImageID( ) == eye_on )
		{
			tc.SetImage( eye_off, eye_off );
			//SendMessage( 1234 );
			ndr.bVal= false;
		}
		else
		{
			tc.SetImage( eye_on, eye_on );
			ndr.bVal = true;
		}
		ndr.nmHdr.code = ID_LAYERTREE_CHANGED;
		ndr.nmHdr.hwndFrom = GetSafeHwnd();
		ndr.nmHdr.idFrom = 37;// (DWORD)(HTREEITEM)cur;
		ndr.id = (uint32_t)(HTREEITEM)tc;
		AfxGetMainWnd()->SendMessage(WM_NOTIFY, 1, (WPARAM)&ndr);

		*pResult = 1;

		//dock_notify_type note(dock_notify_type::layer, tc.GetData(), (uint32_t)(HTREEITEM)tc);
		//TRACE("the message: %d\n", CMD_TEST);
		//AfxGetMainWnd()->SendMessage(WM_NOTIFY, 2, (WPARAM)&ndr);
		//::SendMessage(*AfxGetMainWnd(), CMD_TEST, (WPARAM)&note, NULL);
	}

	TRACE( "OnLButtonDown: %s, %x\n", tc.GetText( ) );

	//CDockablePane::OnLButtonDown( info.hdr., point);
}

// ..............................................................................
int LayerTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create views:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if( ! layer_tree_view.Create( dwViewStyle, rectDummy, this, ID_LAYER_TREECTRL ) )
	{
		TRACE0("Failed to create Layer_View treectrl\n");
		return -1;      // fail to create
	}

	// Load images:
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_SORT);
	m_wndToolBar.LoadToolBar(IDR_SORT, 0, 0, TRUE /* Is locked */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	CMenu menuSort;
	menuSort.LoadMenu(IDR_POPUP_SORT);
	m_wndToolBar.ReplaceButton(ID_SORT_MENU, LayerViewMenuButton(menuSort.GetSubMenu(0)->GetSafeHmenu()));
	LayerViewMenuButton* pButton =  DYNAMIC_DOWNCAST(LayerViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != NULL)
	{
		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;
		pButton->SetImage(GetCmdMgr()->GetCmdImage(m_nCurrSort));
		pButton->SetMessageWnd(this);
	}
	HTREEITEM hRoot = layer_tree_view.InsertItem(_T("Primary Layers"), 0, 0);
	return 0;
}

// ..............................................................................
void LayerTreeView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

// ..............................................................................
void LayerTreeView::OnLvnItemchangedEventlist( NMHDR *pNMHDR, LRESULT *pResult )
{
	TRACE( "OnLvnItemchangedEventlist begin\n" );
	TVHITTESTINFO ht = { 0 };
//	if( pNMHDR->code  & TVN_ITEMCHANGED )
	{
		DWORD dwpos = GetMessagePos( );

		ht.pt.x = GET_X_LPARAM( dwpos );
		ht.pt.y = GET_Y_LPARAM( dwpos );
		::MapWindowPoints( HWND_DESKTOP, pNMHDR->hwndFrom, &ht.pt, 1 );

		TreeView_HitTest( pNMHDR->hwndFrom, &ht );

		if( TVHT_ONITEMSTATEICON & ht.flags )
			; //TODO????????
		TRACE( "OnLvnItemchangedEventlist: %x\n", ht.flags );
			//PostMessage( ID_TVN_CHECKCHANGED, 0, (LPARAM)ht.hItem );
	}
}

// ..............................................................................
void LayerTreeView::LoadLayerView( sp_layer_set_type& layers, bool bClear)
{
	if(bClear)
		layer_tree_view.DeleteAllItems( );
	CTreeCursor cur( layer_tree_view.GetRootItem( ).AddTail( _T("Layers"), 0, 0 ) );

	cur.SetState( TVIS_BOLD, TVIS_BOLD );
	for( auto& layer : layers )
	{
		layer_img_id img= layer.enabled ? eye_on : eye_off;
		CTreeCursor child( cur.AddTail( layer.label.c_str( ), img, img ) );
		child.SetData( layer.id );// Hmmmmmmmm... so.
//		const_cast<cad_layer&>(layer).tree_id = (uint32_t)(HTREEITEM)child;
		layer.tree_id = (uint32_t)(HTREEITEM)child;
	}
	cur.Expand( );
}

// ..............................................................................
void LayerTreeView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&layer_tree_view;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// Select clicked item:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	CMenu menu;
	menu.LoadMenu(IDR_POPUP_SORT);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}
}

// ..............................................................................
void LayerTreeView::AdjustLayout( )
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	layer_tree_view.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

// ..............................................................................
BOOL LayerTreeView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
	if (pMsg->message != 200)
	{
		int test = 0;
	}
}

// ..............................................................................
void LayerTreeView::OnSort( UINT id )
{
	if (m_nCurrSort == id)
	{
		return;
	}

	m_nCurrSort = id;

	LayerViewMenuButton* pButton =  DYNAMIC_DOWNCAST(LayerViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != NULL)
	{
		pButton->SetImage(GetCmdMgr()->GetCmdImage(id));
		m_wndToolBar.Invalidate();
		m_wndToolBar.UpdateWindow();
	}
}

// ..............................................................................
void LayerTreeView::OnUpdateSort(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nCurrSort);
}

// ..............................................................................
void LayerTreeView::OnClassAddMemberFunction()
{
	AfxMessageBox(_T("Add member function..."));
}

// ..............................................................................
void LayerTreeView::OnClassAddMemberVariable()
{
}

// ..............................................................................
void LayerTreeView::OnClassDefinition()
{
}

// ..............................................................................
void LayerTreeView::OnClassProperties()
{
}

// ..............................................................................
void LayerTreeView::OnNewFolder()
{
	AfxMessageBox(_T("New Folder..."));
}

// ..............................................................................
void LayerTreeView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	layer_tree_view.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect( rectTree, ::GetSysColor( COLOR_3DSHADOW ), ::GetSysColor( COLOR_3DSHADOW ) );
}

// ..............................................................................
void LayerTreeView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	layer_tree_view.SetFocus();
}

// ..............................................................................
void LayerTreeView::OnChangeVisualStyle()
{
	m_ClassViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_CLASS_VIEW_24 : IDB_CLASS_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_ClassViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ClassViewImages.Add(&bmp, RGB(255, 0, 0));

	layer_tree_view.SetImageList(&m_ClassViewImages, TVSIL_NORMAL);

	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_SORT_24 : IDR_SORT, 0, 0, TRUE /* Locked */);
}


