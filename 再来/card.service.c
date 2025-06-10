#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>
#include"global.h"
#include"card.file.h"
#include"card.service.h"
#include "billing.file.h"
#include"tool.h"

extern lpBillingNode BillingList;

void timetostr(time_t a, char* s)   //��ʱ��ת��Ϊ������
{
	struct tm* timeinfo;
	timeinfo = localtime(&a);
	strftime(s, 20, "%Y-%m-%d", timeinfo);
}
lpCardNode cardList = NULL;

int initCardList()//�����ʼ������
{
	lpCardNode pHead = NULL;
	//�ڴ����
	if ((pHead = (lpCardNode)malloc(sizeof(CardNode))) == NULL)
	{
		printf("�����ʼ��ʧ�ܣ�\n");
		return FALSE;
	}
	else
	{
		//������������
		pHead->next = NULL;
		cardList = pHead;
		return TRUE;
	}
}
int checkCard(char* str)
{
	if (getCard() == FALSE)
	{
		return FALSE;
	}
	lpCardNode pCur = NULL;
	int flag = 0;
	if (cardList != NULL)
	{
		pCur = cardList->next;
		while (pCur != NULL)
		{
			if (strcmp(pCur->data.aNum, str) == 0 && pCur->data.nStatus != 2)
			{
				flag = 1;
			}
			pCur = pCur->next;
		}
	}
	return flag;
}
void releaseCardList()//�����������
{
	lpCardNode pCur = NULL;
	lpCardNode pNext = NULL;
	if (cardList != NULL)
	{
		//ͷ�ڵ㲻�洢��Ϣ����ָ����һ�ڵ�
		pCur = cardList->next;
		//���浱ǰ�ڵ����һ�ڵ��ٶԵ�ǰ�ڵ�����ͷ�
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


Card* quaryCard(char* str)
{
	if (getCard() == FALSE)
	{
		return NULL;
	}
	lpCardNode pCur = NULL;
	if (cardList != NULL)
	{
		pCur = cardList->next;
		while (pCur != NULL)
		{
			if (strcmp(pCur->data.aNum, str) == 0)
			{
				if (pCur->data.nStatus != 2)
				{
					return &pCur->data;
				}
			}
			pCur = pCur->next;
		}
	}
	return NULL;
}
int getCard()
{
	//��ÿ���Ϣ����
	int cardCnt = 0;
	if ((cardCnt = countCard(CARDPATH)) == 0)
	{
		return FALSE;
	}
	int i = 0;
	Card* pCard = NULL;
	//�����ڴ�
	pCard = (Card*)malloc(sizeof(Card) * cardCnt);
	if (pCard == NULL)
	{
		printf("�����ڴ��Ѿ����꣡\n");
		exit(1);
	}
	//��ȡ�ļ��п���Ϣ
	if (readFile(pCard, CARDPATH) == FALSE)
	{
		printf("�ļ���ȡʧ�ܣ�");
		return FALSE;
	}
	//��������ͷ�ڵ㣬��ǰ�ڵ�
	lpCardNode node = NULL;
	lpCardNode pCur = NULL;
	//�������Ϊ�����������
	if (cardList != NULL)
	{
		releaseCardList();
	}
	//��ʼ������
	initCardList();
	node = cardList;
	//������������
	//β�巨ʵ��
	for (i = 0; i < cardCnt; i++)
	{
		if ((pCur = (lpCardNode)malloc(sizeof(CardNode))) == NULL)
		{
			free(pCard);
			pCard = NULL;
			return FALSE;
		}
		memset(pCur, 0, sizeof(CardNode));
		pCur->data = pCard[i];
		pCur->next = NULL;
		while (node->next != NULL)
		{
			node = node->next;// ���������ҵ�β�ڵ�
		}
		node->next = pCur; // �����½ڵ㵽β��
	}
	//�ͷŽṹ�������ڴ�
	free(pCard);
	pCard = NULL;
	return TRUE;
}
Card* doLogon(char* aNum, char* aCode)
{
	int nIndex = 0;
	// ���ؿ���Ϣ���ж��Ƿ�ɹ�
	if (getCard() == FALSE)
	{
		return NULL;
	}
	lpCardNode pCur = NULL;
	if (cardList != NULL)
	{
		pCur = cardList->next;
		// ����������ҿ���
		while (pCur != NULL)
		{
			if (strcmp(pCur->data.aNum, aNum) == 0)
			{
				// ��Ҫδע��
				if (pCur->data.nStatus != 2)
				{
					// ������Ҫ������ȷ
					if (strcmp(pCur->data.aCode, aCode) != 0)
					{
						printf("�����������\n");
						return NULL;
					}
					// �����Ҫ�㹻
					if (pCur->data.Balance < MIN_BALANCE)
					{
						printf("���������� %.2lf Ԫ�����ֵ�����ϻ���\n", MIN_BALANCE);
						return NULL;
					}
					// ��Ҫ����δ�ϻ�״̬
					if (pCur->data.nStatus == 1)
					{
						printf("�ÿ����ϻ�\n");
						return NULL;
					}
					// �޸Ŀ��ϻ�״̬��ʹ��ʱ��
					pCur->data.nStatus = 1;
					pCur->data.Usecnt++;
					pCur->data.tLastuse = time(NULL);
					// �ж��Ƿ�ɹ����޸ĺ�Ŀ���Ϣ���浽�ļ�
					if (FALSE != updateCard(&pCur->data, CARDPATH, nIndex))
					{
						return &pCur->data;
					}
				}
			}
			pCur = pCur->next;
			nIndex++;
		}
	}
	printf("�ÿ��Ų����ڡ�\n");
	return NULL;
}
Card* doSettle(char* aNum, char* aCode, int* nIndex)
{
	if (getCard() == FALSE)
	{
		return NULL;
	}
	lpCardNode pCur = NULL;
	if (cardList != NULL)
	{
		pCur = cardList->next;
		while (pCur != NULL)
		{
			if (strcmp(pCur->data.aNum, aNum) == 0) // �ҵ�ƥ�俨��
			{
				if (pCur->data.nStatus != 2)  // ��δע��
				{
					if (strcmp(pCur->data.aCode, aCode) != 0)
					{
						printf("�������\n");
						return NULL;
					}
					if (pCur->data.nStatus == 0)
					{
						printf("�ÿ�δ�ϻ���\n");
						return NULL;
					}
					if (FALSE != updateCard(&pCur->data, CARDPATH, *nIndex))
					{
						return &pCur->data;
					}
				}
			}
			pCur = pCur->next;
			(*nIndex)++;
		}
	}
	printf("���˺Ų����ڣ�\n");
	return NULL;
}

Card* doAddmoney(const char* name, const char* code, double* money)
{
	int index = 0;
	if (getCard() == FALSE)  //��ȡ��Ƭ��Ϣʧ��
	{
		return NULL;
	}

	//������Ƭ�б����ƥ�俨��
	lpCardNode pCur = NULL;
	if (cardList != NULL)
	{
		pCur = cardList->next;
		while (pCur != NULL)
		{
			if (strcmp(pCur->data.aNum, name) == 0)//�ҵ�ƥ�俨��
			{
				if (strcmp(pCur->data.aCode, code) != 0)
				{
					printf("������������\a\n");
					return NULL;
				}
				else
				{
					if (pCur->data.nStatus == 2)
					{
						printf("�ÿ���ע����\a");

					} 
					else
					{
						//ִ�г�ֵ����
						pCur->data.Balance += *money;
						if (*money >= 100&&pCur->data.vipgress==0)
						{
							pCur->data.vipgress = 1;
						}
						//������µ��ļ�
						if (FALSE != updateCard(&pCur->data, CARDPATH, index))
						{
							return &pCur->data;
						}
					}
				}
			}
			pCur = pCur->next;
			index++;
		}
		printf("���˺Ų����ڣ�\a\n");
	}
	return NULL;
}

Card* doReturnmoney(const char* name, const char* code, double* money)
{
	int index = 0;
	if (getCard() == FALSE)
	{
		return NULL;
	}
	lpCardNode pCur = NULL;
	if (cardList != NULL)
	{
		pCur = cardList->next;
		while (pCur != NULL)
		{
			if (strcmp(pCur->data.aNum, name) == 0)
			{
				if (strcmp(pCur->data.aCode, code) != 0)
				{
					printf("������������\a\n");
					return NULL;
				}
				else
				{
					if (pCur->data.nStatus == 2)
					{
						printf("�ÿ���ע����\a\n");
						return NULL;
					}
					else
					{
						if (pCur->data.nStatus == 1)
						{
							printf("�ÿ�����ʹ�ã������»���\a\n");
							return NULL;
						}
						else
						{
							if (pCur->data.Balance < *money)
							{
								printf("�ÿ����㣡\a\n");
								return NULL;
							}
							else
							{
								pCur->data.Balance -= *money;
								if (FALSE != updateCard(&pCur->data, CARDPATH, index))
								{
									return &pCur->data;
								}
							}
						}
					}
				}
			}
			pCur = pCur->next;
			index++;
		}
		printf("���˺Ų����ڣ�\a\n");
	}
	return NULL;
}

Card* doCancel(const char* name, const char* code)
{
	int index = 0;
	if (FALSE == getCard())
	{
		return NULL;
	}
	lpCardNode pCur;
	if (cardList != NULL)
	{
		pCur = cardList->next;
		while (pCur != NULL)
		{
			if (strcmp(pCur->data.aNum, name) == 0)
			{
				if (strcmp(pCur->data.aCode, code) != 0)
				{
					printf("������������\a\n");
					return NULL;
				}
				else
				{
					if (pCur->data.nStatus == 1)
					{
						printf("�ÿ�����ʹ�ã������»���\a\n");
						return NULL;
					}
					else
					{
						pCur->data.nStatus = 2;
						if (FALSE != updateCard(&pCur->data, CARDPATH, index))
						{
							return &pCur->data;
						}
					}
				}
			}
			pCur = pCur->next;
			index++;
		}
		printf("���˺Ų����ڣ�\a\n");
	}
	return NULL;
}
Card* queryCards(const char* pName, int* pIndex)
{
	lpCardNode pCur = NULL;
	Card* pCard = NULL;
	if (getCard() == FALSE)
		return NULL;
	pCard = (Card*)malloc(sizeof(Card));
	if (pCard == NULL)
		return NULL;
	pCur = cardList->next;
	while (pCur != NULL)
	{
		if (strstr(pCur->data.aNum, pName) != NULL&&(pCur->data.nStatus!=2))
		{
			pCard[*pIndex] = pCur->data;
			(*pIndex)++;
			pCard = (Card*)realloc(pCard, ((*pIndex) + 1) * sizeof(Card));
		}
		pCur = pCur->next;
	}
	return pCard;
}



void findunder()
{
	if (getCard() == FALSE)
		return;

	char yearMonthStr[20];
	int year, month;

	// ��ʾ�û�����ͳ�Ƶ�����
	int c;
	while ((c = getchar()) != '\n' && c != EOF) {}

	printf("������Ҫͳ�Ƶ����� (��ʽ: YYYY-MM): ");
	fgets(yearMonthStr, sizeof(yearMonthStr), stdin);
	size_t len = strlen(yearMonthStr);
	if (len > 0 && yearMonthStr[len - 1] == '\n')
		yearMonthStr[len - 1] = '\0';

	// �������������
	if (sscanf(yearMonthStr, "%d-%d", &year, &month) != 2) {
		printf("����ĸ�ʽ����ȷ����ʹ�� YYYY-MM ��ʽ��\n");
		return;
	}

	// ������µ���ʼʱ��ͽ���ʱ��
	struct tm startTm = { 0 };
	startTm.tm_year = year - 1900;
	startTm.tm_mon = month - 1;
	startTm.tm_mday = 1;
	startTm.tm_hour = 0;
	startTm.tm_min = 0;
	startTm.tm_sec = 0;
	time_t startTime = mktime(&startTm);

	struct tm endTm = startTm;
	endTm.tm_mon++;
	time_t endTime = mktime(&endTm);

	double totalRecharge = 0;
	double totalRefund = 0;

	int flag = 0;
	lpCardNode pCur = NULL;
	if (cardList != NULL)
	{
		printf("----- %d �� %d �³�ֵ�˷���ϸ -----\n", year, month);
		printf("�˺�\t\t��������\t���\t����ʱ��\n");
		pCur = cardList->next;

		while (pCur != NULL)
		{
			for (int i = 0; i < pCur->data.Usecnt; i++) {
				time_t opTime = strToTime(pCur->data.everytime1[i]);
				if (opTime >= startTime && opTime < endTime) {
					if (pCur->data.everymoney[i] > 0) {
						// ��ֵ����
						totalRecharge += pCur->data.everymoney[i];
						printf("%s\t\t��ֵ\t\t%.2lf\t%s\n", pCur->data.aNum, pCur->data.everymoney[i], pCur->data.everytime1[i]);
						flag = 1;
					}
					else if (pCur->data.everymoney[i] < 0) {
						// �˷Ѳ���
						totalRefund += -pCur->data.everymoney[i];
						printf("%s\t\t�˷�\t\t%.2lf\t%s\n", pCur->data.aNum, -pCur->data.everymoney[i], pCur->data.everytime1[i]);
						flag = 1;
					}
				}
			}
			pCur = pCur->next;
		}

		if (flag == 0)
		{
			printf("��ϵͳ��ָ��ʱ��������޳�ֵ�˷Ѽ�¼���������ע��\a\n");
		}
		else
		{
			printf("----- %d �� %d �³�ֵ�˷ѻ��� -----\n", year, month);
			printf("��ֵ�ܽ��: %.2lf Ԫ\n", totalRecharge);
			printf("�˷��ܽ��: %.2lf Ԫ\n", totalRefund);
		}
	}
}
void totalMoney() {
	int choice;
	printf("��ѡ��ͳ�����ͣ�1 - ����ͳ�ƣ�2 - ����ͳ�ƣ�");
	scanf("%d", &choice);

	int year, month;
	if (choice == 1) {
		printf("������Ҫͳ�Ƶ���ݣ�");
		scanf("%d", &year);
		month = 0; // ����ͳ��ʱ���·���Ϊ 0
	}
	else if (choice == 2) {
		printf("������Ҫͳ�Ƶ���ݺ��·ݣ���ʽ��YYYY MM����");
		scanf("%d %d", &year, &month);
	}
	else {
		printf("������Ч��������ѡ��\n");
		return;
	}

	// ���꣨�£�ͳ�ƿ������
	if (getCard() == FALSE) {
		return;
	}
	lpCardNode pCur = cardList->next;
	int openCardCount = 0;
	double totalRecharge = 0;

	printf("\n%d��", year);
	if (month != 0) {
		printf("%d��", month);
	}
	printf("�������ͳ�Ʊ�\n");
	// ʹ�ù̶���ȸ�ʽ�������ͷ
	printf("%-10s %-20s %-10s\n", "����", "����ʱ��", "�״γ�ֵ���");

	while (pCur != NULL) {
		struct tm* tm_info;
		tm_info = localtime(&pCur->data.tStart);
		if (tm_info->tm_year + 1900 == year && (month == 0 || tm_info->tm_mon + 1 == month)) {
			char start_time[20];
			transTime(pCur->data.tStart, start_time);
			// ʹ�ù̶���ȸ�ʽ�����ÿ����¼
			printf("%-10s %-20s %.2lf\n", pCur->data.aNum, start_time, pCur->data.Balance);
			openCardCount++;
			totalRecharge += pCur->data.Balance;
		}
		pCur = pCur->next;
	}
	printf("�ϼ�\t��������%d\t��%.2lf\n", openCardCount, totalRecharge);

	// ���꣨�£�ͳ���ϻ��»��������
	if (getBilling() == FALSE) {
		return;
	}
	lpBillingNode pBillingCur = BillingList->next;
	double totalConsumption = 0;

	printf("\n%d��", year);
	if (month != 0) {
		printf("%d��", month);
	}
	printf("�ϻ��»��������ͳ�Ʊ�\n");
	// ʹ�ù̶���ȸ�ʽ�������ͷ
	printf("%-10s %-20s %-20s %-10s\n", "����", "�ϻ�ʱ��", "�»�ʱ��", "���ѽ��");

	while (pBillingCur != NULL) {
		struct tm* start_tm_info;
		start_tm_info = localtime(&pBillingCur->data.tStart);
		struct tm* end_tm_info;
		end_tm_info = localtime(&pBillingCur->data.tEnd);
		if (start_tm_info->tm_year + 1900 == year && (month == 0 || start_tm_info->tm_mon + 1 == month)) {
			char start_time[20];
			transTime(pBillingCur->data.tStart, start_time);
			char end_time[20];
			transTime(pBillingCur->data.tEnd, end_time);
			// ʹ�ù̶���ȸ�ʽ�����ÿ����¼
			printf("%-10s %-20s %-20s %.2lf\n", pBillingCur->data.aNum, start_time, end_time, pBillingCur->data.fAmount);
			totalConsumption += pBillingCur->data.fAmount;
		}
		pBillingCur = pBillingCur->next;
	}
	printf("�ϼ�\t%.2lf\n", totalConsumption);
}

Card* getlucky(const char* name, const char* code, int* nIndex)
{
	if (getCard() == FALSE)
	{
		return NULL;
	}
	lpCardNode pCur = NULL;
	if (cardList != NULL)
	{
		pCur = cardList->next;
		while (pCur != NULL)
		{
			if (strcmp(pCur->data.aNum, name) == 0 && strcmp(pCur->data.aCode, code) == 0&&pCur->data.lucy!=0)
			{
		
				return &pCur->data;
			}
			pCur = pCur->next;
			(*nIndex)++;
		}
	}
	return NULL;
}

