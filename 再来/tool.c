#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>
#include"global.h"
#include<windows.h>
#include"card.file.h"
int count(char s[])		//字符串长度判断
{
	int i = 0;
	while (s[i] != '\0')
		i++;
	return i;
}
void transTime(time_t a, char* s)		//将时间转化为年月日时分
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
	tm1.tm_year -= 1900;  // 年份为从1900年开始
	tm1.tm_mon -= 1;      // 月份为0~11
	tm1.tm_sec = 0;
	tm1.tm_isdst = -1;
	time1 = mktime(&tm1);
	return time1;
}

void color(short x)	//自定义函根据参数改变颜色 
{
	if (x >= 0 && x <= 15)//参数在0-15的范围颜色
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), x);	//只有一个参数，改变字体颜色 
	else//默认的颜色白色
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

