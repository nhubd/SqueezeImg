// SqueezeImgDlg.h : header file
//

#pragma once


// CSqueezeImgDlg dialog
class CSqueezeImgDlg : public CDialog
{
// Construction
public:
	CSqueezeImgDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SQUEEZEIMG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSqueeze();

private:
	GLC::CGLString m_wsSourceDirectory;
	GLC::CGLString m_wsTargetDirectory;

	void squeeze();
	void processMessages();
};
