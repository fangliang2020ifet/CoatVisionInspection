#pragma once

// CMyDocument document
class CDialogView;

class CMyDocument : public CDocument
{
	//DECLARE_DYNCREATE(CMyDocument)
	friend class CDialogView;

public:
	CMyDocument();
	DECLARE_DYNCREATE(CMyDocument)
public:
	virtual ~CMyDocument();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);// overridden for document i/o
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
};
