#pragma once
#pragma once
#include<time.h>
#ifndef _MODEL_H_
#define _MODEL_H_
typedef struct Data			//添加卡信息结构体
{
	char aNum[20];			//卡号
	char aCode[10];			//密码
	int nStatus;			//卡状态（0-未上机；1-正在上机；2-已注销）
	time_t tStart;			//开卡时间
	time_t tEnd;			//截止时间
	time_t tLastuse;		//上次使用时间
	time_t undertime;       //潜在用户发掘时间
	double Totaluse;		//总使用金额
	double Balance;			//余额
	int Usecnt;				//使用次数
	int vipgress;           //用户vip等级
	double everymoney[100];  // 每次上机消费
	char everytime1[100][20];  //记录上机时间
	char everytime2[100][20];   //记录下机时间
	char everytime3[100][20];  //记录上机时间(仅年月日)
	char everytime4[100][20];   //记录下机时间(仅年月日)
	int lucy;
}Card;

typedef struct CardNode
{
	Card data;
	struct CardNode* next;
}CardNode, * lpCardNode;

typedef struct Billing
{
	char aNum[20];			//卡号
	time_t tStart;			//上机时间
	time_t tEnd;			//下机时间
	double fAmount;			//消费金额
	double fBalance;
	int nStatus;			//消费状态（0-未结算，1-已结算）
	int nDel;				//删除状态（0-未删除，1-已删除）
	int vipgress;           //用户vip等级
}Billing;

typedef struct BillingNode
{
	Billing data;
	struct BillingNode* next;
}BillingNode, * lpBillingNode;
#endif