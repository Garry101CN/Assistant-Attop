#pragma once
#ifndef LOGINDLG_H
#define LOGINDLG_H
#include "afxwin.h"
#include "resource.h"
// LoginDlg �Ի���

class LoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(LoginDlg)

public:
	LoginDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~LoginDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOGLOGIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_Username;
	CEdit m_Password;
	CButton m_Confirm;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
#endif