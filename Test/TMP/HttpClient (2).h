/*
���ﶨ����һ��Web�࣬����get����post��ҳ
Visit����ʹ��wininet��ȡ��ҳ���ݣ����Զ�ת�룬����ר��
*/
#ifndef WEBCLIENT_H
#define WEBCLIENT_H

#include <afx.h>
#include <Wininet.h>  

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
	CString strServerName;//����
	INTERNET_PORT dwPort;//�˿�

	HttpClient(CString strServerName, INTERNET_PORT dwPort);//����ͨ��getServerName��getPort��ȡ http:80 https:443
	~HttpClient();

	CString Get(HttpInfo &httpinfo);
	CString Post(HttpInfo &httpinfo);
	
	static DWORD getPort(CString url);//ȡ�˿�
	static CString getServerName(CString url, bool isPort);//ȡ����,��ѡ���Ƿ����˿�
	static CString GetPageName(CString url);//ȡҳ���ַ
	static CString UpdateCookie(CString New, CString Old);//��A��B����ͬ��Cookie�ϲ�
	static CString GetCookie(CString Headers);
private:
	HINTERNET hOpen;
	HINTERNET hConnect;
	HINTERNET hRequest;
	CString Visit(HttpInfo &httpinfo, CString Verb);//ɵ��ʽ�Զ�ת���ȡ��ҳ����
};
#endif