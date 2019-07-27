


#include "stdafx.h"
#include <ctrlext.h>
#include "shared.h"
#include "view_share.h"
#include "mainfrm.h"
#include "TinyCADView.h"
#include "Resource.h"
#include "CAD.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// TinyLibView

TinyLibView::TinyLibView()
{
}

TinyLibView::~TinyLibView()
{
}

BEGIN_MESSAGE_MAP(TinyLibView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_COMMAND(ID_OPEN, OnFileOpen)
	ON_COMMAND(ID_OPEN_WITH, OnFileOpenWith)
	ON_COMMAND(ID_DUMMY_COMPILE, OnDummyCompile)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar message handlers

int TinyLibView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create view:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	if (!wndTinyLibTreeView.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("Failed to create file view\n");
		return -1;      // fail to create
	}

	// Load view images:
	m_FileViewImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	wndTinyLibTreeView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* Is locked */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	// Fill in some static tree view data (dummy code, nothing magic here)
	FillFileView();
	AdjustLayout();

	return 0;
}

void TinyLibView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void TinyLibView::FillFileView()
{
	curSelected = wndTinyLibTreeView;
	CTreeCursor cur = wndTinyLibTreeView.InsertItem(L"Files", 0, 0);

	wndTinyLibTreeView.SetItemState(cur, TVIS_BOLD, TVIS_BOLD);
	auto bxl = cur.AddTail(L"Tiny Lib Files", 0, 0);

	bxl.AddTail(L"some_tinylib_.file", 1, 1);
}

void TinyLibView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&wndTinyLibTreeView;
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
			curSelected = hTreeItem;
		}
	}
	pWndTree->SetFocus();
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
}

void TinyLibView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}
	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	wndTinyLibTreeView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void TinyLibView::OnProperties()
{
}

void TinyLibView::OnFileOpen()
{
	//char buf[555];
	//int bytes = GetModuleFileNameA(NULL, buf, 555);
	auto cur = wndTinyLibTreeView.GetSelectedItem();
	DocNotifyObject ndr;
	ndr.nmHdr.code = ID_LAYERTREE_CHANGED;
	ndr.nmHdr.hwndFrom = GetSafeHwnd();
	ndr.nmHdr.idFrom = 37;// (DWORD)(HTREEITEM)cur;
	ndr.id = (uint64_t)(HTREEITEM)cur;
	AfxGetMainWnd()->SendMessage(WM_NOTIFY, 1, (WPARAM)&ndr);
}

void TinyLibView::OnFileOpenWith()
{
	// TODO: Add your command handler code here
}

void TinyLibView::OnDummyCompile()
{
	// TODO: Add your command handler code here
}

void TinyLibView::OnEditCut()
{
	// TODO: Add your command handler code here
}

void TinyLibView::OnEditCopy()
{
	// TODO: Add your command handler code here
}

void TinyLibView::OnEditClear()
{
	// TODO: Add your command handler code here
}

void TinyLibView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	wndTinyLibTreeView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void TinyLibView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	wndTinyLibTreeView.SetFocus();
}

void TinyLibView::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* Locked */);

	m_FileViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_FILE_VIEW_24 : IDB_FILE_VIEW;

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

	m_FileViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_FileViewImages.Add(&bmp, RGB(255, 0, 255));

	wndTinyLibTreeView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);
}

void TinyLibView::OnLButtonDown(UINT nFlags, CPoint point)
{
	assert(false);
	DocNotifyObject ndr;
	ndr.nmHdr.code = ID_LAYERTREE_CHANGED;
	ndr.nmHdr.hwndFrom = GetSafeHwnd();
	ndr.nmHdr.idFrom = 37;
	AfxGetMainWnd()->SendMessageToDescendants(WM_NOTIFY, (WPARAM)&ndr);

	CDockablePane::OnLButtonDown(nFlags, point);
}
