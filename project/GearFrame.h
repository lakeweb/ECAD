#pragma once


// .......................................................................
class FrmRuler : public CMFCBaseToolBar
{
protected:
	long winSize;
	bool bHorz;

public:
	void SetWidth( long width )
	{
		winSize= width;
		ASSERT_KINDOF( CFrameWnd, GetParent( ) );
		static_cast<CFrameWnd*>( GetParent( ) )->RecalcLayout( );
	}
	long GetWidth( ) const { return winSize; }

protected:
	virtual CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz )
	{
		if( bHorz )
			return CSize( 0, winSize );
		else
			return CSize( winSize, 0 );
	}

	//virtual void OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler ) { }
	//virtual CSize CalcDynamicLayout( int nLength, DWORD nMode )  { return CSize( 10, 10 ); }
public:
	virtual BOOL Create(CWnd* pParentWnd, UINT nID= 0x00, long size= 0, bool bHorz= true );

	DECLARE_MESSAGE_MAP( )
	afx_msg void OnPaint( );
};

// .......................................................................
class GearFrame : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(GearFrame)
public:
	GearFrame();

	FrmRuler hRule;
	FrmRuler vRule;
	CMFCStatusBar statusBar;

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//virtual void GetMessageString( UINT nID, CString& rMessage ) const;

	// Implementation
public:
	virtual ~GearFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg LRESULT OnViewPost( WPARAM pos, LPARAM );
	afx_msg void OnUpdateStatusText( CCmdUI *pCmdUI );
};

