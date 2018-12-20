#pragma once

#include "../KiCAD_parse/KiCADStore.h"
// ..........................................................................
//struct tread_info {
//	tread_info() :bDone(false), bBreak(false) {}
//	bool bDone;
//	bool bBreak;
//};

// ..........................................................................
class KiCADDoc : public CADDoc
{
protected:
	KiCADDoc();
	DECLARE_DYNCREATE(KiCADDoc)
	//need to handle layers speciallty
	NSAssembly::LayerSet pLayers;
	//layer_set is populated on each pad translation
	using layer_set = std::vector<size_t>;
	layer_set ki_layers;
	void load_layers(const NSAssembly::string_vect& in);

public:
	BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

protected:
	DECLARE_MESSAGE_MAP()
};
