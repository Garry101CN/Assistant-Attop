// LoginDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LoginDlg.h"
#include "afxdialogex.h"
#include "Func.h"
#include "Code.h"

CString Username;
CString Password;

BOOL isConfirm;//�ж��Ƿ���ȷ����ť���رյĶԻ���
// LoginDlg �Ի���

IMPLEMENT_DYNAMIC(LoginDlg, CDialogEx)

LoginDlg::LoginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOGLOGIN, pParent)
{

}

LoginDlg::~LoginDlg()
{
}

void LoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, m_Username);
	DDX_Control(pDX, IDC_EDIT3, m_Password);
	DDX_Control(pDX, IDOK, m_Confirm);
}


BEGIN_MESSAGE_MAP(LoginDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &LoginDlg::OnBnClickedOk)
END_MESSAGE_MAP()

void LoginDlg::OnBnClickedOk()
{
	m_Username.GetWindowText(Username);
	m_Password.GetWindowText(Password);

	if (Username.IsEmpty() || Password.IsEmpty()) {
		AfxMessageBox(L"��Ϣδ������ȫ�����飡", MB_ICONERROR);
		return;
	}

	isConfirm = TRUE;
	CDialogEx::OnOK();
}


BOOL LoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//Username.Empty();
	//Password.Empty();
	isConfirm = FALSE;
	m_Username.SetWindowText(Username);

	return TRUE;
}