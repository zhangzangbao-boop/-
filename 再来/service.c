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
	pCard = doLogon(aNum, aCode);  // ���õ�¼��֤����
	if (pCard == NULL)
	{
		return NULL;
	}
	Billing billing;
	strcpy(billing.aNum, pCard->aNum);  // ���ƿ��ŵ��˵�
	billing.tStart = time(NULL);  // ��¼�ϻ�ʱ��
	billing.fAmount = 0;  // ��ʼ���ѽ��Ϊ0
	billing.tEnd = time(NULL);  // �»�ʱ���ʼ��Ϊ��ǰʱ��
	billing.nStatus = 0;  // ����״̬��δ����
	billing.fBalance = pCard->Balance;  // ��¼��ǰ���
	billing.nDel = 0;  // ɾ��״̬��δɾ��
	billing.vipgress = pCard->vipgress;  // ��¼VIP�ȼ�
	if (TRUE == saveBilling(&billing, BILLINGPATH))  // �����˵���Ϣ���ļ�
	{
		return pCard;
	}
	return NULL;
}
Card* doSettleInfo(char* aNum, char* aCode, double* fAmount)
{
	// ��ȡ����Ϣ���˵���Ϣ�ļ��е�λ��
	int nIndexb = 0;
	int nIndexc = 0;
	Card* pCard = NULL;
	// �ڻ�ȡ����Ϣͬʱ��ȡλ������
	pCard = doSettle(aNum, aCode, &nIndexc);// ���ý�����֤����
	Billing* pBilling = NULL;
	pBilling = checkBilling(aNum, &nIndexb);// ��ȡδ������˵�
	if (pBilling != NULL)
	{
		// ��װ������Ϣ
		pBilling->tEnd = time(0);  // ��¼�»�ʱ��
		pBilling->fAmount = getAmount(pBilling->tStart, pCard->vipgress);  // �������ѽ��
		*fAmount = pBilling->fAmount;  // ͨ��ָ�뷵�����ѽ��
		pBilling->nStatus = 1;  // ����˵�Ϊ"�ѽ���"
		pBilling->fBalance -= pBilling->fAmount;  // �������
		pCard->nStatus = 0;  // ���¿�״̬Ϊ"δ�ϻ�"

		// �������Ƿ��㹻
		if (pCard->Balance < pBilling->fAmount)
		{
			printf("�������㣬���ֵ�����»���\n");
			return NULL;
		}

		pBilling->fBalance -= pBilling->fAmount;
		// ��װ����Ϣ  ��¼�ϻ�/�»�ʱ��
		pCard->nStatus = 0;
		timetostr(pCard->tLastuse, pCard->everytime3[pCard->Usecnt - 1]);
		transTime(pCard->tLastuse, pCard->everytime1[pCard->Usecnt - 1]);
		pCard->tLastuse = time(NULL);
		timetostr(pCard->tLastuse, pCard->everytime4[pCard->Usecnt - 1]);
		transTime(pCard->tLastuse, pCard->everytime2[pCard->Usecnt - 1]);

		pCard->Totaluse += pBilling->fAmount;  // ������ʹ��ʱ��
		pCard->Balance = pBilling->fBalance;  // ���¿����
		pCard->everymoney[pCard->Usecnt - 1] = pBilling->fAmount;  // ��¼�������ѽ��

		// �������ѽ�����ӳ齱����
		if (pCard->vipgress == 1 && pBilling->fAmount >= 10)
		{
			pCard->lucy++;
		}
		if (pCard->vipgress == 0 && pBilling->fAmount >= 20)
		{
			pCard->lucy++;
		}
		// ���¿���Ϣ���˵���Ϣ���ļ�
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
