#include "stdafx.h"
#include "Func.h"
#include "Code.h"

CString GetRandWord(DWORD n)//��ȡn�������ĸ
{
	//srand((unsigned)time(NULL));
	int i;
	CString str;
	char c;
	int t;
	for (i = 0; i < n; i++)
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
DWORD GetRand(DWORD m, DWORD n)//m <= rand() % (n - m + 1) + m <= n
{
	//srand((unsigned)time(NULL));
	return rand() % (n - m + 1) + m;
}
DOUBLE GetRandZeroToOne()
{
	//srand((unsigned)time(NULL));
	return  (rand() % 10) / (float)10;//����0-1����������
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
	/*
	int c;
	c = LoadString(NULL, id, ss.GetBuffer(1024000), 1024000);
	ss.ReleaseBuffer();
	ss.Left(c);
	*/
	ss.LoadString(id);
	return ss;
}
CString SubString(CString s, CString head, CString tail)
{
	int iPos = s.Find(head);
	if (iPos == -1)return TEXT("");
	iPos += head.GetLength();
	int iPos1 = s.Find(tail, iPos);
	if (iPos1 == -1)return TEXT("");
	return s.Mid(iPos, iPos1 - iPos);
}
CString SubString(CString s, CString head, CString tail,DWORD &t)
{
	int iPos = s.Find(head, t);
	if (iPos == -1)return TEXT("");
	iPos += head.GetLength();
	int iPos1 = s.Find(tail, iPos);
	if (iPos1 == -1)return TEXT("");
	t = iPos1;
	return s.Mid(iPos, iPos1 - iPos);
}
long long getCurrentTime()
{
	long long time_last;
	time_last = time(NULL);     //������  
	struct timeb t1;
	ftime(&t1);
	//CString strTime;
	//strTime.Format(_T("%lldms"), t1.time * 1000 + t1.millitm);  //�ܺ�����  
	//OutputDebugString(strTime);
	return t1.time * 1000 + t1.millitm;
}
BOOL TestUserName(CString User, int n) {
	/*�򵥵�����ÿ̨�����������n���˺ţ���ֹ���������
	����ʹ�÷����棬����ʹ�÷��ؼ�*/
	HKEY  hKey;
	CString KeyName;
	CStringArray ret;


	//��ע���
	if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("SOFTWARE\\SKSoftware"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL) != ERROR_SUCCESS) {
		return TRUE;
	}

	//��ѯ��ǰ�����Ѿ��ù����û�
	CString Value;
	DWORD dwSize;
	BYTE *data;
	DWORD lpType = REG_SZ;
	for (int i = 0; i < n; i++) {
		KeyName.Format(TEXT("%d"), i);

		data = new BYTE[256];
		dwSize = 256;
		
		//��ȡ��ֵ����
		if (RegQueryValueEx(hKey, KeyName, NULL, &lpType, data, &dwSize) != ERROR_SUCCESS) {
			continue;
		}
		
		//��ȡ��һ��ʹ�ù����û���
		Value.Format(TEXT("%s"), data);
		ret.Add(Value);

		delete[]data;
	}

	//RegCloseKey(hKey);

	for (int i = 0; i < ret.GetSize(); i++) {
		//����Ѿ���ע���,ֱ�ӷ��سɹ�
		if (User == ret[i]) {
			RegCloseKey(hKey);
			return TRUE;
		}
		//ע�����û�У�����һ�����û�
		if (i == ret.GetSize() - 1) {

			if (i == n - 1) {
				//�������ܹ�ʹ�õ�����û���
				RegCloseKey(hKey);
				return FALSE;
			}
			else {
				//���ܽ���һ�����û�
				break;
			}

		}
	}

	//��ע�����д�����û�����Ϣ
	KeyName.Format(TEXT("%d"), ret.GetSize());

	RegSetValueEx(hKey, KeyName, NULL, REG_SZ,(BYTE*)User.GetBuffer(0), User.GetLength()*2+2);
	User.ReleaseBuffer();

	RegCloseKey(hKey);
	return TRUE;
}