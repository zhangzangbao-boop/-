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
	//��������Ϣ�������Ϣ���ļ��е�������
	int nIndexb = 0;
	int nIndexc = 0;
	Card* pCard = NULL;
	//�ڻ�ȡ����Ϣͬʱ��ȡ��λ��������
	pCard = doSettle(aNum, aCode, &nIndexc);
	Billing* pBilling = NULL;
	pBilling = checkBilling(aNum, &nIndexb);
	if (pBilling != NULL)
	{
		//��װ��������Ϣ
		pBilling->tEnd = time(0);
		pBilling->fAmount = getAmount(pBilling->tStart,pCard->vipgress);
		*fAmount = pBilling->fAmount;
		pBilling->nStatus = 1;
		pBilling->fBalance -= pBilling->fAmount;
		//��װ��������Ϣ
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
		//���޸ĺ����Ϣ���浽�ļ���
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
	printf("�������ѯ������<����Ϊ1~18>:");
	scanf_s("%s", str, 20);
	int flag1 = 0;
	pCard = quaryCard(str);
	if (pCard != NULL)
	{
		flag1 = 1;
		char Lastuse[20];
		transTime(pCard->tLastuse, Lastuse);
		printf("����\t���\t\t��ʹ�ý��\tʹ�ô���\t��һ��ʹ��ʱ��\t\t\t��״̬\t��vip״̬\t�齱����\n");
		if (pCard->vipgress == 1)
			color(12);
		printf("%s\t%.2lf\t\t%.2lf\t\t%d\t\t%s\t\t%d\t%d\t\t%d\n", pCard->aNum, pCard->Balance, pCard->Totaluse, pCard->Usecnt, Lastuse, pCard->nStatus,pCard->vipgress,pCard->lucy);
		color(7);
	}
	if (flag1 == 0)
	{
		printf("δ�ҵ��ÿ���Ϣ��\n");
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
