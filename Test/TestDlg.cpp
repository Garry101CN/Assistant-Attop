// TestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Test.h"
#include "TestDlg.h"

#include "HttpClient.h"
#include "Func.h"
#include "code.h"
#include "LoginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define POSTTKTIME 3000

CString str;		//����ֵ
CString PostData;	//������ÿ�������ﶨ���ˣ�ֱ�Ӷ����ȫ�ֱ���
CString Header;		//Э��ͷ����
CString DWRSESSIONID;	//ÿ�η��ʶ���ҪGetScriptSessionId��DWRSESSIONID�Ǳز����ٵ�
HttpClient w;


CStringArray AllCourseID;	//���еĿγ̴���
CStringArray AllCourseName;	//���еĿγ�����
CStringArray Quene_Section;	//����ˢʱ����½ڣ���ˢ��ʱ����ֵ����ˢʱ��ʱʹ��
CString CourseAnswer;		//�ÿγ̵Ĵ�

BOOL Endthread;			//����ˢ���߳�

extern CString Username;
extern CString Password;
extern BOOL isConfirm;		//�ж��Ƿ���ȷ����ť���رյĶԻ���

BOOL login();
DWORD WINAPI ThreadNextCourse(LPVOID pParam);
void ThreadNextSection(CString CourseID, CString CourseName, CStringArray &AllSectionID, CStringArray &AllSectionName);
void ThreadPostExes(CString CourseID, CString CourseName, CString SectionID, CString SectionName);
void InitPostTime();
void PostTime(CString CourseID, CString CourseName, CString SectionID, CString SectionName);	//ˢʱ��
void MediaPj(CString allinfo, CString CourseName, CString SectionName);				//ý������

BOOL WriteAnswerToFile(CString path, CString Answer);
CString ReadAnswerFromFile(CString path);
BOOL RefreshAnswerFile();			//����վ�����ش�
void EndCourse();				//����ˢ��
BOOL RefreshCourse();				//ˢ�¿γ�
CString GetSectionStatus(CString CourseID, CString SectionID, DWORD batchId);
BOOL RefreshCourseGrade(CString courseid);	//ˢ��ĳ�γ̵ĳɼ�
BOOL GetSection(CString Courseid, CStringArray &Sectionid, CStringArray &Sectionname);//��ȡCourseid��Ӧ���½�
CString GetScriptSessionId();			//��ȡScriptSessionId
CString OrderAnswer(CString Answer, DWORD n);

void OutPut(CString s)
{
	/*�ڱ༭�����������*/
	AfxGetMainWnd()->GetDlgItem(IDC_EDIT_PUT)->SetWindowText(s);
	UpdateWindow(AfxGetMainWnd()->GetDlgItem(IDC_EDIT_PUT)->m_hWnd);
}
void OutPutAppend(CString s)
{
	/*�ڱ༭���и�������s*/
	CEdit* edit = (CEdit*)(AfxGetMainWnd()->GetDlgItem(IDC_EDIT_PUT));
	CString str;
	edit->GetWindowText(str);
	str = str + s;
	edit->SetWindowText(str);
	edit->LineScroll(edit->GetLineCount());//���������һ��
	UpdateWindow(AfxGetMainWnd()->GetDlgItem(IDC_EDIT_PUT)->m_hWnd);
}

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};
CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()
CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PUT, m_Disp);
	DDX_Control(pDX, IDC_LIST3, m_List);
	DDX_Control(pDX, IDC_LOGIN, m_Login);
	DDX_Control(pDX, IDC_PASS, m_Pass);
}
BEGIN_MESSAGE_MAP(CTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_LOGIN, &CTestDlg::OnBnClickedLogin)
	ON_BN_CLICKED(IDC_PASS, &CTestDlg::OnBnClickedPass)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST3, &CTestDlg::OnLvnItemchangedList3)
END_MESSAGE_MAP()
BOOL CTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu = TEXT("����");
		//strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_Disp.SetWindowText(GetString(12));

	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT);//����ѡ��
	m_List.InsertColumn(0, TEXT("�γ̴���"), LVCFMT_LEFT, 80);
	m_List.InsertColumn(1, TEXT("�γ�����"), LVCFMT_LEFT, 270);
	m_List.InsertColumn(2, TEXT("�ɼ�"), LVCFMT_LEFT, 70);
	m_List.InsertColumn(3, TEXT("ѧϰ����"), LVCFMT_LEFT, 110);
	m_List.InsertColumn(4, TEXT("״̬"), LVCFMT_LEFT, 70);
	
	m_Login.GetFocus();

	srand((unsigned)time(NULL));
	

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}
void CTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}
void CTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}
HCURSOR CTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestDlg::OnBnClickedLogin()
{
	RefreshAnswerFile();
	do 
	{
		LoginDlg dlg;
		dlg.DoModal();
		if (!isConfirm)return;
	} while (!login());

#ifndef ISDEBUG
	//����һ̨������������û�ʹ��
	if (TestUserName(Username, 1000) == FALSE) {
		AfxMessageBox(GetString(16), MB_ICONERROR);
		return;
	}
#endif

	/*��¼�ɹ�*/
	OutPut(TEXT("��½�ɹ�����ʼ��ȡ�����ڽ��еĿγ�...û�еĻ�ҪȥNPC��������"));

	if (!RefreshCourse())
	{
		OutPut(TEXT("��¼�ɹ�������ѯ����Щ�γ�ʧ�ܣ������µ�¼��"));
		AfxMessageBox(TEXT("��¼�ɹ�������ѯ����Щ�γ�ʧ�ܣ������µ�¼��"), MB_OK | MB_ICONWARNING);
		return ;
	}
	if (m_List.GetItemCount() == 0)
	{
		AfxMessageBox(TEXT("��¼�ɹ������ǻ�ƣ�\n�����û���κογ�Ҳ����ȥ��ȡ��������������"), MB_OK | MB_ICONWARNING);
	}
	else
	{
		m_Pass.EnableWindow(TRUE);
		m_Pass.SetFocus();
		OutPut(TEXT("��¼����ˣ��������ˢ�ΰ�ť��ʼˢ���£�����~\r\n��ʾ�������ʱ��û�п�ʼˢ�Σ���ô����Ҫ���µ�¼��"));
		AfxMessageBox(TEXT("��¼����ˣ���㿪ʼˢ������...ϴˢˢϴˢˢ��"), MB_OK | MB_ICONINFORMATION);
	}
}
BOOL login()
{
	CString str;
	/*��¼�������ص�¼�ɹ����httpinfo��DWRSESSIONID*/

	OutPut(TEXT("���ڵ�¼..."));
	/*ģ���һ�δ���ҳ����ȡ��JSESSIONID���е���*/
	str = w.Get(TEXT("http://www.attop.com/"));
	/*���û��û����þ��ֶ�����һ��JSESSIONID*/
	if (w.GetCookie().IsEmpty())
	{
		CString JSESSIONID;
		JSESSIONID = TEXT("AAFE06E597DA8BCE86043B3099E") + GetRandStr(10000, 99999);//�������JSESSIONID
		OutputDebugString(TEXT("�ֶ����ɵ�JSESSIONID��") + JSESSIONID + TEXT("\n"));
		w.AddCookie(TEXT("JSESSIONID"), JSESSIONID);
	}
	else
	{
		OutputDebugString(TEXT("���") + w.GetCookie() + TEXT("\n"));
	}

	/*��ȡDWRSESSIONID*/
	w.AddSendHeader(TEXT("Referer"), TEXT("http://www.attop.com/index.htm"));
	str = w.Post(GetString(101), GetString(102));

	DWRSESSIONID = SubString(str, TEXT("r.handleCallback(\"0\",\"0\",\""), TEXT("\");"));
	OutputDebugString(TEXT("��õ�DWRSESSIONID��") + DWRSESSIONID + TEXT("\n"));
	if (DWRSESSIONID.IsEmpty())
	{
		AfxMessageBox(TEXT("��ȡDWRSESSIONID����,������û��Ӵ��"), MB_OK | MB_ICONERROR);
		return FALSE;
	}
	else
	{
		w.AddCookie(TEXT("DWRSESSIONID"), DWRSESSIONID);
	}
	OutputDebugString(TEXT("��õ�Cookies��") + w.GetCookie() + TEXT("\n"));

	/*��ȡͼƬ��֤��*/
	w.Get(GetString(LOGIN_IMAGE));
	CString authorCode = w.GetCookie(TEXT("rand"));
	w.AddSendHeader(GetString(105));


	/*��������ʼ��¼*/
	w.AddSendHeader(GetString(LOGIN105));
	w.AddCookie(TEXT("rand"), GetRandStr(1000, 9999));
	PostData = GetString(LOGIN104) + GetScriptSessionId();
	PostData.Replace(TEXT("Username"), Username);
	PostData.Replace(TEXT("Password"), Password);
	PostData.Replace(TEXT("AuthorCode"), authorCode);
	str = w.Post(GetString(LOGIN103), PostData);

	OutputDebugString(TEXT("��½�󷵻ص����ݣ�\n") + str + TEXT("\n"));

	if (str.Find(TEXT("flag:-4")) != -1|| str.Find(TEXT("flag:20")) != -1)
	{
		AfxMessageBox(TEXT("���Ѷ�����������˺Ż����룬�����������"), MB_OK | MB_ICONERROR);
		return FALSE;
	}
	if (str.Find(TEXT("flag:1")) == -1)
	{
		AfxMessageBox(TEXT("��¼ʧ�ܣ�����û����������ǲ�������ˣ����һ�£�"), MB_OK | MB_ICONWARNING);
		return FALSE;
	}
	return TRUE;
}

BOOL pressed;
void CTestDlg::OnBnClickedPass()
{
	if (pressed)
	{
		if (AfxMessageBox(TEXT("�������ˢ�Σ���ȷ��Ҫֹͣ��"), MB_ICONQUESTION | MB_OKCANCEL) != IDOK)return;
		m_Pass.SetWindowText(TEXT("ֹͣ��"));
		m_Pass.EnableWindow(FALSE);

		Endthread = TRUE;
		pressed = FALSE;
	}
	else
	{
		OutputDebugString(TEXT("��Ҫˢ�Ŀγ̣�\n"));
		for (int i = 0; i < m_List.GetItemCount(); i++)
		{
#ifdef ISDEBUG
			OutputDebugString(m_List.GetItemText(i, 0) + TEXT(".") + m_List.GetItemText(i, 1) + TEXT("\n"));
			AllCourseID.Add(m_List.GetItemText(i, 0));
			AllCourseName.Add(m_List.GetItemText(i, 1));
#else
			if (m_List.GetItemText(i, 4) == TEXT("ѧϰ��") && m_List.GetItemText(i, 2) != TEXT("100"))
			{
				/*
				pszFileName = TEXT("TK\\") + m_List.GetItemText(i, 0) + TEXT(".txt");
				if (!myFile.Open(pszFileName, CFile::modeNoTruncate | CFile::typeBinary | CFile::modeRead | CFile::shareDenyNone))
				{
					AfxMessageBox(m_List.GetItemText(i, 0) + TEXT(" ") + m_List.GetItemText(i, 1)+TEXT("\n�ÿγ�û����⣬�����Զ����⣬�뵽������վ���أ�") , MB_ICONINFORMATION);
				}
				else
				{
					myFile.Close();
				}
				*/
				OutputDebugString(m_List.GetItemText(i, 0) + TEXT(".") + m_List.GetItemText(i, 1) + TEXT("\n"));
				AllCourseID.Add(m_List.GetItemText(i, 0));
				AllCourseName.Add(m_List.GetItemText(i, 1));
			}
#endif
		}
#ifndef ISDEBUG
		if (AllCourseID.GetCount()==0)
		{
			AfxMessageBox(TEXT("���ֵܣ�������пγ���û����Ҫˢ�ĿΣ��������ѧ�ԣ�ѧ�����أ�\n���ѧ���˿�ȥ��ҵ�ɣ�"),MB_OK);
			return;
		}
#endif
		m_Pass.SetWindowText(TEXT("ֹͣˢ��"));
		pressed = TRUE;
		Endthread = FALSE;
		OutPut(TEXT("��ʼˢ�Σ�\r\n"));

		AfxBeginThread((AFX_THREADPROC)ThreadNextCourse, NULL);//ˢ������
		//AfxBeginThread((AFX_THREADPROC)ThreadPostTime, NULL);//ˢʱ������
	}
	
}

DWORD WINAPI ThreadNextCourse(LPVOID pParam)//ˢ��������,���γ�
{
	/*��Ҫ�ǻ��γ̣�һ���γ�ˢ���ˣ�ˢ��һ���γ̣���ȡ���γ������½�*/

	CString CourseID;//��ǰ���еĿγ̴���
	CString CourseName;//��ǰ���еĿγ�����
	CStringArray AllSectionID;//ÿ���γ�����ÿ�ڵ�ID
	CStringArray AllSectionName;//ÿ���γ�����ÿ�ڵ�����

#ifndef ISDEBUG
	if (!RefreshAnswerFile()) {
		//�����и��£�û���²��ܼ���ʹ��
		return 0;
	}
#endif
	for (int i = 0; i <= AllCourseID.GetCount(); i++)
	{
		if (i== AllCourseID.GetCount())//���пγ�������۶�ˢ����
		{
#ifndef ISDEBUG 
			if (Endthread == TRUE)EndCourse();
			InitPostTime();//ˢʱ������
#else
			EndCourse();
#endif
			return 0;
		}
		else//�����µĿγ�
		{
			CourseID = AllCourseID.GetAt(i);
			CourseName = AllCourseName.GetAt(i);
		}

		if (GetSection(CourseID,AllSectionID,AllSectionName))//��ȡ�����½�
		{
#ifdef ISDEBUG 
			CString tmp;
			tmp.Format(TEXT("%lld"), GetCurrentTime());
			tmp = TEXT("<Version>") + tmp + TEXT("</Version>\r\n");
			WriteAnswerToFile(TEXT("TK\\") + CourseID + TEXT(".txt"), tmp);
#else
			CourseAnswer.Empty();
			CourseAnswer = ReadAnswerFromFile(TEXT("TK\\") + CourseID + TEXT(".txt"));//��ȡ�γ̴�
#endif
			if (Endthread == TRUE)EndCourse();
			ThreadNextSection(CourseID, CourseName, AllSectionID, AllSectionName);//��ʼ��ǰ�γ�
			if (Endthread == TRUE)EndCourse();
		}
		else
		{
			OutPut(CourseID + TEXT(" ") + CourseName + TEXT("\r\n��ȡ�γ��½�ʧ�ܣ�\r\n"));
		}
	}
	return 0;
}
void ThreadNextSection(CString CourseID, CString CourseName, CStringArray &AllSectionID, CStringArray &AllSectionName)
{
	/*�л��½ڣ�һ���γ̵�ĳһ��ˢ�����Ҫ�л�����һ��*/
	CString SectionID;//��ǰ���нڵ�ID
	CString SectionName;//��ǰ���нڵ�����

	for (int i = 0; i <= AllSectionID.GetCount(); i++)
	{
		if (i == AllSectionID.GetCount())//�����½ڶ�ˢ����
		{
			RefreshCourseGrade(CourseID);
			OutputDebugString(TEXT("�Ѿ�ˢ��ÿγ�\n"));
			return ;//��һ�γ�
		}
		else//�����½�
		{
			SectionID = AllSectionID.GetAt(i);
			SectionName = AllSectionName.GetAt(i);
		}
		OutputDebugString(TEXT("��ʼˢ�ýڿγ̣�") + SectionID + TEXT(".") + SectionName + TEXT("\n"));
		ThreadPostExes(CourseID, CourseName, SectionID, SectionName);
		RefreshCourseGrade(CourseID);
		if (Endthread == TRUE)EndCourse();
	}
}
void ThreadPostExes(CString CourseID, CString CourseName, CString SectionID, CString SectionName)
{
	/*��ĳ�µ���һ���ύ������ȡĳ�µ����д�*/

	CString Exes;//һ����������
	CString ExesName;//������ͺ͵ڼ���
	CString Question;//����ͱ�ѡ��
	CString Answer;//���ύ�Ĵ�
	CString AnswerID;//���ID
	CString Selection;//ĳ��ѡ��
	CString allinfo;
	CString tmp;
	DWORD Pos = 0;
	DWORD Pos1 = 0;

	/*��ȡ����״̬*/
	CString Status;
	BOOL Status_xiti=FALSE;
	BOOL Status_meiti=FALSE;
	Status = GetSectionStatus(CourseID, SectionID, 1);
	if (Status.Find(TEXT("OK</strong> \\r\\n                 <span class=\\\"explain_rate\\\"><a href=\\\"javascript:;\\\" onclick=\\\"atPage(\\\'ʱ��˵��")) == -1)
	{
		/*ʱ��ûˢ�꣬�����ˢ����*/
		Quene_Section.Add(TEXT("|") + CourseID + TEXT("|") + CourseName + TEXT("|") + SectionID + TEXT("|") + SectionName + TEXT("|"));
	}
	if (Status.Find(TEXT("OK</strong> \\r\\n                 <span class=\\\"explain_rate\\\"><a href=\\\"javascript:;\\\" onclick=\\\"atPage(\\\'ϰ��˵��")) != -1)
	{
		/*ϰ��ˢ����*/
		Status_xiti = TRUE;
	}
	if (Status.Find(TEXT("OK</strong> \\r\\n             <span class=\\\"explain_rate\\\"><a href=\\\"javascript:;\\\" onclick=\\\"atPage(\\\'ý��˵��")) != -1)
	{
		/*ý��ˢ����*/
		Status_meiti = TRUE;
	}
#ifndef ISDEBUG
	if (Status_xiti == TRUE&&Status_meiti == TRUE)
	{
		OutPutAppend(TEXT("��������ɣ�") + CourseName + TEXT(" ") + SectionName + TEXT("\r\n"));
		return;//ȫ��ˢ���˾ͷ���
	}
#endif
	
	/*��ʼ��ȡ���ڵ��������ݣ�����ý�����ۺ�ϰ��*/

	Header = GetString(117) + GetString(11);
	Header.Replace(TEXT("CourseID"), CourseID);
	Header.Replace(TEXT("SectionID"), SectionID);
	w.AddSendHeader(Header);
	PostData = GetString(116) + GetScriptSessionId();
	PostData.Replace(TEXT("CourseID"), CourseID);
	PostData.Replace(TEXT("SectionID"), SectionID);
	str = w.Post(GetString(115), PostData);

#ifndef ISDEBUG
	if(Status_meiti!=TRUE)MediaPj(str, CourseName, SectionName);//��ý������
#endif

	str = SubString(str, TEXT("<span class=\\\"delNum"), TEXT("</dd>"));
	str = HexStrToWChars(str);

	if (Endthread == TRUE)EndCourse();
#ifdef ISDEBUG
	/*�����µ���ȫ����ȡ���������ں�̨����ȡ*/
	WriteAnswerToFile(TEXT("TK\\") + CourseID + TEXT(".")+ CourseName+TEXT(".txt"), TEXT("\r\n\r\n��")+CourseID + TEXT(".") + CourseName + TEXT("  ") + SectionName+TEXT("��\r\n"));
	while (1)
	{
		Exes.Empty(); ExesName.Empty(); Question.Empty(); AnswerID.Empty(); tmp.Empty(); Selection.Empty();
		Pos1 = 0;
		Exes = SubString(str, TEXT("<li name=\\\"xt\\\""), TEXT("\\r\\n                   \\r\\n                 </li>"), Pos);//ÿһ����
		if (Exes.IsEmpty())
		{
			Exes = SubString(str, TEXT("<li>"), TEXT("\\r\\n\\r\\n                  \\r\\n"), Pos);//���ֿ��ܵĽ�β
		}
		if (Exes.IsEmpty())//��һ�ڵ�������
		{
			Answer = Answer.Left(Answer.GetLength() - 3);//ȥ��ĩβ��&;&
			Answer = TEXT("<") + SectionID + TEXT(">") + Answer + TEXT("</") + SectionID + TEXT(">\r\n");
			WriteAnswerToFile(TEXT("TK\\") + CourseID + TEXT(".txt"),Answer);
			OutputDebugString(TEXT("\n�ýڵ����Ѿ�ˢ�꣬��һ�ڣ�") + SectionName + TEXT("\r\n"));
			return ;//����һ�ڣ�
		}
		//if (Exes.Find(TEXT("ed-ans")) == -1)continue;//�ж��Ƿ�ش���ȷ���������ȷ�Ͳ�Ҫ
		ExesName = SubString(Exes, TEXT("<h5>"), TEXT("\\r\\n"), Pos1);//�ڼ���
		AnswerID = SubString(Exes, TEXT("id=\\\""), TEXT("\\\""));
		AnswerID = AnswerID.Right(AnswerID.GetLength() - 3);//xt_4260 dx_2182 mx_2180 pd_2187ֻҪ���������
		Answer = Answer + AnswerID + TEXT("&=&");

		if (ExesName.Find(TEXT("�����")) != -1)
		{
			DWORD Last = 0;
			DWORD Pos2 = 0;
			Question = SubString(Exes, TEXT("<p>\\r\\n\\t\\t\\t\\t\\t\\t"), TEXT("\\r\\n                   </p>"), Pos1);
			while (1)//ȡ���м�����
			{
				Pos2 = Question.Find(TEXT("\\r\\n                             "), Last);
				if (Pos2 == -1)
				{
					Pos2 = Question.Find(TEXT("\\r\\n\\t\\t\\t\\t\\t "), Last);
				}
				if (Pos2 == -1)
				{
					tmp = tmp + Question.Right(Question.GetLength() - Last);
					break;
				}
				tmp = tmp + Question.Mid(Last, Pos2 - Last) + TEXT("��   ��");
				Last = Pos2;
				Answer = Answer + SubString(Question, TEXT("value=\\\""), TEXT("\\\" />��"), Last) + TEXT("&,&");
				if (Question.Find(TEXT("\\r\\n\\t\\t\\t\\t\\t\\t"), Last) == -1)
				{
					Last = Last + 6;//  \" />���ĳ���
				}
				else
				{
					Last = Question.Find(TEXT("\\r\\n\\t\\t\\t\\t\\t\\t"), Last);
					Last = Last + 16;// \r\n\t\t\t\t\t\t�ĳ���
				}
			}
			if(Answer.Right(3)=TEXT("&,&"))
				Answer = Answer.Left(Answer.GetLength() - 3);//ȥ��ĩβ��&,&
			Question = TEXT("\r\nQuestion��") + tmp + TEXT("\r\nAnswer��") + Answer + TEXT("\r\n");
			//OutputDebugString(TEXT("\nQuestion��") + Question);
		}
		else if (ExesName.Find(TEXT("��ѡ��")) != -1 || ExesName.Find(TEXT("��ѡ��")) != -1 || ExesName.Find(TEXT("�ж���")) != -1)
		{
			CString xuanze;
			int n = 0;
			Question = SubString(Exes, TEXT("<p>"), TEXT("</p>"), Pos1);
			//OutputDebugString(TEXT("\nQuestion��") + Question + TEXT("\nAnswer��\n"));
			Question = TEXT("\r\nQuestion��") + Question + TEXT("\r\nAnswer��\r\n");
			while (1)
			{
				Selection = SubString(Exes, TEXT("<li"), TEXT("\\r\\n"), Pos1);
				if (Selection.IsEmpty())break;
				if ((Selection.Find(TEXT("checked=\\\"checked\\\"")) != -1))
				{
					xuanze = xuanze + SubString(Selection, TEXT("value=\\\""), TEXT("\\\"")) + TEXT(",");
					n++;
					//Answer = Answer + SubString(Selection, TEXT("value=\\\""), TEXT("\\\"")) + TEXT(",");
					Question = Question + TEXT("��");
					//OutputDebugString(TEXT("��"));
				}
				Question = Question + SubString(Selection, TEXT("/>"), TEXT("</li>")) + TEXT("\r\n");
				//OutputDebugString(SubString(Selection, TEXT("/>"), TEXT("</li>")) + TEXT("\n"));
			}

			Answer = Answer + OrderAnswer(xuanze, n);
		}
		OutPut(CourseID + TEXT(".") + CourseName + TEXT("  ") + SectionName + TEXT("\r\n") + ExesName + TEXT("��") + Question);
		WriteAnswerToFile(TEXT("TK\\") + CourseID + TEXT(".") + CourseName + TEXT(".txt"), TEXT("\r\n��") + ExesName + TEXT("��") + Question);
		Answer = Answer + TEXT("&;&");
		//c0-e3=string:4287&=&2&;&4276&=&1,2&;&4277&=&2
	}//һ��������
#else
	/*�ύ���µ����*/
	Answer = SubString(CourseAnswer, TEXT("<") + SectionID + TEXT(">"), TEXT("</") + SectionID + TEXT(">"));
	if (str.Find(TEXT("��ȷ�ʣ�100%")) != -1|| Status_xiti==TRUE)
	{
		OutPutAppend(TEXT("�����������꣺") + CourseName + TEXT(" ") + SectionName + TEXT("\r\n"));
		return;
	}
	if (!Answer.IsEmpty())//����⣬������û���
	{
		OutPutAppend(TEXT("�ύ��⣺") + CourseName + TEXT(" ") + SectionName + TEXT("\r\n"));
		Header = GetString(117);
		Header.Replace(TEXT("CourseID"), CourseID);
		Header.Replace(TEXT("SectionID"), SectionID);
		w.AddSendHeader(Header);
		PostData = GetString(119) + GetScriptSessionId();
		CString t = UrlDecodeUTF8(Answer);
		PostData.Replace(TEXT("Answer"), t);
		PostData.Replace(TEXT("CourseID"), CourseID);
		PostData.Replace(TEXT("SectionID"), SectionID);
		str = w.Post(GetString(118), PostData);
#ifndef ISDEBUG
		OutPutAppend(TEXT("�ȴ���һ�½�...\r\n"));
		Sleep(POSTTKTIME);//��ʱһ��ʱ�䣬����һ��
#endif
	}
	else
	{
		OutPutAppend(TEXT("����⣺") + CourseName + TEXT(" ") + SectionName + TEXT("\r\n"));
	}
#endif
}
void MediaPj(CString allinfo, CString CourseName, CString SectionName)
{
	/*�Կγ��е�ý���������*/
	CString MediaID;
	DWORD pos = 0;
	CString s;
		
	OutPutAppend(TEXT("�ύý�����ۣ�") + CourseName + TEXT(" ") + SectionName + TEXT("\r\n"));
	while (1)
	{
		if (Endthread == TRUE)EndCourse();
		MediaID = SubString(allinfo, TEXT("parent.showMediaRight("), TEXT(")"), pos);
		if (MediaID.IsEmpty())
		{
			OutputDebugString(TEXT("����ý���Ѿ������꣺") + CourseName + TEXT(" ") + SectionName + TEXT("\n"));
			break;
		}
		OutputDebugString(TEXT("�����ύý�����ۣ�") + CourseName + TEXT(" ") + SectionName + TEXT(" ") + MediaID + TEXT(" "));
		Header = GetString(122);
		Header.Replace(TEXT("MediaID"), MediaID);
		w.AddSendHeader(Header);
		PostData = GetString(121) + GetScriptSessionId();
		PostData.Replace(TEXT("MediaID"), MediaID);
		s = w.Post(GetString(118), PostData);//�ύ
		OutputDebugString(TEXT(" ���ύ\n"));
	}
}

void InitPostTime()
{
	/*
	����ˢʱ����߳�
	�ͻ���ÿ15�������������һ�����ݣ�ÿ�η��ͣ�batchId++�������͵��Ĵ�ʱ
	�ͻ��˻��ٴη�����������ȡ��ǰ�γ̵�ʣ��ʱ��

	����ʹ��N���߳���ˢʱ�䣬���Դ���ȼӿ�ˢʱ��
	*/
	OutPut(TEXT("��ʼˢʱ�䣺\r\n"));

	CString section;
	CString CourseID;//��ǰ���еĿγ̴���
	CString CourseName;//��ǰ���еĿγ�����
	CString SectionID;
	CString	SectionName;
	DWORD iPos;

	int NumSection = Quene_Section.GetCount();
	for (int i = 0; i <NumSection; i++)
	{
		/*ȡ��һ��*/
		iPos = 0;
		section = Quene_Section.GetAt(0);
		Quene_Section.RemoveAt(0);
		CourseID = SubString(section, TEXT("|"), TEXT("|"), iPos);
		CourseName = SubString(section, TEXT("|"), TEXT("|"), iPos);
		SectionID = SubString(section, TEXT("|"), TEXT("|"), iPos);
		SectionName = SubString(section, TEXT("|"), TEXT("|"), iPos);

		PostTime( CourseID, CourseName, SectionID, SectionName);
		if (Endthread == TRUE)EndCourse();
		RefreshCourseGrade(CourseID);
	}
}
void PostTime(CString CourseID, CString CourseName, CString SectionID, CString SectionName)
{
	CString str;
	DWORD begin = 1;
	DWORD batchId = begin;
	CString LastTime=TEXT("---");//�ϴ�ʣ��ʱ��

	CString t;//һ��û�õı���
	while (1)
	{
		/*ÿˢһ��ʱ�䶼��ȡһ��ʣ��ʱ�䣬�������Զ��Ӧ�ó���ˢ��ʱ�����ˢ��ʱ��*/
		if (Endthread == TRUE)EndCourse();
		str = GetSectionStatus(CourseID, SectionID, batchId);
		if (str.Find(TEXT("OK</strong> \\r\\n                 <span class=\\\"explain_rate\\\"><a href=\\\"javascript:;\\\" onclick=\\\"atPage(\\\'ʱ��˵��")) != -1)
		{
			/*ˢ����*/
			OutPutAppend(TEXT("���½ڵ�ʱ��ˢ���ˣ�") + CourseName + TEXT(" ") + SectionName+TEXT("\r\n"));
			OutputDebugString(TEXT("���½ڵ�ʱ��ˢ���ˣ�") + CourseName +TEXT(" ")+ SectionName + TEXT("\n"));
			return;
		}
		else
		{
				CString RemainTime = SubString(str, TEXT("��ѧʱ��\\\">"), TEXT("</span>"));
				RemainTime = RemainTime + TEXT("/") + SubString(str, TEXT("��ѧϰʱ��\\\">"), TEXT("</span>"));

				//if ((batchId - begin) % 4 == 0)//ÿ4��
				if (LastTime.CollateNoCase(RemainTime) != 0)//��ʣ��ʱ���иı�ʱˢ��
				{
					LastTime = RemainTime;
					OutPutAppend(TEXT("ʣ��ʱ�䣺") + CourseName + TEXT(" ") + SectionName + TEXT(" ") + RemainTime + TEXT("\r\n"));
					OutputDebugString(TEXT("ʣ��ʱ�䣺") + CourseName + TEXT(" ") + SectionName + TEXT(" ") + RemainTime + TEXT("\n"));
					batchId++;
					begin = batchId;
				}
		}
		for (int j = 1; j <= 15; j++)/*��ʱ15��*/
		{
			Sleep(1000);
			if (Endthread == TRUE)EndCourse();
		}
		Header = GetString(117);
		Header.Replace(TEXT("CourseID"), CourseID);
		Header.Replace(TEXT("SectionID"), SectionID);
		w.AddSendHeader(Header);
		PostData = GetString(110) + GetScriptSessionId();
		PostData.Replace(TEXT("CourseID"), CourseID);
		PostData.Replace(TEXT("SectionID"), SectionID);
		t.Format(TEXT("%d"), batchId);
		PostData.Replace(TEXT("BatchID"), t);
		str = w.Post(GetString(109), PostData);
		if (str.Find(TEXT("flag:1")) == -1)
		{
			OutputDebugString(TEXT("POSSʱ�����") + CourseName + TEXT(" ") + SectionName + TEXT("\n"));
		}
		else
		{
			OutPutAppend(TEXT("ˢ��15�룺") + CourseName + TEXT(" ") + SectionName + TEXT("\r\n"));
			OutputDebugString(TEXT("POSS��һ��ʱ�䣺") + CourseName + TEXT(" ") + SectionName + TEXT("\n"));
		}
		//OutputDebugString(TEXT("ˢʱ�䷵�ص����ݣ�\n") + str);
		batchId++;	
	}
	return ;
}

void EndCourse()
{
	OutPutAppend(TEXT("\r\nˢ����ֹͣ��"));
	PostData.Empty();
	str.Empty();	//����ֵ

	CourseAnswer.Empty();
	Quene_Section.RemoveAll();
	AllCourseID.RemoveAll();
	AllCourseName.RemoveAll();

	CButton* pass = (CButton*)(AfxGetMainWnd()->GetDlgItem(IDC_PASS));
	pass->SetWindowText(TEXT("��ˢ"));
	pass->EnableWindow(TRUE);

	AfxEndThread(0, TRUE);
}
BOOL RefreshCourse()
{
	/*ˢ�����пγ̲������ڴ�����*/
	w.AddSendHeader(GetString(108));
	PostData = GetString(107) + GetScriptSessionId();
	str = w.Post(GetString(106), PostData);
	str = HexStrToWChars(str);
	if (str.Find(TEXT("flag:0")) == -1)
	{
		return FALSE;
	}
	//m_Disp.SetWindowTextA(str);

	CString TD;//һ��
	DWORD TDpos = 0;
	DWORD index = -1;

	CString TR;//һ��
	DWORD TRpos = 0;

	CListCtrl* list = (CListCtrl*)(AfxGetMainWnd()->GetDlgItem(IDC_LIST3));

	list->DeleteAllItems();
	while (1)
	{
		TR = SubString(str, TEXT("<tr>\\r\\n                <td>"), TEXT("</tr>\\r\\n"), TRpos);//ȡ��һ��
		if (TR.IsEmpty()) break;
		TD = SubString(TR, TEXT(""), TEXT("</td>\\r\\n"), TDpos);
		index = list->InsertItem(++index, TD);//����γ̴���
		list->SetItem(index, 0, LVIF_TEXT, TD, NULL, NULL, NULL, NULL);//�γ�����
		TD = SubString(TR, TEXT("target=\\\"_blank\\\">"), TEXT("</a></td>"), TDpos);
		list->SetItem(index, 1, LVIF_TEXT, TD, NULL, NULL, NULL, NULL);//�γ�����
		TD = SubString(TR, TEXT("<td>"), TEXT("</td>\\r\\n"), TDpos);
		list->SetItem(index, 2, LVIF_TEXT, TD, NULL, NULL, NULL, NULL);//�γ̳ɼ�
		TD = SubString(TR, TEXT("<td>"), TEXT("</td>\\r\\n"), TDpos);
		list->SetItem(index, 3, LVIF_TEXT, TD, NULL, NULL, NULL, NULL);//ѧϰ����
		TD = SubString(TR, TEXT("<td>"), TEXT("</td>\\r\\n"), TDpos);
		list->SetItem(index, 4, LVIF_TEXT, TD, NULL, NULL, NULL, NULL);//״̬
		TDpos = 0;
	}
	return TRUE;
}
BOOL RefreshCourseGrade(CString courseid)
{
	str = w.Get(TEXT("http://www.attop.com/wk/index.htm?id=") + courseid);
	CString Grade = SubString(str, TEXT("class=\"markNum\">"), TEXT("</strong>"));
	//OutputDebugString(TEXT("��ȡ������") + courseid + TEXT("  ") + Grade + TEXT("\n"));
	if (Grade.IsEmpty())return FALSE;
	CListCtrl* list = (CListCtrl*)(AfxGetMainWnd()->GetDlgItem(IDC_LIST3));
	for (int i = 0; i < list->GetItemCount(); i++)
	{
		if (list->GetItemText(i, 0).Compare(courseid) == 0)
		{
			list->SetItem(i, 2, LVIF_TEXT, Grade, NULL, NULL, NULL, NULL);//�γ̳ɼ�
			return TRUE;
		}
	}

	return FALSE;
}
BOOL GetSection(CString Courseid, CStringArray &Sectionid, CStringArray &Sectionname)
{
	/*����CourseIDȡ���ÿγ��е������½�*/
	str = w.Get(TEXT("http://www.attop.com/wk/learn.htm?id=") + Courseid);

	Sectionid.RemoveAll();
	Sectionname.RemoveAll();

	DWORD pos = 0;
	DWORD pos1 = 0;
	CString zhang;
	CString jie;
	OutputDebugString(TEXT("ȡ��ĳ�γ̵������½ڣ�\n"));
	while (1)
	{
		zhang = SubString(str, TEXT("<dd>"), TEXT("</dd>"), pos);
		if (zhang.IsEmpty())
		{
			zhang = SubString(str, TEXT("<dt name=\"zj\""), TEXT("</dd>"), pos);//�������
		}
		if (zhang.IsEmpty())break;

		while (1)
		{
			jie = SubString(zhang, TEXT("<li"), TEXT("</li>"), pos1);
			if (jie.IsEmpty())break;
			CString tmp;
			tmp = SubString(zhang, TEXT("span title=\""), TEXT("</span>"));
			tmp = tmp.Left(4);//ֻҪ�ڼ���
			Sectionid.Add(SubString(jie, TEXT("id=\"j_"), TEXT("\">")));
			Sectionname.Add(tmp + TEXT(" ") + SubString(jie, TEXT("title=\""), TEXT("\"")));
			OutputDebugString(tmp + TEXT(" ") + SubString(jie, TEXT("id=\"j_"), TEXT("\">")) + TEXT(" ") + SubString(jie, TEXT("title=\""), TEXT("\"")) + TEXT("\n"));
		}
		pos1 = 0;
	}
	return TRUE;
}
CString GetSectionStatus(CString CourseID, CString SectionID, DWORD batchId)
{
	/*��ȡ�½ڵ���̨������ʱ����ɶ��٣���Ŀ��ɶ��٣�ý��������ɶ���*/
	CString t;
	CString result;
	Header = GetString(117);
	Header.Replace(TEXT("CourseID"), CourseID);
	Header.Replace(TEXT("SectionID"), SectionID);
	w.AddSendHeader(Header);
	t.Format(TEXT("%d"), batchId);
	PostData = GetString(112) + GetScriptSessionId();
	PostData.Replace(TEXT("CourseID"), CourseID);
	PostData.Replace(TEXT("SectionID"), SectionID);
	PostData.Replace(TEXT("BatchID"), t);
	result = w.Post(GetString(109), PostData);
	result = HexStrToWChars(result);
	return result;
}
CString tokenify(long long number)
{
	//CString strTime;
	//strTime.Format(_T("%lldms"), number);  //�ܺ�����  
	//OutputDebugString(strTime);
	/*���ǻ�ȡGScriptSessionId��һ�δ��룬����ԭ��ҳJS�ĵ�*/
	CString tokenbuf;
	CString charmap(TEXT("1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ*$"));
	long long  remainder = number;
	while (remainder > 0)
	{
		tokenbuf = tokenbuf + charmap.GetAt(remainder & 0x3F);
		remainder = floor(remainder / 64);
	}
	return tokenbuf;
}
CString GetScriptSessionId()
{
	/*����һ��DWRSESSIONID��������Ӧ���㷨����������ScriptSessionId
	�走�������������㷨��д����Ȼ�ܱ�Invalid Page�����������ĥ���Һö�ö��죡����
	���ң�ͨ�����ץ���ŷ��֣����ScriptSessionId��һ���Եģ�ÿ��POST������ʹ���µ�ScriptSessionId����Ȼ�ܴ����֮��ŷ��֣������
	_pageId = tokenify(new Date().getTime()) + "-" + tokenify(Math.random() * 1E16);
	ScriptSessionId=DWRSESSIONID+"/"+_pageId;
	tokenify: function(number) {
	var tokenbuf = [];
	var charmap = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ*$";
	var remainder = number;
	while (remainder > 0)
	{
	tokenbuf.push(charmap.charAt(remainder & 0x3F));
	remainder = Math.floor(remainder / 64);
	}
	return tokenbuf.join("");
	}
	*/
	CString ScriptSessionId;
	ScriptSessionId = DWRSESSIONID + TEXT("/");
	ScriptSessionId = ScriptSessionId + tokenify(getCurrentTime()) + TEXT("-") + tokenify(GetRandZeroToOne()* 1E16);
	return ScriptSessionId;
}

CString ReadAnswerFromFile(CString path)
{
	CStdioFile File;
	CString result;
	if (File.Open(path, CFile::modeNoTruncate | CFile::typeBinary | CFile::modeRead | CFile::shareDenyNone))
	{
		ULONGLONG count = File.GetLength();
		char *p = new char[count + 1];
		memset(p, 0, count + 1);
		File.Read(p, count);
		result = p;
		delete[]p;
		File.Close();
	}
	return result;
}
BOOL WriteAnswerToFile(CString path,CString Answer)
{
	CStdioFile File;
	if (File.Open(path, CFile::modeNoTruncate|CFile::modeCreate | CFile::typeBinary | CFile::modeReadWrite | CFile::shareDenyNone))
	{
		File.SeekToEnd();
		char *p = UnicodeToANSI(Answer);
		File.Write(p, strlen(p));
		delete[]p;
		File.Close();
		return TRUE;
	}
	return FALSE;
}
BOOL RefreshAnswerFile()
{
	/*����վ�����ش�,�����س����ܷ��������*/
	//CString LocalAnswer;
	//CString LocaVersion;
	//CString OnlineVersion;
	//CString OnlineAnswer;
	//CString path;
	//CString LackCourse;
	//CString tmp;

	//HttpClient MyWeb;
	//OutPut(TEXT("���ڼ���ļ�����...\r\n"));
	//CreateDirectory(TEXT("TK"),NULL);
	//str = MyWeb.Get(GetString(14));
	//OnlineVersion = SubString(str, TEXT("<Version>"), TEXT("</Version>"));
	//if (OnlineVersion.IsEmpty())return TRUE;
	//if (OnlineVersion.CompareNoCase(GetString(1001))!=0)
	//{
	//	AfxMessageBox(SubString(str, TEXT("<VersionContent>"), TEXT("</VersionContent>")),MB_ICONINFORMATION);
	//	return FALSE;
	//}
	//if (!SubString(str, TEXT("<Tips>"), TEXT("</Tips>")).IsEmpty())
	//{
	//	AfxMessageBox(SubString(str, TEXT("<Tips>"), TEXT("</Tips>")), MB_ICONINFORMATION);
	//}
	//for (int i = 0; i < AllCourseID.GetCount(); i++)
	//{
	//	path = TEXT("TK\\") + AllCourseID.GetAt(i) + TEXT(".txt");
	//	LocalAnswer = ReadAnswerFromFile(path);
	//	LocaVersion = SubString(LocalAnswer, TEXT("<Version>"), TEXT("</Version>"));
	//	OnlineVersion = SubString(str, TEXT("<") + AllCourseID.GetAt(i) + TEXT(">"), TEXT("</") + AllCourseID.GetAt(i) + TEXT(">"));
	//	if (OnlineVersion.IsEmpty())continue;
	//	if (OnlineVersion.CompareNoCase(LocaVersion) == 0)continue;//�汾��ͬ
	//	CString url = GetString(15);
	//	url.Replace(TEXT("CourseID"), AllCourseID.GetAt(i));
	//	OnlineAnswer = MyWeb.Get(url);
	//	if (OnlineAnswer.IsEmpty())
	//	{
	//		LackCourse = LackCourse + AllCourseID.GetAt(i) + TEXT(" ") + AllCourseName.GetAt(i) + TEXT("\n");
	//		continue;
	//	}
	//	DeleteFile(path);
	//	WriteAnswerToFile(path, OnlineAnswer);
	//	OutPutAppend(TEXT("��ȡ������ļ�����...")+ AllCourseID.GetAt(i)+TEXT(" ")+ AllCourseName.GetAt(i)+ TEXT("\r\n"));
	//}
	//OutPutAppend(TEXT("������ϣ�\r\n"));
	//if (!LackCourse.IsEmpty())AfxMessageBox(LackCourse + TEXT("\n��Щ�γ�ȱ������ļ�������ˢ�⣡"), MB_ICONWARNING);
	return TRUE;
}

CString OrderAnswer(CString Answer,DWORD n)
{
	/*��  2,3,1, �����Ĵ������ 1,2,3 */
	int *a= new int[n];
	DWORD iPos = 0;
	CString Num;
	if (Answer.IsEmpty())return TEXT("");
	if (Answer.Left(1).Compare(TEXT(",")) != 0)Answer = TEXT(",") + Answer;//�����ڱ�
	if (Answer.Right(1).Compare(TEXT(",")) != 0)Answer = Answer + TEXT(",");//�����ڱ�
	for (int k = 0; k < n; k++)
	{
		Num = SubString(Answer, TEXT(","), TEXT(","), iPos);
		a[k] = _ttoi(Num);
	}
	int i, j, temp;
	for (j = 0; j < n - 1; j++)
		for (i = 0; i < n - 1 - j; i++)
		{
			if (a[i] > a[i + 1])
			{
				temp = a[i];
				a[i] = a[i + 1];
				a[i + 1] = temp;
			}
		}
	Answer.Empty();
	CString t;
	for (int k = 0; k < n; k++)
	{
		t.Format(TEXT("%d"), a[k]);
		Answer = Answer + t+TEXT(",");
	}
	if (Answer.Right(1) = TEXT(","))
		Answer = Answer.Left(Answer.GetLength() - 1);//ȥ��ĩβ��,
	delete[]a;
	return Answer;
}

void CTestDlg::OnLvnItemchangedList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}
