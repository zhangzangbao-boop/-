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

//将卡信息以二进制形式追加到文件，若文件不存在则创建
int saveCard(Card* pCard, const char* pPath)  //指向 Card 的指针  字符串路径指针
{
	FILE* fp = NULL;
	if ((fp = fopen(pPath, "ab")) == NULL)   //打开文件（追加模式 ab）
	{
		if ((fp = fopen(pPath, "wb")) == NULL)  //若失败则尝试以写入模式创建文件（wb）
		{
			printf("文件打开失败!\n");
			return FALSE;
		}
	}
	fwrite(pCard, sizeof(Card), 1, fp);  //以二进制格式把一整张卡结构写入文件
	fclose(fp);
	return TRUE;
}

//从文件读取所有卡信息到数组，用于初始化链表
int readFile(Card* pCard, const char* pPath)
{
	FILE* fp = NULL;
	Card* plCard = NULL;
	int i = 0;
	//打开文件
	if ((fp = fopen(pPath, "rb")) == NULL)
	{
		printf("文件打开失败!\n");
		return FALSE;
	}
	//分配内存
	if ((plCard = (Card*)malloc(sizeof(Card))) == NULL)
	{
		return FALSE;
	}
	//遍历文件
	while (!feof(fp))
	{
		if (0 != fread(plCard, sizeof(Card), 1, fp))
		{
			pCard[i++] = *plCard;
		}
	}
	free(plCard);
	//关闭文件
	fclose(fp);

	return TRUE;
}

//统计文件中卡的数量，用于确定数组大小
int countCard(const char* pPath)
{
	FILE* fp = NULL;
	int nIndex = 0; //数组的索引号
	Card* pCard = NULL;

	//以只读的方式打开二进制文件, 如果打开失败, 则返回FALSE
	if ((fp = fopen(pPath, "rb")) == NULL)
	{
		if ((fp = fopen(pPath, "wb+")) == NULL)
		{
			printf("文件打开失败!\n");
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

	//返回文件中消费信息的条数
	return nIndex;
}

//更新文件中指定位置的卡信息，通过定位文件指针实现
int updateCard(Card* pCard, const char* pPath, int nIndex)
{
	FILE* fp = NULL;
	Card* plCard = NULL;
	int nLine = 0;
	long lPosition = 0; //文件位置标记

	//以读写的方式打开二进制文件, 如果文件打开失败, 则返回FALSE

	if ((fp = fopen(pPath, "rb+")) == NULL)
	{
		return FALSE;
	}
	if ((plCard = (Card*)malloc(sizeof(Card))) == NULL)
	{
		return FALSE;
	}
	//遍历文件, 找到该条信息, 并更新
	while ((!feof(fp)) && (nLine < nIndex))
	{
		if (0 != fread(plCard, sizeof(Card), 1, fp))
		{
			//获取文件标识位置
			lPosition = ftell(fp);
			nLine++;
		}
	}
	//定位到文件标识位置
	fseek(fp, lPosition, 0);
	//进行覆盖式写入
	fwrite(pCard, sizeof(Card), 1, fp);
	fclose(fp);
	return TRUE;
}