// TestDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

//#define ISDEBUG//������壬��������ȡ��⣬��������壬���Ƿ�����

// CTestDlg �Ի���
class CTestDlg : public CDialogEx
{
// ����
public:
	CTestDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	BOOL GetMsg(CString source, CString& phone, CString& msg, CString& other);

	CEdit m_Disp;
	CListCtrl m_List;
	CButton m_Login;
	afx_msg void OnBnClickedLogin();
	CButton m_Pass;
	afx_msg void OnBnClickedPass();

	afx_msg void OnLvnItemchangedList3(NMHDR *pNMHDR, LRESULT *pResult);
};
