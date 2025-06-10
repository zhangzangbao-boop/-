#pragma once
#pragma once
#ifndef _TOOL_H_
#define _TOOL_H_
int count(char s[]);//数字符串长度
//void timetostr(time_t a, char* s);

void transTime(time_t a, char* s);//时间转化年月日时分
time_t strToTime(char* pTime);//年月日时分转化时间


void color(short x);


//Card* Create(Card* pCard);//链表建立
//Card* Insert(Card* pHead, Card* pNew);//链表插入
//Card* Delete(Card* pHead, unsigned n);//链表删除
#endif