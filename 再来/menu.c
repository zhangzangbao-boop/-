#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include"model.h"
#include"global.h"
#include"tool.h"
#include"card.file.h"
#include"card.service.h"
#include"billing.file.h"
#include"billing.service.h"
#include<Windows.h>
#include"service.h"
void Outputmenu()
{
	printf("��ӭ����Ʒѹ���ϵͳ\n---------�˵�---------\n");
	printf("1.��ӿ�\n2.��ѯ��\n3.�ϻ�\n4.�»�\n5.��ֵ\n6.�˷�\n7.��ѯͳ��\n8.ע����\n9.���˳齱\n0.�˳�\n");
	printf("��ѡ��˵����ţ�0~9)��");
}
void addcard()			    	// �������ܺ���
{
	Card card;
	card.nStatus = 0;			   // δ�ϻ�
	card.Totaluse = 0;             //��ʹ�ý��
	card.Usecnt = 0;               //ʹ�ô���
	printf("---------����---------\n");
	while (1) {
		printf("�����뿨��<����Ϊ1~18>:");
		scanf("%s", &card.aNum);
		if (count(card.aNum) < 1 || count(card.aNum) > 18) {
			printf("���ų��Ȳ�����Ҫ������������:\n");
		}
		else if (checkCard(card.aNum)) {
			printf("�ÿ����Ѵ��ڣ�����������:\n");
		}
		else {
			break;
		}
	}

	while (1) {
		printf("����������<����Ϊ1~8>:");
		scanf("%s", &card.aCode);
		if (count(card.aCode) < 1 || count(card.aCode) > 8) {
			printf("���볤�Ȳ�����Ҫ������������:\n");
		}
		else {
			break;
		}
	}

	double inputMoney;
	while (1) {
		printf("�����뿪�����<RMB>\n<�״γ�ֵ100Ԫ���Ͽɿ�ͨvipȨ��>:");
		scanf("%lf", &inputMoney);
		if (inputMoney < 0) {
			printf("��������Ϊ���������������롣\n");
		}
		else {
			break;
		}
	}
	card.Balance = inputMoney;//���
	card.tStart = card.tLastuse = time(NULL);
	card.undertime = card.tStart + WEEK;
	card.tEnd = card.tStart + VALIDITY;  // ��Ч�� = ��ǰʱ�� + ʱ��
	card.Usecnt = 0;//ʹ�ô���
	card.lucy = 0;
	memset(card.everytime1, 0, sizeof(card.everytime1));
	memset(card.everytime2, 0, sizeof(card.everytime2));
	memset(card.everytime3, 0, sizeof(card.everytime3));
	memset(card.everytime4, 0, sizeof(card.everytime4));
	if (card.Balance >= 100)//���
	{
		card.vipgress = 1;
		card.lucy = 1;
	}
	else card.vipgress = 0;
	if (saveCard(&card, CARDPATH))   // ������Ϣд���ļ�
	{
		card.nStatus = 0;			   // δ�ϻ�
		printf("������Ϣ�ɹ���\n");
		printf("����\t����\t���\t��״̬\tvip״̬\t���˴���\n");
		printf("%s\t%s\t%.2lf\t%d\t%d\t%d\n", card.aNum, card.aCode, card.Balance, card.nStatus, card.vipgress, card.lucy);
	}
}
void query()
{
	printf("---------��ѯ��---------\n");
	int choice = 0;
	printf("��ѡ���ѯ���ͣ�1---��ȷ��ѯ  2---ģ����ѯ����");
	scanf("%d", &choice);
	switch (choice)
	{
	case 1:  queryInfo(); break;
    case 2:
	{
		Card* pCard;
		int num = 0;
		char aName[20] = { 0 };
		char Lastuse[20] = { 0 };
		printf("�������ѯ����<����Ϊ1-18>��");
		scanf("%s", aName);
		pCard = queryCards(aName, &num);
		if (pCard == NULL || num == 0)
			printf("\nû�иÿ�����Ϣ\n");
		else
		{
			printf("\n��ѯ���Ŀ���Ϣ��\n");
			printf("����\t\t���\t\t��ʹ�ý��\tʹ�ô���\t��һ��ʹ��ʱ��\t\t\t��״̬\tvip״̬\t�齱����\n");
			for (int i = 0; i < num; i++)
			{
				transTime(pCard[i].tLastuse, Lastuse);  //��ʱ��ת��Ϊ�ַ���
				if (pCard[i].vipgress == 1)
				{
					color(12);
						printf("%s\t\t%.2lf\t\t%.2lf\t\t%d\t\t%s\t\t%d\t%d\t%d\n", pCard[i].aNum, pCard[i].Balance, pCard[i].Totaluse, pCard[i].Usecnt, Lastuse, pCard[i].nStatus, pCard[i].vipgress,pCard[i].lucy);
						color(7);
				}
				else
				printf("%s\t\t%.2lf\t\t%.2lf\t\t%d\t\t%s\t\t%d\t%d\t%d\n", pCard[i].aNum, pCard[i].Balance, pCard[i].Totaluse, pCard[i].Usecnt, Lastuse, pCard[i].nStatus,pCard[i].vipgress, pCard[i].lucy);
			}
			free(pCard);
			pCard = NULL;
		}
	}
	break;
	default: printf("��������\a\n"); break;
    } 
}

void logon()
{
	Card* pCard;
	printf("---------�ϻ�---------\n");
	char aNum[20] = { 0 };
	char aCode[10] = { 0 };
	printf("�����뿨��<����Ϊ1~18>��");
	scanf_s("%s", aNum, 20);
	printf("����������<����Ϊ1~8>:");
	scanf_s("%s", aCode, 10);
	pCard = doLogonInfo(aNum, aCode);//�����û��ϻ���������֤�û���Ϣ�������µ����Ѽ�¼
	if (pCard != NULL)
	{
		if (pCard->vipgress == 1)
		{
			color(12);
			printf("�ϻ��ɹ����𾴵�vip�û�!\n");
			char Lastuse[20];
			transTime(pCard->tLastuse, Lastuse);
			color(7);
			printf("����\t\t���\t\t�ϻ�ʱ��\n");
			color(12);
			printf("%s\t\t%.2lf\t\t%s\n", pCard->aNum, pCard->Balance, Lastuse);
			color(7);
		}
		else {
			printf("�ϻ��ɹ���\n");
			char Lastuse[20];
			transTime(pCard->tLastuse, Lastuse);
			printf("����\t\t���\t\t�ϻ�ʱ��\n");
			printf("%s\t\t%.2lf\t\t%s\n", pCard->aNum, pCard->Balance, Lastuse);
		}
	}
	else
	{
		printf("�ϻ�ʧ�ܣ�\n");
	}
}

void settle()
{
	printf("---------�»�---------\n");
	Card* pCard = NULL;
	char aNum[20] = { 0 };
	char aCode[10] = { 0 };
	double fAmount = 0;
	printf("�����뿨��<����Ϊ1~18>��");
	scanf_s("%s", aNum, 20);
	printf("����������<����Ϊ1~8>:");
	scanf_s("%s", aCode, 10);
	pCard = doSettleInfo(aNum, aCode, &fAmount); //�����û��»��������������ѽ����¿����˵�״̬
	if (pCard != NULL)
	{
		if (pCard->vipgress == 1)
		{
			color(12);
			printf("��ӭ���ٴι��٣�\n");
			color(7);
			printf("����\t\t���\t\t���ѽ��\t�齱����\n");
			color(12);
			printf("%s\t\t%.2lf\t\t%.2lf\t\t%d\n", pCard->aNum, pCard->Balance, fAmount,pCard->lucy);
			color(7);
		}
		else
		{
			printf("�»��ɹ���\n");
			printf("����\t\t���\t\t���ѽ��\t�齱����\n");
			printf("%s\t\t%.2lf\t\t%.2lf\t\t%d\n", pCard->aNum, pCard->Balance, fAmount,pCard->lucy);
		}
	}
	else
	{
		printf("�»�ʧ�ܣ�\n");
	}
}

void addmoney()
{
	printf("---------��ֵ---------\n");
	Card* pCard = NULL;
	double money = 0;
	char aNum[20] = { 0 };
	char aCode[10] = { 0 };
	int validInput = 0;

	while (!validInput) {
		printf("�����뿨��<����Ϊ1~18>:");
		scanf_s("%s", aNum, 20);
		printf("����������<����Ϊ1~8>:");
		scanf_s("%s", aCode, 10);

		// ���� doAddmoney �������г�ֵ���������뿨�š�����ͳ�ֵ����ָ��
		pCard = doAddmoney(aNum, aCode, &money);
		if (pCard != NULL && pCard->nStatus == 0) {
			validInput = 1;
		}
		else {
			if (pCard != NULL && pCard->nStatus == 1) {
				printf("���û������ϻ��������»��ٳ�ֵ��\n");
			}
			else {
				printf("���Ż�����������������룡\n");
			}
		}
	}

	if (pCard != NULL && pCard->nStatus == 0) {
		printf("�������ֵ���\n<���γ�ֵ���100Ԫ�����Ͽɿ�ͨvip�˻�>:");
		scanf("%lf", &money);
		pCard = doAddmoney(aNum, aCode, &money);
		// ��ӳ�ֵ�����
		if (money <= 0) {
			printf("��ֵ������Ϊ�����������²�����\n");
			return;
		}
		if (pCard != NULL && pCard->nStatus == 0) {
			printf("��ֵ�ɹ���\n");
			printf("-----��ֵ��Ϣ����-----\n");
			printf("����\t\t��ֵ���\t���\n");
			printf("%s\t\t%.2lf\t\t%.2lf\n", pCard->aNum, money, pCard->Balance);
		}
		else {
			printf("��ֵʧ�ܣ�\n");
		}
	}
}

void returnmoney()
{
	printf("---------�˷�---------\n");
	Card* pCard = NULL;
	double money = 0;
	char aNum[20] = { 0 };
	char aCode[10] = { 0 };
	int validInput = 0;

	while (!validInput) {
		printf("�����뿨��<����Ϊ1~18>:");
		scanf_s("%s", aNum, 20);
		printf("����������<����Ϊ1~8>:");
		scanf_s("%s", aCode, 10);

		pCard = doReturnmoney(aNum, aCode, &money);
		if (pCard != NULL) {
			validInput = 1;
		}
		else {
			printf("���Ż�����������������룡\n");
		}
	}

	if (pCard != NULL) {
		printf("�������˷ѽ�");
		scanf("%lf", &money);
		pCard = doReturnmoney(aNum, aCode, &money);
		if (pCard != NULL) {
			printf("-----�˷���Ϣ����-----\n");
			printf("����\t\t�˷ѽ��\t���\n");
			printf("%s\t\t%.2lf\t\t%.2lf\n", pCard->aNum, money, pCard->Balance);
		}
		else {
			printf("�˷�ʧ�ܣ�\n");
		}
	}
}

void cancel()
{
	printf("---------ע��---------\n");
	Card* pCard = NULL;
	char aNum[20] = { 0 };
	char aCode[10] = { 0 };
	printf("�����뿨��<����Ϊ1~18>��");
	scanf_s("%s", aNum, 20);
	printf("����������<����Ϊ1~8>��");
	scanf_s("%s", aCode, 10);

	// �Ȳ����� doCancel��ֱ�Ӽ�鿨�Ƿ����
	if (checkCard(aNum))
	{
		// ��ȡ����Ϣ
		pCard = quaryCard(aNum);
		if (pCard != NULL)
		{
			printf("--------�û���Ϣ---------\n");
			printf("����\t���\tvip״̬\t�齱����\t\n");
			printf("%s\t%.2lf\t%d\t%d\n", pCard->aNum, pCard->Balance, pCard->vipgress, pCard->lucy);
			printf("�Ƿ�ȷ��ע�����û���1---�� 2---��");
			int choice;
			scanf("%d", &choice);
			if (choice == 1)
			{
				double money = pCard->Balance;
				Card* refundCard = doReturnmoney(aNum, aCode, &money);
				if (refundCard != NULL)
				{
					// �˷ѳɹ��󣬽���״̬����Ϊ��ע��
					pCard->nStatus = 2;
					int index = getCardIndex(aNum);
					if (index != -1 && updateCard(pCard, CARDPATH, index))
					{
						printf("-----ע����Ϣ����-----\n");
						printf("����\t\t�˷ѽ��\n");
						printf("%s\t\t%.2lf\n", pCard->aNum, money);
						printf("ע���ɹ���\n");
					}
					else
					{
						printf("���¿�״̬ʧ�ܣ�\a\n");
					}
				}
				else
				{
					printf("�˷�ʧ�ܣ�\a\n");
				}
			}
			if (choice == 2)
			{
				printf("ע��ʧ�ܣ�\a\n");
			}
		}
	}
	else
	{
		printf("���û������ڣ�\a\n");
	}
}

// ��ȡ��������
int getCardIndex(const char* aNum)
{
	if (getCard() == FALSE)
	{
		return -1;
	}
	lpCardNode pCur = cardList->next;
	int index = 0;
	while (pCur != NULL)
	{
		if (strcmp(pCur->data.aNum, aNum) == 0)
		{
			return index;
		}
		pCur = pCur->next;
		index++;
	}
	return -1;
}

void manager()
{
	printf("-------��ѯͳ��-------\n");
	printf("��ѡ���ѯ��Ŀ\n1---��ֵ�˷���ϸ\n2---���꣨�£�ͳ�ƿ������ϻ��»����\n");
	printf("����˵���ţ�");
	int choice;
	scanf("%d", &choice);
	switch (choice)
	{
	case 1: {
		//���꣨�£�ͳ�Ƴ�ֵ�˷��������Ҫ�г�ֵ�˷ѵ���ϸ�ͻ���
		findunder();
	}
		  break;
	case 2:
	{
		totalMoney();
	}
		  break;
	break;
	default: printf("����������\a\n"); break;
	}
}

void luckytime()
{
	Card* pCard = NULL;
	int Index = 0;
	char aNum[20] = { 0 };
	char aCode[20] = { 0 };
	printf("------���˳齱------\n");
	printf("�˺ţ�"); scanf("%s", aNum);
	printf("���룺"); scanf("%s", aCode);
	pCard = getlucky(aNum,aCode,&Index);
	if(pCard!=NULL)
	{ 
		int c = 0;
		printf("------�齱����------\n");
		printf("һ�Ƚ�������20RMB\n���Ƚ�������10RMB\n���Ƚ�������5RMB\n");
		printf("�Ƿ�����1�γ齱�������г齱��1---�� 2---�񣩣�"); scanf("%d", &c);
		if (c == 2)
		{
			return;
		}
		if (c == 1)
		{
			do
			{
				pCard->lucy--;
				srand(time(0));
				int num = rand() % 100 + 1;
				if (num >= 95)
				{
					color(4);
					printf("��ϲ��������һ�Ƚ���20Ԫ\n");
					color(7);
					pCard->Balance += 20;
				}
				if (num < 95 && num >= 80)
				{
					color(6);
					printf("��ϲ!�����˶��Ƚ���10Ԫ\n");
					color(7);
					pCard->Balance += 10;
				}
				if (num < 80 && num >= 60)
				{
					color(9);
					printf("��ϲ!���������Ƚ���5Ԫ\n");
					color(7);
					pCard->Balance += 5;
				}
				if(num<60)
				{
					printf("δ�н�����л���Ĳ��룡\n");
				}
				updateCard(pCard, CARDPATH, Index);
				printf("�Ƿ��ٴν��г齱(1---�� 2---��):");
				scanf("%d", &c);
			} while (c == 1&&pCard->lucy>0);
			if (c == 1 && pCard->lucy <= 0)
			{
				printf("���ĳ齱�������㣡\n");
			}
		}
	}
	else
	{
			printf("�ÿ������ڻ����޳齱������\a\n");
	}
}