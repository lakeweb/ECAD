
// GearsView.h : interface of the CGearsView class
//

#pragma once

class CGearsView : public CADView
{
protected:
	CGearsView( );
	DECLARE_DYNCREATE( CGearsView )
public:
	CGearsDoc* GetDocument( ) const;

	bool bAnimate;
	bool bInPaint;
	public:
		virtual ~CGearsView( );

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
DECLARE_MESSAGE_MAP( )
	//afx_msg void OnToolsAnimate( );
	//afx_msg void OnUpdateToolsAnimate( CCmdUI *pCmdUI );
	afx_msg void OnTimer( UINT nIDEvent );
public:
	afx_msg void OnPaint();
};

#ifndef _DEBUG  // debug version in GearsView.cpp
inline CGearsDoc* CGearsView::GetDocument() const
   { return reinterpret_cast<CGearsDoc*>(m_pDocument); }
#endif
