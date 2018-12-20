#pragma once



// TestCADView.h
//
#pragma once

class TestCADView : public CADView
{
protected:
	TestCADView();
	DECLARE_DYNCREATE(TestCADView)
public:
	TestCADDoc * GetDocument() const;

public:
	virtual ~TestCADView();
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnInitialUpdate(); // called first time after construct

#ifdef _DEBUGx
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
	//afx_msg void OnToolsAnimate( );
	//afx_msg void OnUpdateToolsAnimate( CCmdUI *pCmdUI );
public:
	afx_msg void OnPaint();
};

#ifndef _DEBUGx  // debug version in GearsView.cpp
inline TestCADDoc* TestCADView::GetDocument() const
{
	return reinterpret_cast<TestCADDoc*>(m_pDocument);
}
#endif
