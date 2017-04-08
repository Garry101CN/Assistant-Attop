#include "stdafx.h"
#include <afxinet.h> //������MFC CInternetSession���
#include "HttpClient.h"
#include "Code.h"
#include "Func.h"
#define MAXSIZE 102400
CString HttpClient::Post(HttpInfo &httpinfo)
{
	return Visit(httpinfo,TEXT("POST"));
}
CString HttpClient::Get(HttpInfo &httpinfo)
{
	return Visit(httpinfo, TEXT("GET"));
}

CString HttpClient::Visit(HttpInfo &httpinfo,CString Verb)
{
	DWORD dwServiceType;
	unsigned short nPort;
	CString strServer, strObject,strHeader(httpinfo.Header);
	CString Content;

	try
	{
		/*�ֽ���ַ*/
		if (!AfxParseURL(httpinfo.url, dwServiceType, strServer, strObject, nPort))
		{
			OutputDebugString(httpinfo.url + TEXT("������Ч�������ַ��\n"));
			return TEXT("");
		}

		/*���ӷ�����*/
		CHttpFile* pFile;
		CHttpConnection *pServer =sess.GetHttpConnection(strServer, nPort);
		if (pServer == NULL)
		{
			OutputDebugString(TEXT("���ӷ�����ʧ�ܣ�\n"));
			return TEXT("");
		}

		if (Verb.Compare(TEXT("POST")) == 0)
		{
			pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject, NULL, 1, NULL, NULL, INTERNET_FLAG_EXISTING_CONNECT);
		}
		else
		{
			pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObject, NULL, 1, NULL, NULL, INTERNET_FLAG_EXISTING_CONNECT);
		}
		if (pFile == NULL)
		{
			OutputDebugString(TEXT("�Ҳ��������ַ��\n"));
			return TEXT("");
		}

		/*��ʼ���챨��*/
		if (strHeader.Find(TEXT("Referer:")) == -1)
			strHeader = strHeader + TEXT("\r\n") + TEXT("Referer:") + httpinfo.url;
		if (strHeader.Find(TEXT("Accept:")) == -1)
			strHeader = strHeader + TEXT("\r\n") + TEXT("Accept: */*");
		if (strHeader.Find(TEXT("Accept-Language:")) == -1)
			strHeader = strHeader + TEXT("\r\n") + TEXT("Accept-Language: zh-cn");
		if (strHeader.Find(TEXT("Content-Type:")) == -1)
			strHeader = strHeader + TEXT("\r\n") + TEXT("Content-Type: application/x-www-form-urlencoded");
		if (!httpinfo.Cookie.IsEmpty())
			strHeader = strHeader + TEXT("\r\n") + TEXT("Cookie: ") + httpinfo.Cookie;
		if (strHeader.Find(TEXT("Content-Length:")) == -1&& Verb.Compare(TEXT("POST")) == 0)
		{
			CString tmp;
			tmp.Format(TEXT("%d"), httpinfo.PostData.GetLength());
			strHeader = strHeader + TEXT("\r\n") + TEXT("Content-Length: ") + tmp;
		}
		strHeader = strHeader + TEXT("\r\n\r\n");//���Ľ���

		if (Verb.Compare(TEXT("POST")) == 0)
		{
			char *pszData = NULL;
			pszData = UnicodeToANSI(httpinfo.PostData);
			pFile->SendRequest(strHeader, strHeader.GetLength(), pszData, strlen(pszData));
			delete[]pszData;
		}
		else
		{
			pFile->SendRequest(strHeader, strHeader.GetLength(), NULL, 0);
		}

		/*������ҳ����*/
		CString strSentence;
		DWORD dwStatus;
		DWORD dwBuffLen = sizeof(dwStatus);
		BOOL bSuccess = pFile->QueryInfo(HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwBuffLen);
		if (bSuccess && dwStatus >= 200 && dwStatus<300)
		{
			char buffer[2048];
			char *sReadBuffer = new char[MAXSIZE];
			
			char *temp;
			DWORD dwReadSize = 0;//�Ѿ�������ܳ���
			DWORD dwMax = MAXSIZE;//�ܹ��洢��󳤶�
			DWORD nReadCount = 0;//ÿ�ζ���ĳ���

			memset(buffer, 0, 2048);
			while ((nReadCount = pFile->Read(buffer, 2048)) > 0)
			{
				if (nReadCount == 0)break;
				if (nReadCount + dwReadSize > dwMax)//�ڴ�װ������
				{
					temp = sReadBuffer;
					sReadBuffer = new char[MAXSIZE + dwMax];//׷���ڴ�
					memcpy(sReadBuffer, temp, dwReadSize);
					delete[]temp;
					dwMax = MAXSIZE + dwMax;
				}
				memcpy(&sReadBuffer[dwReadSize], buffer, nReadCount);
				dwReadSize += nReadCount;
				memset(buffer, 0, 2048);
			}
			Content = sReadBuffer;
			Content = Content.Left(dwReadSize);
			/*��utf-8ת����Ansi*/
			int iPos = Content.Find(TEXT("utf-8"));
			if (iPos != -1 && iPos < 500 || httpinfo.Header.Find(TEXT("utf-8")) != -1)
			{
				char *p = UTF8ToANSI(sReadBuffer);
				Content = p;
				delete[]p;
				Content = Content.Left(dwReadSize);
			}
			delete[]sReadBuffer;
		}
		else
		{
			OutputDebugString(TEXT("��ȡ��վ����ʧ�ܣ�\n"));
			pFile->Close();
			pServer->Close();
			//sess.Close();
			return TEXT("");
		}

		
		/*�ӷ��ص�����ͷ����ȡCookie����ԭ����Cookie�ϲ�e*/
		CString SetCookie;
		DWORD iCount = 0;
		if (pFile->QueryInfo(HTTP_QUERY_SET_COOKIE, SetCookie, &iCount))
		{
			httpinfo.Cookie = UpdateCookie(SetCookie, httpinfo.Cookie);//����Cookie
		}
		pFile->Close();
		pServer->Close();
		//sess.Close();
		return Content;
	}
	catch (...)
	{
		OutputDebugString(TEXT("�������postʧ�ܣ�\n"));
		return TEXT("");
	}

}

CString HttpClient::GetCookie(CString Headers)
{
	CString Row;
	CString Cookie;
	DWORD pos = 0;
	DWORD tmp;

	Headers = Headers + TEXT("\r\n");
	while (1)
	{
		if (pos != 0)pos += 2;
		tmp = pos;
		pos = Headers.Find(TEXT("\r\n"), pos);
		if (pos == -1 || pos == 0 || pos == Headers.GetLength() - 2)//�մ�����Ѱ�ҵ����һ�����з�
		{
			break;
		}
		else
		{
			Row = Headers.Mid(tmp, pos - tmp);
		}
		if (Row.Find(TEXT("Set-Cookie:")) != -1)
		{
			Cookie = Cookie + SubString(Row, TEXT("Set-Cookie:"), TEXT(";")) + TEXT("; ");
		}
	}
	Cookie = Cookie.Left(Cookie.GetLength() - 2);
	return Cookie;
}
CString HttpClient::UpdateCookie(CString New, CString Old)
{
	int pos = 0;
	int tmp;
	CString Row;
	CString Row1;
	New.Remove(TEXT(' '));
	Old.Remove(TEXT(' '));
	if (New.IsEmpty())return Old;
	if (Old.IsEmpty())return New;
	while (1)
	{
		if (pos == -1)break;
		if (pos != 0)pos++;
		tmp = pos;
		pos = Old.Find(TEXT(";"), pos);
		if (pos == -1)
		{
			Row = Old.Right(Old.GetLength() - tmp);
		}
		else
		{
			Row = Old.Mid(tmp, pos - tmp);
		}

		int pos1 = 0;
		while (1)//�ж��ϵ�cookie���µ�cookie���Ƿ���ڣ�����������ӽ�ȥ�������������ϵ�
		{
			if (pos1 == -1)
			{
				New = New + TEXT("; ") + Row;
				break;//���µ�����û�оɵģ��ͰѾɵ���ӵ��µ�����
			}
			if (pos1 != 0)pos1++;
			tmp = pos1;
			pos1 = New.Find(TEXT(";"), pos1);
			if (pos1 == -1)
			{
				Row1 = New.Right(New.GetLength() - tmp);
			}
			else
			{
				Row1 = New.Mid(tmp, pos1 - tmp);
			}
			if (New.Right(8) == TEXT("=deleted"))break;
			int i = Row1.Find(TEXT("="));
			int j = Row.Find(TEXT("="));
			if (Row1.Left(i) == Row.Left(j))break;//���ھ�����

		}
	}
	return New;
}