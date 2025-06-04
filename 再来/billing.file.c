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
			printf("文件打开失败!\n");
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
		printf("文件打开失败!\n");
		return FALSE;
	}
	if ((plBiling = (Billing*)malloc(sizeof(Billing))) == NULL)
	{
		return FALSE;
	}
	//遍历文件
	while (!feof(fp))
	{
		if (0 != fread(plBiling, sizeof(Billing), 1, fp))
		{
			pBilling[i++] = *plBiling;
		}
	}
	free(plBiling);
	//关闭文件
	fclose(fp);

	return TRUE;
}
int countBilling(const char* pPath)
{
	FILE* fp = NULL; //文件指针
	int nIndex = 0; //数组的索引号
	Billing* pBilling = NULL;

	//以只读的方式打开二进制文件, 如果打开失败, 则返回FALSE
	if ((fp = fopen(pPath, "rb")) == NULL)
	{
		if ((fp = fopen(pPath, "wb+")) == NULL)
		{
			printf("文件打开失败!\n");
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

	//关闭文件
	fclose(fp);

	//返回文件中消费信息的条数
	return nIndex;
}
int updateBilling(Billing* pBilling, const char* pPath, int nIndex)
{
	FILE* fp = NULL; //文件指针
	Billing* plBilling = NULL;
	int nLine = 0;
	long lPosition = 0; //文件位置标记

	//以读写的方式打开二进制文件, 如果文件打开失败, 则返回FALSE

	if ((fp = fopen(pPath, "rb+")) == NULL)
	{
		return FALSE;
	}
	if ((plBilling = (Billing*)malloc(sizeof(Billing))) == NULL)
	{
		return FALSE;
	}


	//遍历文件, 找到该条信息, 并更新
	while ((!feof(fp)) && (nLine < nIndex))
	{
		if (0 != fread(plBilling, sizeof(Billing), 1, fp))
		{
			//获取文件标识位置
			lPosition = ftell(fp);
			nLine++;
		}
	}

	//移动到文件标识位置
	fseek(fp, lPosition, 0);

	//将数据写进文件
	fwrite(pBilling, sizeof(Billing), 1, fp);

	//关闭文件
	fclose(fp);

	return TRUE;
}