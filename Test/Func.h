/*
���ﶨ�����һЩ�����Ժ���
*/

#ifndef FUNC_H
#define FUNC_H

#include <afx.h>
#include<time.h>
#include<sys/timeb.h>

CString SubString(CString s, CString head, CString tail);		//��ȡ�ַ���
CString SubString(CString s, CString head, CString tail,DWORD &t);	//��T��ʼ��head��tailλ�÷��ص�t�����ڳ���Ѱ���ַ���
DWORD GetRand(DWORD m, DWORD n);	//ȡ�����
DOUBLE GetRandZeroToOne();		//0��1�������
CString GetString(DWORD id);		//����Դ�ļ���ȡ�ַ���
CString GetRandWord(DWORD n);		//ȡ����ΪN�������ĸ�ַ���
CString GetRandStr(DWORD m, DWORD n);	//ȡ�������ֻ�����Ƿ���ֵ���ַ�������
long long getCurrentTime();		//��ȡ���ڵ�1970��1��1�յĺ�����
BOOL TestUserName(CString User, int n);	//����һ̨��������¼n���û�������������򷵻ؼ�
#endif