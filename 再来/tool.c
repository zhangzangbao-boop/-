#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>
#include"global.h"
#include<windows.h>
#include"card.file.h"
int count(char s[])		//�ַ��������ж�
{
	int i = 0;
	while (s[i] != '\0')
		i++;
	return i;
}
void transTime(time_t a, char* s)		//��ʱ��ת��Ϊ������ʱ��
{
	struct tm* timeinfo;
	timeinfo = localtime(&a);
	strftime(s, 20, "%Y-%m-%d %H:%M", timeinfo);
}

time_t strToTime(char* pTime)
{
	struct tm tm1;
	time_t time1;
	sscanf(pTime, "%d-%d-%d %d:%d", &tm1.tm_year, &tm1.tm_mon,
		&tm1.tm_mday, &tm1.tm_hour, &tm1.tm_min);
	tm1.tm_year -= 1900;  // ���Ϊ��1900�꿪ʼ
	tm1.tm_mon -= 1;      // �·�Ϊ0~11
	tm1.tm_sec = 0;
	tm1.tm_isdst = -1;
	time1 = mktime(&tm1);
	return time1;
}

void color(short x)	//�Զ��庯���ݲ����ı���ɫ 
{
	if (x >= 0 && x <= 15)//������0-15�ķ�Χ��ɫ
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), x);	//ֻ��һ���������ı�������ɫ 
	else//Ĭ�ϵ���ɫ��ɫ
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

