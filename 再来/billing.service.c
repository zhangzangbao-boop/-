#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>
#include"billing.file.h"
#include"global.h"
lpBillingNode BillingList = NULL;
int initBillingList()
{
	lpBillingNode pHead = NULL;
	if ((pHead = (lpBillingNode)malloc(sizeof(BillingNode))) == NULL)
	{
		printf("链表初始化失败！\n");
		return FALSE;
	}
	else
	{
		pHead->next = NULL;
		BillingList = pHead;
		return TRUE;
	}
}
void releaseBillingList()
{
	lpBillingNode pCur = NULL;
	lpBillingNode pNext = NULL;
	if (BillingList != NULL)
	{
		pCur = BillingList->next;
		pNext = pCur->next;
		while (pCur != NULL)
		{
			free(pCur);
			pCur = NULL;
			if (pNext != NULL)
			{
				pCur = pNext;
				pNext = pCur->next;
			}
		}
	}
}

int getBilling()
{
	int billingCnt = 0;
	if ((billingCnt = countBilling(BILLINGPATH)) == 0)
	{
		return FALSE;
	}
	int i = 0;
	Billing* pBilling = NULL;
	pBilling = (Billing*)malloc(sizeof(Billing) * billingCnt);
	if (pBilling == NULL)
	{
		printf("堆区内存已经用完！\n");
		exit(1);
	}
	if (readBillingFile(pBilling, BILLINGPATH) == FALSE)
	{
		printf("文件读取失败！");
		return FALSE;
	}
	lpBillingNode node = NULL;
	lpBillingNode pCur = NULL;
	if (BillingList != NULL)
	{
		releaseBillingList();
	}
	initBillingList();
	node = BillingList;
	for (i = 0; i < billingCnt; i++)
	{
		if ((pCur = (lpBillingNode)malloc(sizeof(BillingNode))) == NULL)
		{
			free(pBilling);
			pBilling = NULL;
			return FALSE;
		}
		memset(pCur, 0, sizeof(BillingNode));
		pCur->data = pBilling[i];
		pCur->next = NULL;
		while (node->next != NULL)
		{
			node = node->next;
		}
		node->next = pCur;
	}
	free(pBilling);
	pBilling = NULL;
	return TRUE;
}
Billing* checkBilling(char* aNum, int* pIndex)
{
	if (getBilling() == FALSE)
	{
		return NULL;
	}
	lpBillingNode pCur = NULL;
	if (BillingList != NULL)
	{
		pCur = BillingList->next;
		while (pCur != NULL)
		{
			if ((strcmp(pCur->data.aNum, aNum) == 0) && (pCur->data.nStatus == 0))
			{
				return &pCur->data;
			}
			pCur = pCur->next;
			(*pIndex)++;
		}
	}
	return NULL;
}