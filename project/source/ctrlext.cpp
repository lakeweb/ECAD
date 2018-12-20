// ctrlext.cpp :
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "ctrlext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

#define _AFXCTL_INLINE inline

const CTreeCursor& CTreeCursor::operator = ( const CTreeCursor& posSrc )
{
	if( &posSrc != this ){
		m_hTreeItem= posSrc.m_hTreeItem;
		m_pTree= posSrc.m_pTree;
	}
	return *this;
}

CTreeCursor CTreeCursor::_Insert( LPCTSTR strItem, int nImageNorm, int nImageSel, HTREEITEM hAfter )
{
	TV_INSERTSTRUCT ins;
	ins.hParent= m_hTreeItem;
	ins.hInsertAfter= hAfter;
	ins.item.mask= TVIF_TEXT;
	ins.item.pszText= (LPTSTR)strItem;
	if( nImageNorm != -1 )
	{
		ins.item.mask|= TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		ins.item.iImage= nImageNorm;
		ins.item.iSelectedImage= nImageSel == -1 ? nImageNorm : nImageSel;
	}
	return CTreeCursor( m_pTree->InsertItem( &ins ), m_pTree );
}

int CTreeCursor::GetChildCount( )
{
	HTREEITEM hChildItem = m_pTree->GetChildItem( m_hTreeItem );
	int count= 0;
	for( ; hChildItem; )
	{
		++count;
		hChildItem= m_pTree->GetNextSiblingItem( hChildItem );
	}
	return count;
}

int CTreeCursor::GetImageID( )
{
	TV_ITEM item;
	item.mask= TVIF_HANDLE | TVIF_IMAGE;
	item.hItem= m_hTreeItem;
	m_pTree->GetItem( &item );
	return item.iImage;
}

void CTreeCursor::DeleteAllChildren( )
{
	HTREEITEM item;
	while( m_hTreeItem != NULL && m_pTree->ItemHasChildren( m_hTreeItem ) )
	{
		item= m_pTree->GetChildItem( m_hTreeItem );
		if( m_pTree->ItemHasChildren( item ) )
		{
			CTreeCursor tc( item, m_pTree );
			tc.DeleteAllChildren( );
		}
		m_pTree->DeleteItem( item );
	}		

//if (pmyTreeCtrl->ItemHasChildren(hmyItem))
//{
//   HTREEITEM hNextItem;
//   HTREEITEM hChildItem = pmyTreeCtrl->GetChildItem(hmyItem);
//
//   while (hChildItem != NULL)
//   {
//      hNextItem = pmyTreeCtrl->GetNextItem(hChildItem, TVGN_NEXT);
//      pmyTreeCtrl->DeleteItem(hChildItem);
//      hChildItem = hNextItem;
//   }
//}
}

void CTreeCursor::SetHasChildern( )
{
	TVITEM tvi;
	tvi.cChildren= 1;
	tvi.mask= TVIF_CHILDREN;
	tvi.hItem= m_hTreeItem;
	m_pTree->SetItem( &tvi );
}

// ...........................................
CTreeCursor CTreeCtrlTraverse::GetNextNode( )
{
	CTreeCursor tc;

	if( !depth )
		return CTreeCursor( NULL, &tree_ctrl );

	if( CTreeCursor( tree_pos, &tree_ctrl ).HasChildren( ) )
	{
		++depth;
		tree_pos= CTreeCursor( tree_pos, &tree_ctrl ).GetChild( );
		return CTreeCursor( tree_pos, &tree_ctrl );
	}
	else if( CTreeCursor( tree_pos, &tree_ctrl ).GetNextSibling( ).IsValid( ) )
	{
		tree_pos= CTreeCursor( tree_pos, &tree_ctrl ).GetNextSibling( );
		return CTreeCursor( tree_pos, &tree_ctrl );
	}
	else
	{
		do
		{
			tree_pos= CTreeCursor( tree_pos, &tree_ctrl ).GetParent( ).GetNextSibling( );
		}
		while( --depth && ! tree_pos );
		return CTreeCursor( tree_pos, &tree_ctrl );
	}
}

//IMPLEMENT_DYNCREATE( CTreeCtrlEx, CTreeCtrl )
IMPLEMENT_DYNAMIC( CTreeCtrlEx, CTreeCtrl )
BEGIN_MESSAGE_MAP( CTreeCtrlEx, CTreeCtrl )
	ON_WM_LBUTTONDOWN( )
END_MESSAGE_MAP()

void CTreeCtrlEx::OnLButtonDown( UINT nFlags, CPoint point ) 
{
	TREECLICKA nr;
	nr.hit_info= { point, 0, NULL };
	HitTest( &nr.hit_info );

	nr.hdr.code= TCN_LMOUSEBUTTON;//NOTE: in this projects resource range
	nr.hdr.hwndFrom= GetSafeHwnd( );
	nr.hdr.idFrom= GetDlgCtrlID( );
	nr.mouse_flags= nFlags;
	nr.ptClick= point;
	nr.pClass= this;

//		nr.dwFlags= 0;
	if( GetParent( )->SendMessage( WM_NOTIFY, GetParent( )->GetDlgCtrlID( ), (LPARAM)&nr ) )
		return;

	CTreeCtrl::OnLButtonDown( nFlags, point );
//	OnButtonDown( TRUE, nFlags, point);
}

/*
// .....................................................................
IMPLEMENT_DYNAMIC( CTreeCtrlEx, CMultiTree )
BEGIN_MESSAGE_MAP( CTreeCtrlEx, CMultiTree )
	ON_WM_LBUTTONDOWN( )
	ON_WM_KEYDOWN( )

END_MESSAGE_MAP( )

void CTreeCtrlEx::OnLButtonDown( UINT nFlags, CPoint point )
{
	// Set focus to control if key strokes are needed.
	// Focus is not automatically given to control on lbuttondown
     
	m_dwDragStart = GetTickCount( );
     
	if(nFlags & MK_CONTROL )
	{
		// Control key is down
		UINT flag;
		HTREEITEM hItem = HitTest( point, &flag );
		if( hItem )
		{
			// Toggle selection state
			UINT uNewSelState =
			GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED ?
			0 : TVIS_SELECTED;
			// Get old selected (focus) item and state
			HTREEITEM hItemOld = GetSelectedItem( );
			UINT uOldSelState = hItemOld ?	GetItemState( hItemOld, TVIS_SELECTED ) : 0;
			// Select new item
			if( GetSelectedItem() == hItem )
			SelectItem( NULL );	// to prevent edit
//			CTreeCtrl::OnLButtonDown( nFlags, point );
     
			// Set proper selection (highlight) state for new item
			SetItemState(hItem, uNewSelState, TVIS_SELECTED);
     
			// Restore state of old selected item
			if (hItemOld && hItemOld != hItem)
			SetItemState(hItemOld, uOldSelState, TVIS_SELECTED);
     
			m_hItemFirstSel = NULL;
     
			return;
		}
	}
	else if(nFlags & MK_SHIFT)
	{
		// Shift key is down
		UINT flag;
		HTREEITEM hItem = HitTest( point, &flag );
     
		// Initialize the reference item if this is the first shift selection
		if( !m_hItemFirstSel )
		m_hItemFirstSel = GetSelectedItem();
     
		// Select new item
		if( GetSelectedItem() == hItem )
		SelectItem( NULL );	// to prevent edit
		CTreeCtrl::OnLButtonDown(nFlags, point);
     
		if( m_hItemFirstSel )
		{
			SelectItems( m_hItemFirstSel, hItem );
			return;
		}
	}
	else
	{
		// Normal - remove all selection and let default
		// handler do the rest
		ClearSelection( );
		m_hItemFirstSel = NULL;
	}
	CTreeCtrl::OnLButtonDown(nFlags, point);
}

// SelectItems - Selects items from hItemFrom to hItemTo. Does not
// - select child item if parent is collapsed. Removes
// - selection from all other items
// hItemFrom - item to start selecting from
// hItemTo - item to end selection at.
BOOL CTreeCtrlEx::SelectItems( HTREEITEM hItemFrom, HTREEITEM hItemTo )
{
	HTREEITEM hItem = GetRootItem();
 
	// Clear selection upto the first item
	while ( hItem && hItem!=hItemFrom && hItem!=hItemTo )
	{
	hItem = GetNextVisibleItem( hItem );
	SetItemState( hItem, 0, TVIS_SELECTED );
	}
 
	if ( !hItem )
	return FALSE;	// Item is not visible
 
	SelectItem( hItemTo );
 
	// Rearrange hItemFrom and hItemTo so that hItemFirst is at top
	if( hItem == hItemTo )
	{
	hItemTo = hItemFrom;
	hItemFrom = hItem;
	}
 
 
	// Go through remaining visible items
	BOOL bSelect = TRUE;
	while ( hItem )
	{
	// Select or remove selection depending on whether item
	// is still within the range.
	SetItemState( hItem, bSelect ? TVIS_SELECTED : 0, TVIS_SELECTED );
 
	// Do we need to start removing items from selection
	if( hItem == hItemTo )
	bSelect = FALSE;
 
	hItem = GetNextVisibleItem( hItem );
	}
 
	return TRUE;
}     

void CTreeCtrlEx::ClearSelection( )
{
	// This can be time consuming for very large trees
	// and is called every time the user does a normal selection
	// If performance is an issue, it may be better to maintain
	// a list of selected items
	for ( HTREEITEM hItem=GetRootItem(); hItem!=NULL; hItem= CTreeCtrl::GetNextItem( hItem, TVGN_NEXT ) )
		if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
			SetItemState( hItem, 0, TVIS_SELECTED );
}

void CTreeCtrlEx::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if ( (nChar==VK_UP || nChar==VK_DOWN) && GetKeyState( VK_SHIFT )&0x8000)
	{
	// Initialize the reference item if this is the first shift selection
	if( !m_hItemFirstSel )
	{
	m_hItemFirstSel = GetSelectedItem();
	ClearSelection();
	}
     
	// Find which item is currently selected
	HTREEITEM hItemPrevSel = GetSelectedItem();
     
	HTREEITEM hItemNext;
	if ( nChar==VK_UP )
	hItemNext = GetPrevVisibleItem( hItemPrevSel );
	else
	hItemNext = GetNextVisibleItem( hItemPrevSel );
     
	if ( hItemNext )
	{
	// Determine if we need to reselect previously selected item
	BOOL bReselect =
	!( GetItemState( hItemNext, TVIS_SELECTED ) & TVIS_SELECTED );
     
	// Select the next item - this will also deselect the previous item
	SelectItem( hItemNext );
     
	// Reselect the previously selected item
	if ( bReselect )
	SetItemState( hItemPrevSel, TVIS_SELECTED, TVIS_SELECTED );
	}
	return;
	}
	else if( nChar >= VK_SPACE )
	{
	m_hItemFirstSel = NULL;
	ClearSelection( );
	}
	CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}



HTREEITEM CTreeCtrlEx::GetFirstSelectedItemX( )
{
	for ( HTREEITEM hItem = GetRootItem( ); hItem!=NULL; hItem = GetNextItem( hItem, TVGN_NEXT ) )
	if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
	return hItem;
     
	return NULL;
}
     
HTREEITEM CTreeCtrlEx::GetNextSelectedItemX( HTREEITEM hItem )
{
	for ( hItem = GetNextItem( hItem, TVGN_NEXT ); hItem!=NULL; hItem = GetNextItem( hItem, TVGN_NEXT ) )
	if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
	return hItem;
     
	return NULL;
}
     
HTREEITEM CTreeCtrlEx::GetPrevSelectedItemX( HTREEITEM hItem )
{
	for ( hItem = GetPrevVisibleItem( hItem ); hItem!=NULL; hItem = GetPrevVisibleItem( hItem ) )
	if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
	return hItem;
     
	return NULL;
}

*/

