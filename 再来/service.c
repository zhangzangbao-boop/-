#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include"global.h"
#include"model.h"
#include"tool.h"
#include"card.file.h"
#include"card.service.h"
#include"billing.file.h"
#include"billing.service.h"
#include"service.h"
#include"Windows.h"
void releaseList()
{
	releaseBillingList();
	releaseCardList();
}
Card* doLogonInfo(char* aNum, char* aCode)
{
	Card* pCard = NULL;
	pCard = doLogon(aNum, aCode);
	if (pCard == NULL)
	{
		return NULL;
	}
	Billing billing;
	strcpy(billing.aNum, pCard->aNum);
	billing.tStart = time(NULL);
	billing.fAmount = 0;
	billing.tEnd = time(NULL);
	billing.nStatus = 0;
	billing.fBalance = pCard->Balance;
	billing.nDel = 0;
	billing.vipgress = pCard->vipgress;
	if (TRUE == saveBilling(&billing, BILLINGPATH))
	{
		return pCard;
	}
	return NULL;
}
Card* doSettleInfo(char* aNum, char* aCode, double* fAmount)
{
	//卡消费信息与基本信息在文件中的索引号
	int nIndexb = 0;
	int nIndexc = 0;
	Card* pCard = NULL;
	//在获取卡信息同时获取其位置索引号
	pCard = doSettle(aNum, aCode, &nIndexc);
	Billing* pBilling = NULL;
	pBilling = checkBilling(aNum, &nIndexb);
	if (pBilling != NULL)
	{
		//组装卡消费信息
		pBilling->tEnd = time(0);
		pBilling->fAmount = getAmount(pBilling->tStart,pCard->vipgress);
		*fAmount = pBilling->fAmount;
		pBilling->nStatus = 1;
		pBilling->fBalance -= pBilling->fAmount;
		//组装卡基本信息
		pCard->nStatus = 0;
		timetostr(pCard->tLastuse, pCard->everytime3[pCard->Usecnt - 1]);
		transTime(pCard->tLastuse, pCard->everytime1[pCard->Usecnt-1]);
		pCard->tLastuse = time(NULL);
		timetostr(pCard->tLastuse, pCard->everytime4[pCard->Usecnt - 1]);
		transTime(pCard->tLastuse, pCard->everytime2[pCard->Usecnt-1]);
		pCard->Totaluse += pBilling->fAmount;
		pCard->Balance = pBilling->fBalance;
		pCard->everymoney[pCard->Usecnt-1] = pBilling->fAmount;
		if (pCard->vipgress == 1 && pBilling->fAmount >= 10)
		{
			pCard->lucy++;
		}
		if (pCard->vipgress == 0 && pBilling->fAmount >= 20)
		{
			pCard->lucy++;
		}
		//将修改后的信息保存到文件中
		if ((TRUE == updateBilling(pBilling, BILLINGPATH, nIndexb)) && (TRUE == updateCard(pCard, CARDPATH, nIndexc)))
		{
			return pCard;
		}
	}
	return NULL;
}

void queryInfo()
{
	char str[20] = { 0 };
	Card* pCard = NULL;
	printf("请输入查询卡卡号<长度为1~18>:");
	scanf_s("%s", str, 20);
	int flag1 = 0;
	pCard = quaryCard(str);
	if (pCard != NULL)
	{
		flag1 = 1;
		char Lastuse[20];
		transTime(pCard->tLastuse, Lastuse);
		printf("卡号\t余额\t\t总使用金额\t使用次数\t上一次使用时间\t\t\t卡状态\t卡vip状态\t抽奖次数\n");
		if (pCard->vipgress == 1)
			color(12);
		printf("%s\t%.2lf\t\t%.2lf\t\t%d\t\t%s\t\t%d\t%d\t\t%d\n", pCard->aNum, pCard->Balance, pCard->Totaluse, pCard->Usecnt, Lastuse, pCard->nStatus,pCard->vipgress,pCard->lucy);
		color(7);
	}
	if (flag1 == 0)
	{
		printf("未找到该卡信息！\n");
	}
}
double getAmount(time_t tStart, int vip)
{
	double discount = 1.0;
	time_t tEnd = time(NULL);
	int sec = 0;
	sec = tEnd - tStart;
	double fee = 0;
	if (vip == 1)
		discount = 0.7;
	fee = ((ceil((sec / 60.0) / UNIT)) * CHARGE) * discount;
	return fee;
}
