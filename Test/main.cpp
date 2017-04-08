#include "Web.h"
#include "code.h"
#include "stdafx.h"
#include<windows.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD GetRand(DWORD m, DWORD n);
CString GetString(DWORD id);
CString GetRandWord(DWORD n);
CString GetRandStr(DWORD m, DWORD n);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("MyWindows");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("���������Ҫ�� Windows NT ����ִ�У�"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName,
		TEXT("Windows"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		860,
		600,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	static HWND hEditAdd;
	static HWND hEditMessage;
	//static HWND hButton;

	switch (message)
	{
	case WM_CREATE:
		hEditAdd =CreateWindow(TEXT("EDIT"), TEXT("http://www.attop.com"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER ,
			10, 10, 650, 30, hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 0);

		hEditMessage = CreateWindow(TEXT("EDIT"), TEXT(""), WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE|WS_VSCROLL ,
			10, 50, 800, 500, hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 0);

		CreateWindow(TEXT("BUTTON"), TEXT("��ת"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			670, 10, 80, 30, hwnd, (HMENU)10001, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 0);
		CreateWindow(TEXT("BUTTON"), TEXT("test"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			760, 10, 80, 30, hwnd, (HMENU)10002, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 0);
		return 0;
		/*case WM_PAINT:
		
		hdc = BeginPaint(hwnd, &ps);
		
		GetClientRect(hwnd, &rect);
		DrawText(hdc, TEXT("Hello,world!"), -1, &rect,
		DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		
		TextOut(hdc, 200, 300, TEXT("hello world!"), strlen("hello world!"));
		EndPaint(hwnd, &ps);
		
		return 0;*/

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_COMMAND: 
		if (LOWORD(wParam) == 10001 && HIWORD(wParam) == BN_CLICKED) {//ת����ť
			int count= GetWindowTextLength(hEditAdd) + 2;
			TCHAR *text=new TCHAR[count];
			GetWindowText(hEditAdd,text, count);

			Web w;
			CString cookie;
			w.Open(text);
			CString str=w.Visit(text, TEXT("GET"), TEXT(""), cookie,TEXT(""));

			SetWindowText(hEditMessage, str);
			//Edit_GetText(hEdit, g_szMainText, 128);
			//PostMessage(hwnd, WM_CLOSE, 0, 0);
			delete[]text;
		}
		if (LOWORD(wParam) == 10002 && HIWORD(wParam) == BN_CLICKED) {//���԰�ť
			Web w;
			CString cookie;
			CString info;//�ύ��Ϣ
			CString str;//����ֵ
			CString strHeaders;//Э��ͷ
			CString url;
			CString SESSIONID;
			CString scriptSessionId;
			if(!w.Open(GetString(10)))//����
			{
				OutputDebugString("����ʧ�ܣ�");
				return 0;
			}

			/*��ʼģ�¸մ���ҳ*/
			srand((unsigned)time(NULL));
			SESSIONID = TEXT("AAFE06E597DA8BCE86043B3099E")+ GetRandStr(10000, 99999);//�������JSESSIONID
			OutputDebugString(TEXT("���ɵ�JSESSIONID��\n") + SESSIONID + "\n");
			url = GetString(101);
			info = GetString(102);
			cookie = TEXT("JSESSIONID=") + SESSIONID;
			strHeaders = TEXT("Referer: ") + GetString(10);
			//strHeaders = TEXT("Referer: ");
			str = w.Visit(url, TEXT("POST"), info, cookie, strHeaders);
			OutputDebugString(TEXT("ģ�����ҳ���ص����ݣ�\n") + str + "\n");
			SESSIONID = SubString(str,TEXT("r.handleCallback(\"0\",\"0\",\""),TEXT("\");"));/*���DWRSESSIONID*/
			OutputDebugString(TEXT("��õ�DWRSESSIONID��\n") + SESSIONID + "\n");
			if (SESSIONID.IsEmpty())
			{
				OutputDebugString ("��ȡDWRSESSIONID����\n");
				return 0;
			}
			else
			{
				cookie = TEXT("DWRSESSIONID=") + SESSIONID + TEXT(";") + cookie;
			}


			/*��������ʼ��¼*/
			url = GetString(103);
			scriptSessionId = SESSIONID + TEXT("/") + GetRandWord(7) + TEXT("-") + GetRandWord(9);
			OutputDebugString(TEXT("���ɵ�scriptSessionId��\n") + scriptSessionId + "\n");
			info = GetString(104) + scriptSessionId; //scriptSessionId����ҳ�����������
			/*dwr.engine._pageId = dwr.engine.util.tokenify(new Date().getTime()) + "-" + dwr.engine.util.tokenify(Math.random() * 1E16);*/
			cookie = cookie + TEXT(";rand=") + GetRandStr(1000, 9999);
			strHeaders = GetString(105);
			str = w.Visit(url, TEXT("POST"), info, cookie, strHeaders);
			OutputDebugString(TEXT("��½�󷵻ص����ݣ�\n") + str + "\n");
			if (str.Find(TEXT("flag:1")) == -1)
			{
				OutputDebugString("��¼ʧ�ܣ�����û����������Ƿ���ȷ��\n");
				return 0;
			}
			OutputDebugString(TEXT("��¼���cookie��\n") + cookie + "\n");

			/*������ȡ������Щ�γ�*/
			url = GetString(106);
			info = GetString(107) + scriptSessionId;
			//û�������ɵ�cookie
			strHeaders = GetString(108) + GetString(11);
			/*���11��ŵ��ַ��������⣬���ӱش���������о������죬��JAVAд��ץ����֪��Ҫ���������*/
			str = w.Visit(url, TEXT("POST"), info, cookie, strHeaders);
			//OutputDebugString(TEXT("��ѯ����Щ�γ̷��ص����ݣ�\n") + str + "\n");
			OutputDebugString(TEXT("��ѯ����Щ�γ̷��ص����ݣ�\n") + HexStrToWChars(str) );
			if (str.Find(TEXT("flag:0")) == -1)
			{
				OutputDebugString("��ѯ����Щ�γ�ʧ�ܣ�\n");
				return 0;
			}


			OutputDebugString("���");
			return 0;


			info =
TEXT("callCount=1\nwindowName=\nc0-scriptName=zsClass\nc0-methodName=coreAjax\nc0-id=0\nc0-param0=string:login\nc0-e1=string:18375666059\nc0-e2=string:Mxiaoding\nc0-e3=string: \nc0-e4=number:2\nc0-param1=Object_Object:{username:reference:c0-e1,password:reference:c0-e2,rand:reference:c0-e3,autoflag:reference:c0-e4}\nc0-param2=string:doLogin\nbatchId=1\ninstanceId=0\npage=%2Flogin_pop.htm\nscriptSessionId=1NnElFf$uFDoV9OWj20lAElI4bl/Ble76bl-Zew7izfec\n");
			str = w.Visit(TEXT("http://www.attop.com/js/ajax/call/plaincall/zsClass.coreAjax.dwr"),TEXT("POST"), info, cookie, TEXT(""));
			//SetWindowText(hEditMessage, str);
			OutputDebugString(str+"\n");

			info =
TEXT("callCount=1\nwindowName=\nc0-scriptName=zsClass\nc0-methodName=commonAjax\nc0-id=0\nc0-param0=string:getAjaxList\nc0-e1=string: \nc0-e2=string:study.htm\nc0-e3=number:1\nc0-e4=string:showajaxinfo\nc0-param1=Object_Object:{param:reference:c0-e1, pagename:reference:c0-e2, currentpage:reference:c0-e3, showmsg:reference:c0-e4}\nc0-param2=string:doGetAjaxList\nbatchId=2\ninstanceId=0\npage=%2Fuser%2Fstudy.htm\n");
			str = w.Visit(TEXT("http://www.attop.com/js/ajax/call/plaincall/zsClass.commonAjax.dwr"), TEXT("POST"), info, cookie, TEXT(""));
			SetWindowText(hEditMessage, str);
			OutputDebugString(cookie + TEXT("\n"));
			OutputDebugString(str + TEXT("\n"));





			/*if (w.Open(TEXT("http://www.023001.com/cqdx/cq10000.asp"))) {
				CString str = w.Visit(TEXT("http://www.023001.com/cqdx/cq10000.asp"),
					TEXT("POST"), TEXT("no=130650"), cookie, TEXT(""));
				SetWindowText(hEditMessage, str);


				OutputDebugString(str);
			}*/
		}




		return 0;

	}


	return DefWindowProc(hwnd, message, wParam, lParam);
}
CString GetRandWord(DWORD n)//��ȡn�������ĸ
{
	int i;
	CString str;
	char c;
	int t;
	for (i = 0; i < n; ++i) 
	{
		t = rand() % 3;
		if (t>1)
		{
			t = 'a';
		}
		else
		{
			t = 'A';
		}
		c = t + rand() % 26;
		str = str + c;
	}
	return str;
}
DWORD GetRand(DWORD m,DWORD n)//m <= rand() % (n - m + 1) + m <= n
{
	return rand() % (n - m + 1) + m;
}
CString GetRandStr(DWORD m, DWORD n)//m <= rand() % (n - m + 1) + m <= n
{
	CString str;
	str.Format(TEXT("%d"), GetRand(m, n));
	return str;
}
CString GetString(DWORD id)//����Դ����ȡ�ַ���
{
	CString ss;
	int c;
	c = LoadString(NULL, id, ss.GetBuffer(1024000), 1024000);
	ss.ReleaseBuffer();
	ss.Left(c);
	return ss;
}