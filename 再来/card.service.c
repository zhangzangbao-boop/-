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

void checkcardStd()
{
	if (getCard() == FALSE)
		return NULL;
	int num1 = 0, num2 = 0, num3 = 0;
	lpCardNode pCur = NULL;
	if (cardList != NULL)
	{
		pCur = cardList->next;
		while (pCur != NULL)
		{
			if (pCur->data.nStatus == 0 || pCur->data.nStatus == 1)
				num2++;
			if (pCur->data.nStatus == 2)
				num3++;
			pCur = pCur->next;
			num1++;
		}
	}
	printf("��ֹĿǰ������%d�ſ���ע�ᣬ��%d�ſ���Ȼ��ʹ���У���%d�ſ��Ѿ�����ע��\n", num1, num2, num3);
}

void findmost()
{
	if (getCard() == FALSE)
		return NULL;
	int flag = 0;
	lpCardNode pCur = NULL;
	if (cardList != NULL)
	{
		printf("-----��Ծ�û���Ϣ-----\n");
		pCur = cardList->next;

		while (pCur != NULL)
		{
			if ((pCur->data.vipgress==0&&pCur->data.Totaluse > 100)||(pCur->data.vipgress==1&&pCur->data.Totaluse>=70)&&pCur->data.nStatus!=2)
			{
				if (flag == 0)
					printf("�˺�\t\t��ʹ�ý��\t�ϴ�ʹ��ʱ��\t\tvip״̬\n");  //���ţ���ʹ�ý��ϴ�ʹ��ʱ��
				flag = 1;
				char lastime[20] = { 0 };
				transTime(pCur->data.tLastuse, lastime);
				printf("%s\t\t%.2lf\t\t%s\t\t%d\n", pCur->data.aNum, pCur->data.Totaluse, lastime,pCur->data.vipgress);
			}
			pCur = pCur->next;
		}
		if (flag == 0)
		{
			printf("��ϵͳ���޻�Ծ�û��������ľ�Ӫ��\a\n");
		}
	}
}

void findunder()
{
	if (getCard() == FALSE)
		return;

	char startTimeStr[20];
	char endTimeStr[20];

	// ��ʾ�û�������ʼʱ��ͽ���ʱ��
	int c;
	while ((c = getchar()) != '\n' && c != EOF) {}

	printf("��������ʼʱ�� (��ʽ: YYYY-MM-DD HH:MM): ");
	fgets(startTimeStr, sizeof(startTimeStr), stdin);
	size_t len = strlen(startTimeStr);
	if (len > 0 && startTimeStr[len - 1] == '\n')
		startTimeStr[len - 1] = '\0';

	printf("���������ʱ�� (��ʽ: YYYY-MM-DD HH:MM): ");
	fgets(endTimeStr, sizeof(endTimeStr), stdin);
	len = strlen(endTimeStr);
	if (len > 0 && endTimeStr[len - 1] == '\n')
		endTimeStr[len - 1] = '\0';

	// �������ʱ���ַ���ת��Ϊ time_t ����
	time_t startTime = strToTime(startTimeStr);
	time_t endTime = strToTime(endTimeStr);

	// ʱ��Ϸ���У��
	if (startTime == -1 || endTime == -1 || startTime > endTime) {
		printf("�����ʱ�䲻�Ϸ��������ʽ�ͷ�Χ��\n");
		return;
	}

	int flag = 0;
	lpCardNode pCur = NULL;
	if (cardList != NULL)
	{
		printf("-----Ǳ���û���Ϣ-----\n");
		pCur = cardList->next;

		while (pCur != NULL)
		{
			// ����ϴ�ʹ��ʱ���Ƿ���ָ��ʱ�����
			if (pCur->data.tLastuse >= startTime && pCur->data.tLastuse <= endTime &&
				(((pCur->data.tLastuse < pCur->data.undertime) && (pCur->data.Totaluse > 50)) ||
					(pCur->data.vipgress == 1 && pCur->data.nStatus != 2)))
			{
				if (flag == 0)
					printf("�˺�\t\t��ʹ�ý��\t�ϴ�ʹ��ʱ��\t\tvip״̬\n");  //���ţ���ʹ�ý��ϴ�ʹ��ʱ��,vip
				flag = 1;
				char lastime[20] = { 0 };
				transTime(pCur->data.tLastuse, lastime);
				printf("%s\t\t%.2lf\t\t%s\t\t%d\n", pCur->data.aNum, pCur->data.Totaluse, lastime, pCur->data.vipgress);
			}
			pCur = pCur->next;
		}
		if (flag == 0)
		{
			printf("��ϵͳ��ָ��ʱ���������Ǳ���û��������ľ�Ӫ��\a\n");
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
void checkUser()
{
	if (getCard() == FALSE)
	{
		return NULL;
	}
	char aName[20] = {0};
	int i;
	int flag = 0;
	printf("��������Ҫ��ѯ���˻�:");
	scanf("%s", aName);
	lpCardNode pCur = NULL;
	if (cardList != NULL)
	{
		pCur = cardList->next;
		while (pCur != NULL)
		{
			if (strcmp(aName, pCur->data.aNum) == 0)
			{
				flag = 1;
				if (pCur->data.Usecnt >= 1)
				{
					printf("�û���Ϊ��%s�Ŀ���Ϣ����\n", aName);
					printf("�ϻ�ʱ��\t\t\t�»�ʱ��\t\t\tʹ�ý��\n");
					for (i = 0; i < pCur->data.Usecnt; i++)
					{
						printf("%s\t\t%s\t\t%.2lf\n", pCur->data.everytime1[i], pCur->data.everytime2[i], pCur->data.everymoney[i]);
					}
					printf("���û���ʹ�ô���Ϊ%d,�˻����Ϊ%.2lf\n", pCur->data.Usecnt, pCur->data.Balance);
					if (pCur->data.nStatus == 2)
						printf("���û���ע����\n");
				}
				else
				{
					printf("���û�����ʹ�ü�¼\n");
				}
			}
			pCur = pCur->next;
		}
		if (flag == 0)
		{
			printf("δ���ҵ�����û���Ϣ!\a\n");
		}
	}
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

void checkdate()
{

	if (getCard() == FALSE)
	{
		return NULL;
	}
	char Btime[20] = { 0 };
	char Etime[20] = { 0 };
	char time1[20];
	char time2[20];
	int i=0;
	int flag1 = 0;
	int flag2 = 0;
	int flag3 = 0;
	double all = 0;
	double mon = 0;
	printf("��������Ҫ��ѯ�Ŀ�ʼ���ڣ���ʽΪYYYY-MM-DD��:");
	scanf("%s", Btime);
	printf("��������Ҫ��ѯ�Ľ������ڣ���ʽΪYYYY-MM-DD��:");
	scanf("%s", Etime);
	lpCardNode pCur = NULL;
	if (cardList != NULL)
	{
		pCur = cardList->next;
		while (pCur != NULL)
		{
			for (i = 0; i < pCur->data.Usecnt; i++)
			{
				if (strcmp(pCur->data.everytime3[i], Btime) >= 0 && strcmp(pCur->data.everytime4[i], Etime) <= 0)
				{
					mon += pCur->data.everymoney[i];
					flag1 = 1;
					flag3 = 1;
				}
			}
			if (flag1 == 1 && flag2 == 0)
			{
				printf("�˺�\t����\n");
				flag2 = 1;
			}
			if (flag3 == 1)
			{
				printf("%s\t%.2lf\n", pCur->data.aNum, mon);
				flag3 = 0;
				all += mon;
				mon = 0;
			}
			pCur = pCur->next;
		}
		if (flag1 == 0)
		{
			printf("��ʱ��������û�!\n");
		}
	}
	if (flag1 != 0)
	{
		printf("�����ѽ��Ϊ��%.2lf\n", all);
	}
}