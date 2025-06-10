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
	pCard = doLogon(aNum, aCode);  // 调用登录验证函数
	if (pCard == NULL)
	{
		return NULL;
	}
	Billing billing;
	strcpy(billing.aNum, pCard->aNum);  // 复制卡号到账单
	billing.tStart = time(NULL);  // 记录上机时间
	billing.fAmount = 0;  // 初始消费金额为0
	billing.tEnd = time(NULL);  // 下机时间初始化为当前时间
	billing.nStatus = 0;  // 结算状态：未结算
	billing.fBalance = pCard->Balance;  // 记录当前余额
	billing.nDel = 0;  // 删除状态：未删除
	billing.vipgress = pCard->vipgress;  // 记录VIP等级
	if (TRUE == saveBilling(&billing, BILLINGPATH))  // 保存账单信息到文件
	{
		return pCard;
	}
	return NULL;
}
Card* doSettleInfo(char* aNum, char* aCode, double* fAmount)
{
	// 获取卡信息和账单信息文件中的位置
	int nIndexb = 0;
	int nIndexc = 0;
	Card* pCard = NULL;
	// 在获取卡信息同时获取位置索引
	pCard = doSettle(aNum, aCode, &nIndexc);// 调用结算验证函数
	Billing* pBilling = NULL;
	pBilling = checkBilling(aNum, &nIndexb);// 获取未结算的账单
	if (pBilling != NULL)
	{
		// 封装结算信息
		pBilling->tEnd = time(0);  // 记录下机时间
		pBilling->fAmount = getAmount(pBilling->tStart, pCard->vipgress);  // 计算消费金额
		*fAmount = pBilling->fAmount;  // 通过指针返回消费金额
		pBilling->nStatus = 1;  // 标记账单为"已结算"
		pBilling->fBalance -= pBilling->fAmount;  // 更新余额
		pCard->nStatus = 0;  // 更新卡状态为"未上机"

		// 检查余额是否足够
		if (pCard->Balance < pBilling->fAmount)
		{
			printf("卡内余额不足，请充值后再下机。\n");
			return NULL;
		}

		pBilling->fBalance -= pBilling->fAmount;
		// 封装卡信息  记录上机/下机时间
		pCard->nStatus = 0;
		timetostr(pCard->tLastuse, pCard->everytime3[pCard->Usecnt - 1]);
		transTime(pCard->tLastuse, pCard->everytime1[pCard->Usecnt - 1]);
		pCard->tLastuse = time(NULL);
		timetostr(pCard->tLastuse, pCard->everytime4[pCard->Usecnt - 1]);
		transTime(pCard->tLastuse, pCard->everytime2[pCard->Usecnt - 1]);

		pCard->Totaluse += pBilling->fAmount;  // 更新总使用时长
		pCard->Balance = pBilling->fBalance;  // 更新卡余额
		pCard->everymoney[pCard->Usecnt - 1] = pBilling->fAmount;  // 记录本次消费金额

		// 根据消费金额增加抽奖次数
		if (pCard->vipgress == 1 && pBilling->fAmount >= 10)
		{
			pCard->lucy++;
		}
		if (pCard->vipgress == 0 && pBilling->fAmount >= 20)
		{
			pCard->lucy++;
		}
		// 更新卡信息和账单信息到文件
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
