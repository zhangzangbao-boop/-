#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>
#include"global.h"
#include"model.h"
#include"tool.h"
#include"card.file.h"
#include"card.service.h"

//������Ϣ�Զ�������ʽ׷�ӵ��ļ������ļ��������򴴽�
int saveCard(Card* pCard, const char* pPath)  //ָ�� Card ��ָ��  �ַ���·��ָ��
{
	FILE* fp = NULL;
	if ((fp = fopen(pPath, "ab")) == NULL)   //���ļ���׷��ģʽ ab��
	{
		if ((fp = fopen(pPath, "wb")) == NULL)  //��ʧ��������д��ģʽ�����ļ���wb��
		{
			printf("�ļ���ʧ��!\n");
			return FALSE;
		}
	}
	fwrite(pCard, sizeof(Card), 1, fp);  //�Զ����Ƹ�ʽ��һ���ſ��ṹд���ļ�
	fclose(fp);
	return TRUE;
}

//���ļ���ȡ���п���Ϣ�����飬���ڳ�ʼ������
int readFile(Card* pCard, const char* pPath)
{
	FILE* fp = NULL;
	Card* plCard = NULL;
	int i = 0;
	//���ļ�
	if ((fp = fopen(pPath, "rb")) == NULL)
	{
		printf("�ļ���ʧ��!\n");
		return FALSE;
	}
	//�����ڴ�
	if ((plCard = (Card*)malloc(sizeof(Card))) == NULL)
	{
		return FALSE;
	}
	//�����ļ�
	while (!feof(fp))
	{
		if (0 != fread(plCard, sizeof(Card), 1, fp))
		{
			pCard[i++] = *plCard;
		}
	}
	free(plCard);
	//�ر��ļ�
	fclose(fp);

	return TRUE;
}

//ͳ���ļ��п�������������ȷ�������С
int countCard(const char* pPath)
{
	FILE* fp = NULL;
	int nIndex = 0; //�����������
	Card* pCard = NULL;

	//��ֻ���ķ�ʽ�򿪶������ļ�, �����ʧ��, �򷵻�FALSE
	if ((fp = fopen(pPath, "rb")) == NULL)
	{
		if ((fp = fopen(pPath, "wb+")) == NULL)
		{
			printf("�ļ���ʧ��!\n");
			return FALSE;
		}
	}

	pCard = (Card*)malloc(sizeof(Card));

	while (!feof(fp))
	{
		if (0 != fread(pCard, sizeof(Card), 1, fp))
		{
			nIndex++;
		}
	}

	fclose(fp);

	//�����ļ���������Ϣ������
	return nIndex;
}

//�����ļ���ָ��λ�õĿ���Ϣ��ͨ����λ�ļ�ָ��ʵ��
int updateCard(Card* pCard, const char* pPath, int nIndex)
{
	FILE* fp = NULL;
	Card* plCard = NULL;
	int nLine = 0;
	long lPosition = 0; //�ļ�λ�ñ��

	//�Զ�д�ķ�ʽ�򿪶������ļ�, ����ļ���ʧ��, �򷵻�FALSE

	if ((fp = fopen(pPath, "rb+")) == NULL)
	{
		return FALSE;
	}
	if ((plCard = (Card*)malloc(sizeof(Card))) == NULL)
	{
		return FALSE;
	}
	//�����ļ�, �ҵ�������Ϣ, ������
	while ((!feof(fp)) && (nLine < nIndex))
	{
		if (0 != fread(plCard, sizeof(Card), 1, fp))
		{
			//��ȡ�ļ���ʶλ��
			lPosition = ftell(fp);
			nLine++;
		}
	}
	//��λ���ļ���ʶλ��
	fseek(fp, lPosition, 0);
	//���и���ʽд��
	fwrite(pCard, sizeof(Card), 1, fp);
	fclose(fp);
	return TRUE;
}