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

void timetostr(time_t a, char* s)   //将时间转化为年月日
{
	struct tm* timeinfo;
	timeinfo = localtime(&a);
	strftime(s, 20, "%Y-%m-%d", timeinfo);
}
lpCardNode cardList = NULL;

int initCardList()//链表初始化函数
{
	lpCardNode pHead = NULL;
	//内存分配
	if ((pHead = (lpCardNode)malloc(sizeof(CardNode))) == NULL)
	{
		printf("链表初始化失败！\n");
		return FALSE;
	}
	else
	{
		//逐条建立链表
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
void releaseCardList()//链表清除函数
{
	lpCardNode pCur = NULL;
	lpCardNode pNext = NULL;
	if (cardList != NULL)
	{
		//头节点不存储信息，先指向下一节点
		pCur = cardList->next;
		//保存当前节点的下一节点再对当前节点进行释放
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
	//获得卡信息数量
	int cardCnt = 0;
	if ((cardCnt = countCard(CARDPATH)) == 0)
	{
		return FALSE;
	}
	int i = 0;
	Card* pCard = NULL;
	//分配内存
	pCard = (Card*)malloc(sizeof(Card) * cardCnt);
	if (pCard == NULL)
	{
		printf("堆区内存已经用完！\n");
		exit(1);
	}
	//读取文件中卡信息
	if (readFile(pCard, CARDPATH) == FALSE)
	{
		printf("文件读取失败！");
		return FALSE;
	}
	//定义链表头节点，当前节点
	lpCardNode node = NULL;
	lpCardNode pCur = NULL;
	//如果链表不为空则清空链表
	if (cardList != NULL)
	{
		releaseCardList();
	}
	//初始化链表
	initCardList();
	node = cardList;
	//逐条建立链表
	//尾插法实现
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
			node = node->next;// 遍历链表找到尾节点
		}
		node->next = pCur; // 插入新节点到尾部
	}
	//释放结构体数组内存
	free(pCard);
	pCard = NULL;
	return TRUE;
}
Card* doLogon(char* aNum, char* aCode)
{
	int nIndex = 0;
	// 加载卡信息并判断是否成功
	if (getCard() == FALSE)
	{
		return NULL;
	}
	lpCardNode pCur = NULL;
	if (cardList != NULL)
	{
		pCur = cardList->next;
		// 遍历链表查找卡号
		while (pCur != NULL)
		{
			if (strcmp(pCur->data.aNum, aNum) == 0)
			{
				// 需要未注销
				if (pCur->data.nStatus != 2)
				{
					// 密码需要输入正确
					if (strcmp(pCur->data.aCode, aCode) != 0)
					{
						printf("密码输入错误\n");
						return NULL;
					}
					// 余额需要足够
					if (pCur->data.Balance < MIN_BALANCE)
					{
						printf("卡内余额低于 %.2lf 元，请充值后再上机。\n", MIN_BALANCE);
						return NULL;
					}
					// 需要处于未上机状态
					if (pCur->data.nStatus == 1)
					{
						printf("该卡已上机\n");
						return NULL;
					}
					// 修改卡上机状态和使用时间
					pCur->data.nStatus = 1;
					pCur->data.Usecnt++;
					pCur->data.tLastuse = time(NULL);
					// 判断是否成功将修改后的卡信息保存到文件
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
	printf("该卡号不存在。\n");
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
			if (strcmp(pCur->data.aNum, aNum) == 0) // 找到匹配卡号
			{
				if (pCur->data.nStatus != 2)  // 卡未注销
				{
					if (strcmp(pCur->data.aCode, aCode) != 0)
					{
						printf("密码错误！\n");
						return NULL;
					}
					if (pCur->data.nStatus == 0)
					{
						printf("该卡未上机！\n");
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
	printf("该账号不存在！\n");
	return NULL;
}

Card* doAddmoney(const char* name, const char* code, double* money)
{
	int index = 0;
	if (getCard() == FALSE)  //读取卡片信息失败
	{
		return NULL;
	}

	//遍历卡片列表查找匹配卡号
	lpCardNode pCur = NULL;
	if (cardList != NULL)
	{
		pCur = cardList->next;
		while (pCur != NULL)
		{
			if (strcmp(pCur->data.aNum, name) == 0)//找到匹配卡号
			{
				if (strcmp(pCur->data.aCode, code) != 0)
				{
					printf("输入的密码错误！\a\n");
					return NULL;
				}
				else
				{
					if (pCur->data.nStatus == 2)
					{
						printf("该卡已注销！\a");

					} 
					else
					{
						//执行充值操作
						pCur->data.Balance += *money;
						if (*money >= 100&&pCur->data.vipgress==0)
						{
							pCur->data.vipgress = 1;
						}
						//保存更新到文件
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
		printf("该账号不存在！\a\n");
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
					printf("输入的密码错误！\a\n");
					return NULL;
				}
				else
				{
					if (pCur->data.nStatus == 2)
					{
						printf("该卡已注销！\a\n");
						return NULL;
					}
					else
					{
						if (pCur->data.nStatus == 1)
						{
							printf("该卡正在使用，请先下机！\a\n");
							return NULL;
						}
						else
						{
							if (pCur->data.Balance < *money)
							{
								printf("该卡余额不足！\a\n");
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
		printf("该账号不存在！\a\n");
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
					printf("输入的密码错误！\a\n");
					return NULL;
				}
				else
				{
					if (pCur->data.nStatus == 1)
					{
						printf("该卡正在使用，请先下机！\a\n");
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
		printf("该账号不存在！\a\n");
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

	// 提示用户输入统计的年月
	int c;
	while ((c = getchar()) != '\n' && c != EOF) {}

	printf("请输入要统计的年月 (格式: YYYY-MM): ");
	fgets(yearMonthStr, sizeof(yearMonthStr), stdin);
	size_t len = strlen(yearMonthStr);
	if (len > 0 && yearMonthStr[len - 1] == '\n')
		yearMonthStr[len - 1] = '\0';

	// 解析输入的年月
	if (sscanf(yearMonthStr, "%d-%d", &year, &month) != 2) {
		printf("输入的格式不正确，请使用 YYYY-MM 格式！\n");
		return;
	}

	// 计算该月的起始时间和结束时间
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
		printf("----- %d 年 %d 月充值退费明细 -----\n", year, month);
		printf("账号\t\t操作类型\t金额\t操作时间\n");
		pCur = cardList->next;

		while (pCur != NULL)
		{
			for (int i = 0; i < pCur->data.Usecnt; i++) {
				time_t opTime = strToTime(pCur->data.everytime1[i]);
				if (opTime >= startTime && opTime < endTime) {
					if (pCur->data.everymoney[i] > 0) {
						// 充值操作
						totalRecharge += pCur->data.everymoney[i];
						printf("%s\t\t充值\t\t%.2lf\t%s\n", pCur->data.aNum, pCur->data.everymoney[i], pCur->data.everytime1[i]);
						flag = 1;
					}
					else if (pCur->data.everymoney[i] < 0) {
						// 退费操作
						totalRefund += -pCur->data.everymoney[i];
						printf("%s\t\t退费\t\t%.2lf\t%s\n", pCur->data.aNum, -pCur->data.everymoney[i], pCur->data.everytime1[i]);
						flag = 1;
					}
				}
			}
			pCur = pCur->next;
		}

		if (flag == 0)
		{
			printf("本系统在指定时间段内暂无充值退费记录，请继续关注！\a\n");
		}
		else
		{
			printf("----- %d 年 %d 月充值退费汇总 -----\n", year, month);
			printf("充值总金额: %.2lf 元\n", totalRecharge);
			printf("退费总金额: %.2lf 元\n", totalRefund);
		}
	}
}
void totalMoney() {
	int choice;
	printf("请选择统计类型：1 - 按年统计；2 - 按月统计：");
	scanf("%d", &choice);

	int year, month;
	if (choice == 1) {
		printf("请输入要统计的年份：");
		scanf("%d", &year);
		month = 0; // 按年统计时，月份设为 0
	}
	else if (choice == 2) {
		printf("请输入要统计的年份和月份（格式：YYYY MM）：");
		scanf("%d %d", &year, &month);
	}
	else {
		printf("输入无效，请重新选择。\n");
		return;
	}

	// 按年（月）统计开卡情况
	if (getCard() == FALSE) {
		return;
	}
	lpCardNode pCur = cardList->next;
	int openCardCount = 0;
	double totalRecharge = 0;

	printf("\n%d年", year);
	if (month != 0) {
		printf("%d月", month);
	}
	printf("开卡情况统计表\n");
	// 使用固定宽度格式化输出表头
	printf("%-10s %-20s %-10s\n", "卡号", "开卡时间", "首次充值金额");

	while (pCur != NULL) {
		struct tm* tm_info;
		tm_info = localtime(&pCur->data.tStart);
		if (tm_info->tm_year + 1900 == year && (month == 0 || tm_info->tm_mon + 1 == month)) {
			char start_time[20];
			transTime(pCur->data.tStart, start_time);
			// 使用固定宽度格式化输出每条记录
			printf("%-10s %-20s %.2lf\n", pCur->data.aNum, start_time, pCur->data.Balance);
			openCardCount++;
			totalRecharge += pCur->data.Balance;
		}
		pCur = pCur->next;
	}
	printf("合计\t开卡数：%d\t金额：%.2lf\n", openCardCount, totalRecharge);

	// 按年（月）统计上机下机消费情况
	if (getBilling() == FALSE) {
		return;
	}
	lpBillingNode pBillingCur = BillingList->next;
	double totalConsumption = 0;

	printf("\n%d年", year);
	if (month != 0) {
		printf("%d月", month);
	}
	printf("上机下机消费情况统计表\n");
	// 使用固定宽度格式化输出表头
	printf("%-10s %-20s %-20s %-10s\n", "卡号", "上机时间", "下机时间", "消费金额");

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
			// 使用固定宽度格式化输出每条记录
			printf("%-10s %-20s %-20s %.2lf\n", pBillingCur->data.aNum, start_time, end_time, pBillingCur->data.fAmount);
			totalConsumption += pBillingCur->data.fAmount;
		}
		pBillingCur = pBillingCur->next;
	}
	printf("合计\t%.2lf\n", totalConsumption);
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

