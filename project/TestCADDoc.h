#pragma once


// ..........................................................................
class TestCADDoc : public CADDoc
{
protected:
	TestCADDoc();
	DECLARE_DYNCREATE(TestCADDoc)

public:
	BOOL OnOpenDocument(LPCTSTR lpszPathName);

protected:
	DECLARE_MESSAGE_MAP()
};
