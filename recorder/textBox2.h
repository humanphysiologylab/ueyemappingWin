#pragma once


// textBox2 dialog

class textBox2 : public CDialogEx
{
	DECLARE_DYNAMIC(textBox2)

public:
	textBox2(CWnd* pParent = nullptr);   // standard constructor
	virtual ~textBox2();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IDSSIMPLELIVE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
