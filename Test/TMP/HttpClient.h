/*
һ��Http�ͻ���
���ò�ͬ��session�����Զ�ͬһ����վͬʱ��¼��ͬ���˺ţ���������ִ��ŵ�����
*/

#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H
#include <afxinet.h>

typedef struct HttpInfo
{
	CString url;//����
	CString Header;//Э��ͷ
	CString PostData;//POSTר�ã��ύ��Ϣ��ʵ����CookiesҲ����뵽���
	CString Cookie;//�ύCookies,���������ݱ���ʱ���Զ��ش����ص�Cookies
}HttpInfo;

class HttpClient
{
public:
	CString Post(HttpInfo &httpinfo);
	CString Get(HttpInfo &httpinfo);
	static CString UpdateCookie(CString New, CString Old);
	static CString GetCookie(CString Headers);
private:
	CInternetSession sess;//ÿ��session�Ķ�����־�����ڶ�ͬһ��վ�����������
	CString Visit(HttpInfo &httpinfo, CString Verb);
};
#endif 