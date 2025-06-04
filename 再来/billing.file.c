#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>
#include"global.h"
#include"model.h"
#include"tool.h"
#include"billing.file.h"
#include"billing.service.h"
int saveBilling(Billing* pBilling, const char* pPath)
{
	FILE* fp = NULL;
	if ((fp = fopen(pPath, "ab")) == NULL)
	{
		if ((fp = fopen(pPath, "wb")) == NULL)
		{
			printf("�ļ���ʧ��!\n");
			return FALSE;
		}
	}
	fwrite(pBilling, sizeof(Billing), 1, fp);
	fclose(fp);
	return TRUE;
}
int readBillingFile(Billing* pBilling, const char* pPath)
{
	FILE* fp = NULL;
	Billing* plBiling = NULL;
	int i = 0;
	if ((fp = fopen(pPath, "rb")) == NULL)
	{
		printf("�ļ���ʧ��!\n");
		return FALSE;
	}
	if ((plBiling = (Billing*)malloc(sizeof(Billing))) == NULL)
	{
		return FALSE;
	}
	//�����ļ�
	while (!feof(fp))
	{
		if (0 != fread(plBiling, sizeof(Billing), 1, fp))
		{
			pBilling[i++] = *plBiling;
		}
	}
	free(plBiling);
	//�ر��ļ�
	fclose(fp);

	return TRUE;
}
int countBilling(const char* pPath)
{
	FILE* fp = NULL; //�ļ�ָ��
	int nIndex = 0; //�����������
	Billing* pBilling = NULL;

	//��ֻ���ķ�ʽ�򿪶������ļ�, �����ʧ��, �򷵻�FALSE
	if ((fp = fopen(pPath, "rb")) == NULL)
	{
		if ((fp = fopen(pPath, "wb+")) == NULL)
		{
			printf("�ļ���ʧ��!\n");
			return FALSE;
		}
	}

	pBilling = (Billing*)malloc(sizeof(Billing));

	while (!feof(fp))
	{
		if (0 != fread(pBilling, sizeof(Billing), 1, fp))
		{
			nIndex++;
		}
	}

	//�ر��ļ�
	fclose(fp);

	//�����ļ���������Ϣ������
	return nIndex;
}
int updateBilling(Billing* pBilling, const char* pPath, int nIndex)
{
	FILE* fp = NULL; //�ļ�ָ��
	Billing* plBilling = NULL;
	int nLine = 0;
	long lPosition = 0; //�ļ�λ�ñ��

	//�Զ�д�ķ�ʽ�򿪶������ļ�, ����ļ���ʧ��, �򷵻�FALSE

	if ((fp = fopen(pPath, "rb+")) == NULL)
	{
		return FALSE;
	}
	if ((plBilling = (Billing*)malloc(sizeof(Billing))) == NULL)
	{
		return FALSE;
	}


	//�����ļ�, �ҵ�������Ϣ, ������
	while ((!feof(fp)) && (nLine < nIndex))
	{
		if (0 != fread(plBilling, sizeof(Billing), 1, fp))
		{
			//��ȡ�ļ���ʶλ��
			lPosition = ftell(fp);
			nLine++;
		}
	}

	//�ƶ����ļ���ʶλ��
	fseek(fp, lPosition, 0);

	//������д���ļ�
	fwrite(pBilling, sizeof(Billing), 1, fp);

	//�ر��ļ�
	fclose(fp);

	return TRUE;
}