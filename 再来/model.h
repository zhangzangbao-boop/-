#pragma once
#pragma once
#include<time.h>
#ifndef _MODEL_H_
#define _MODEL_H_
typedef struct Data			//��ӿ���Ϣ�ṹ��
{
	char aNum[20];			//����
	char aCode[10];			//����
	int nStatus;			//��״̬��0-δ�ϻ���1-�����ϻ���2-��ע����
	time_t tStart;			//����ʱ��
	time_t tEnd;			//��ֹʱ��
	time_t tLastuse;		//�ϴ�ʹ��ʱ��
	time_t undertime;       //Ǳ���û�����ʱ��
	double Totaluse;		//��ʹ�ý��
	double Balance;			//���
	int Usecnt;				//ʹ�ô���
	int vipgress;           //�û�vip�ȼ�
	double everymoney[100];  // ÿ���ϻ�����
	char everytime1[100][20];  //��¼�ϻ�ʱ��
	char everytime2[100][20];   //��¼�»�ʱ��
	char everytime3[100][20];  //��¼�ϻ�ʱ��(��������)
	char everytime4[100][20];   //��¼�»�ʱ��(��������)
	int lucy;
}Card;

typedef struct CardNode
{
	Card data;
	struct CardNode* next;
}CardNode, * lpCardNode;

typedef struct Billing
{
	char aNum[20];			//����
	time_t tStart;			//�ϻ�ʱ��
	time_t tEnd;			//�»�ʱ��
	double fAmount;			//���ѽ��
	double fBalance;
	int nStatus;			//����״̬��0-δ���㣬1-�ѽ��㣩
	int nDel;				//ɾ��״̬��0-δɾ����1-��ɾ����
	int vipgress;           //�û�vip�ȼ�
}Billing;

typedef struct BillingNode
{
	Billing data;
	struct BillingNode* next;
}BillingNode, * lpBillingNode;
#endif