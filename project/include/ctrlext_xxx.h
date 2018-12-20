// ctrlext.h :
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __TREECTLX_H__
#define __TREECTLX_H__

#ifdef _AFX_NO_AFXCMN_SUPPORT
	#error Windows Common Control classes not supported in this library variant.
#endif

#ifndef __AFXWIN_H__
	#include <afxwin.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// TREECTLX - MFC Tree Control Helper Classes

class CTreeCtrlEx;

/////////////////////////////////////////////////////////////////////////////
// CTreeCursor

class CTreeCursor
{
	// Attributes
protected:
	HTREEITEM	m_hTreeItem;
	CTreeCtrlEx	*m_pTree;

	// Implementation
protected:
	CTreeCursor CTreeCursor::_Insert( LPCTSTR strItem, int nImageNorm, int nImageSel, HTREEITEM hAfter );

	// Operation
public:
	CTreeCursor( );
	CTreeCursor( HTREEITEM hTreeItem, CTreeCtrlEx* pTree );
	CTreeCursor( const CTreeCursor& posSrc );
	~CTreeCursor( );
	const CTreeCursor& operator = ( const CTreeCursor& posSrc );
	operator HTREEITEM( ) const;
	bool IsValid( ) const;
	void SetInvalid( );

	CTreeCursor InsertAfter( LPCTSTR strItem, HTREEITEM hAfter, int nImageNorm= -1, int nImageSel= -1 );
	CTreeCursor AddHead( LPCTSTR strItem, int nImageNorm= -1, int nImageSel= -1 );
	CTreeCursor AddTail( LPCTSTR strItem, int nImageNorm= -1, int nImageSel= -1 );

	int GetImageID( );
	int GetImageIDSel( );

	BOOL GetRect( LPRECT lpRect, BOOL bTextOnly );
	CTreeCursor GetNext( UINT nCode );
	CTreeCursor GetChild( );
	CTreeCursor GetNextSibling( );
	CTreeCursor GetPrevSibling( );
	CTreeCursor GetParent( );
	CTreeCursor GetFirstVisible( );
	CTreeCursor GetNextVisible( );
	CTreeCursor GetPrevVisible( );
	CTreeCursor GetSelected( );
	CTreeCursor GetDropHilight( );
	CTreeCursor GetRoot( );
	CString GetText( );
	BOOL GetImage( int& nImage, int& nSelectedImage );
	UINT GetState( UINT nStateMask );
	DWORD GetData( ) const;
	//BOOL SetItem( UINT nMask, LPCTSTR lpszItem, int nImage,
	//	int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam );
	BOOL SetText( LPCTSTR lpszItem );
	BOOL SetImage( int nImage, int nSelectedImage= -1 );
	BOOL SetState( UINT nState, UINT nStateMask );
	BOOL SetData( DWORD dwData );
	BOOL HasChildren( );
	//returns count of real children
	int GetChildCount( ); //5/31/7

	// Operations
	BOOL Delete( );
	BOOL Expand(UINT nCode= TVE_EXPAND );
	BOOL Collapse( );
	BOOL Select( UINT nCode );
	BOOL Select( );
	BOOL SelectDropTarget( );
	BOOL SelectSetFirstVisible( );
	CEdit* EditLabel( );
	CImageList* CreateDragImage( );
	BOOL SortChildren( );
	BOOL EnsureVisible( );
	void DeleteAllChildren( ); //added 3/26/4
	CTreeCtrlEx& GetTreeCtrl( ); //added 5/14/7
	//will appear as having children [+] without real children
	void SetHasChildern( ); //added 5/30/7
	bool IsExpanded( ) const;
};

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEx

class CTreeCtrlEx : public CTreeCtrl
{
	DECLARE_DYNAMIC( CTreeCtrlEx )
	bool IsSelectedItem( HTREEITEM hItem );

public:
	CTreeCtrlEx( );
	virtual ~CTreeCtrlEx( );
	operator CTreeCtrlEx * ( ) { return this; }
	CImageList* SetImageList( CImageList* pImageList, int nImageListType = TVSIL_NORMAL );

	CTreeCursor GetNextItem( HTREEITEM hItem, UINT nCode );
	CTreeCursor GetChildItem( HTREEITEM hItem );
	CTreeCursor GetNextSiblingItem( HTREEITEM hItem );
	CTreeCursor GetPrevSiblingItem( HTREEITEM hItem );
	CTreeCursor GetParentItem( HTREEITEM hItem );
	CTreeCursor GetFirstVisibleItem( );
	CTreeCursor GetNextVisibleItem( HTREEITEM hItem );
	CTreeCursor GetPrevVisibleItem( HTREEITEM hItem );
	CTreeCursor GetSelectedItem( );
	CTreeCursor GetDropHilightItem( );
	CTreeCursor GetRootItem( );
	CTreeCursor InsertItem( LPTV_INSERTSTRUCT lpInsertStruct );

	CTreeCursor InsertItem( UINT nMask, LPCTSTR lpszItem, int nImage,
		int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam,
		HTREEITEM hParent, HTREEITEM hInsertAfter );

	CTreeCursor InsertItem( LPCTSTR lpszItem, HTREEITEM hParent = TVI_ROOT,
		HTREEITEM hInsertAfter = TVI_LAST );

	CTreeCursor InsertItem( LPCTSTR lpszItem, int nImage, int nSelectedImage,
		HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST );

	CTreeCursor HitTest( CPoint pt, UINT* pFlags = NULL );
	CTreeCursor HitTest( TV_HITTESTINFO* pHitTestInfo );
	//returns true if children and expanded

protected:
	DECLARE_MESSAGE_MAP( )
	afx_msg BOOL OnNMCustomdrawTreeEx( NMHDR *pNMHDR, LRESULT *pResult );
};

class CTreeCtrlTraverse
{
	CTreeCtrlEx& tree_ctrl;
	HTREEITEM root;
	HTREEITEM tree_pos;
	long depth;

public:
	//constructor sets traverse start to root
	CTreeCtrlTraverse::CTreeCtrlTraverse( CTreeCtrlEx& inTC )
		:tree_ctrl( inTC )
		,depth( 1 )
	{
		ASSERT( (HTREEITEM)inTC.GetRootItem( ) );
		root= tree_pos= inTC.GetRootItem( );
	}
	//constructor sets traverse start to tree cursor
	CTreeCtrlTraverse::CTreeCtrlTraverse( CTreeCursor& inPos )
		:tree_ctrl( inPos.GetTreeCtrl( ) )
		,depth( 1 )
	{
		ASSERT( (HTREEITEM)inPos );
		root= tree_pos= inPos;
	}
	CTreeCursor GetCurrentNode( ) const { return CTreeCursor( tree_pos, &tree_ctrl ); }
	//Return next node and increment
	CTreeCursor GetNextNode( );
	//Returns root set by instantiation
	CTreeCursor GetRootNode( ) const { return CTreeCursor( tree_pos, &tree_ctrl ); }
	//Get depth in tree
	long GetDepth( ) const { return depth; }
	void Reset( ) { depth= 1; tree_pos= root; }
};

/////////////////////////////////////////////////////////////////////////////
// CViewTree window

//class CViewTree : public CTreeCtrl
class CViewTreeEx : public CTreeCtrlEx
{
	// Construction
public:
	CViewTreeEx();

	// Overrides
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	// Implementation
public:
	virtual ~CViewTreeEx();

protected:
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx

//class CListCtrlEx : public CListCtrl
//{
//	// Attributes
//protected:
//
//	// Operation
//public:
//	CListCtrlEx();
//	~CListCtrlEx();
//	CImageList* SetImageList(CImageList* pImageList, int nImageListType = TVSIL_NORMAL);
//	BOOL AddColumn(
//		LPCTSTR strItem,int nItem,int nSubItem = -1,
//		int nMask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
//		int nFmt = LVCFMT_LEFT);
//	BOOL AddItem(int nItem,int nSubItem,LPCTSTR strItem,int nImageIndex = -1);
//};


/////////////////////////////////////////////////////////////////////////////

#include <CtrlExt.inl>

#endif //__TREECTLX_H__
