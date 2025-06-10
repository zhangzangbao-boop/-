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
void statisticsByYearMonth(int year, int month) {
	int billingCnt = countBilling(BILLINGPATH);
	if (billingCnt == 0) {
		printf("没有计费记录。\n");
		return;
	}
	Billing* pBilling = (Billing*)malloc(sizeof(Billing) * billingCnt);
	if (pBilling == NULL) {
		printf("内存分配失败。\n");
		return;
	}
	if (readBillingFile(pBilling, BILLINGPATH) == FALSE) {
		printf("文件读取失败。\n");
		free(pBilling);
		return;
	}

	double totalAmount = 0;
	int hasRecord = 0;

	printf("%d年%d月上机下机消费情况统计表\n", year, month);
	// 使用格式化输出，指定每列宽度
	printf("%-10s %-20s %-20s %-10s\n", "卡号", "上机时间", "下机时间", "消费金额");

	for (int i = 0; i < billingCnt; i++) {
		struct tm* tmStart = localtime(&pBilling[i].tStart);
		if (tmStart->tm_year + 1900 == year && tmStart->tm_mon + 1 == month) {
			char startTime[20], endTime[20];
			transTime(pBilling[i].tStart, startTime);
			transTime(pBilling[i].tEnd, endTime);
			// 使用格式化输出，指定每列宽度
			printf("%-10s %-20s %-20s %-10.2lf\n", pBilling[i].aNum, startTime, endTime, pBilling[i].fAmount);
			totalAmount += pBilling[i].fAmount;
			hasRecord = 1;
		}
	}

	if (hasRecord) {
		// 使用格式化输出，指定每列宽度
		printf("%-10s %-20s %-20s %-10.2lf\n", "合计", "", "", totalAmount);
	}
	else {
		printf("该时间段内没有消费记录。\n");
	}

	free(pBilling);
}