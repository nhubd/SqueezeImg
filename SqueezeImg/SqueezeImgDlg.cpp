// SqueezeImgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SqueezeImg.h"
#include "SqueezeImgDlg.h"
#include ".\squeezeimgdlg.h"
#include "ImgSqueeze.h"

#include "GLString.h"

using namespace GLC;
using GLC::CGLString;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSqueezeImgDlg dialog



CSqueezeImgDlg::CSqueezeImgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSqueezeImgDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSqueezeImgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSqueezeImgDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SQUEEZE, OnBnClickedButtonSqueeze)
END_MESSAGE_MAP()

// CSqueezeImgDlg message handlers

BOOL CSqueezeImgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSqueezeImgDlg::OnPaint() 
{
	if (IsIconic()) {
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSqueezeImgDlg::OnQueryDragIcon() {
	return static_cast<HCURSOR>(m_hIcon);
}

void CSqueezeImgDlg::OnBnClickedButtonSqueeze() {
	CString sSourceDir;
	GetDlgItemText(IDC_TXT_SOURCE_DIR, sSourceDir);
	m_wsSourceDirectory = (const wchar_t*)sSourceDir;

	CString sTargetDir;
	GetDlgItemText(IDC_TXT_TARGET_DIR, sTargetDir);
	m_wsTargetDirectory = (const wchar_t*)sTargetDir;

	try {
		squeeze();
	} catch (GLC::CGLException& exc) {
		MessageBox(GLC::CGLString(L"Cannot perform operation due to the following error:\r\n")
				+ exc.GetMessage(),
				GLC::CGLString(L"Error"), MB_OK);
	}
}

void CSqueezeImgDlg::squeeze() {
	CImgSqueeze prc;
	GLC::CGLStrings files = GLC::CGLFile::GetFiles(m_wsSourceDirectory);
	CGLString wsProcessed = L"Processed images: ";
	SetDlgItemText(IDC_STATIC_PROGRESS, wsProcessed + CGLString::FromInteger(0)
			+ L" of " + CGLString::FromInteger(files.Length()));
	processMessages();
	for (unsigned int i = 0; i < files.Length(); i++) {
		prc.SqueezeImage(files[i], m_wsTargetDirectory);
		SetDlgItemText(IDC_STATIC_PROGRESS, wsProcessed + CGLString::FromInteger(i + 1)
				+ L" of " + CGLString::FromInteger(files.Length()));
		processMessages();
	}
}

void CSqueezeImgDlg::processMessages() {
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}