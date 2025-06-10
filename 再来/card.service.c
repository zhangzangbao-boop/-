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
	printf("截止目前，共有%d张卡已注册，有%d张卡仍然在使用中，有%d张卡已经办理注销\n", num1, num2, num3);
}

void findmost()
{
	if (getCard() == FALSE)
		return NULL;
	int flag = 0;
	lpCardNode pCur = NULL;
	if (cardList != NULL)
	{
		printf("-----活跃用户信息-----\n");
		pCur = cardList->next;

		while (pCur != NULL)
		{
			if ((pCur->data.vipgress==0&&pCur->data.Totaluse > 100)||(pCur->data.vipgress==1&&pCur->data.Totaluse>=70)&&pCur->data.nStatus!=2)
			{
				if (flag == 0)
					printf("账号\t\t总使用金额\t上次使用时间\t\tvip状态\n");  //卡号，总使用金额，上次使用时间
				flag = 1;
				char lastime[20] = { 0 };
				transTime(pCur->data.tLastuse, lastime);
				printf("%s\t\t%.2lf\t\t%s\t\t%d\n", pCur->data.aNum, pCur->data.Totaluse, lastime,pCur->data.vipgress);
			}
			pCur = pCur->next;
		}
		if (flag == 0)
		{
			printf("本系统暂无活跃用户，请用心经营！\a\n");
		}
	}
}

void findunder()
{
	if (getCard() == FALSE)
		return;

	char startTimeStr[20];
	char endTimeStr[20];

	// 提示用户输入起始时间和结束时间
	int c;
	while ((c = getchar()) != '\n' && c != EOF) {}

	printf("请输入起始时间 (格式: YYYY-MM-DD HH:MM): ");
	fgets(startTimeStr, sizeof(startTimeStr), stdin);
	size_t len = strlen(startTimeStr);
	if (len > 0 && startTimeStr[len - 1] == '\n')
		startTimeStr[len - 1] = '\0';

	printf("请输入结束时间 (格式: YYYY-MM-DD HH:MM): ");
	fgets(endTimeStr, sizeof(endTimeStr), stdin);
	len = strlen(endTimeStr);
	if (len > 0 && endTimeStr[len - 1] == '\n')
		endTimeStr[len - 1] = '\0';

	// 将输入的时间字符串转换为 time_t 类型
	time_t startTime = strToTime(startTimeStr);
	time_t endTime = strToTime(endTimeStr);

	// 时间合法性校验
	if (startTime == -1 || endTime == -1 || startTime > endTime) {
		printf("输入的时间不合法，请检查格式和范围！\n");
		return;
	}

	int flag = 0;
	lpCardNode pCur = NULL;
	if (cardList != NULL)
	{
		printf("-----潜在用户信息-----\n");
		pCur = cardList->next;

		while (pCur != NULL)
		{
			// 检查上次使用时间是否在指定时间段内
			if (pCur->data.tLastuse >= startTime && pCur->data.tLastuse <= endTime &&
				(((pCur->data.tLastuse < pCur->data.undertime) && (pCur->data.Totaluse > 50)) ||
					(pCur->data.vipgress == 1 && pCur->data.nStatus != 2)))
			{
				if (flag == 0)
					printf("账号\t\t总使用金额\t上次使用时间\t\tvip状态\n");  //卡号，总使用金额，上次使用时间,vip
				flag = 1;
				char lastime[20] = { 0 };
				transTime(pCur->data.tLastuse, lastime);
				printf("%s\t\t%.2lf\t\t%s\t\t%d\n", pCur->data.aNum, pCur->data.Totaluse, lastime, pCur->data.vipgress);
			}
			pCur = pCur->next;
		}
		if (flag == 0)
		{
			printf("本系统在指定时间段内暂无潜在用户，请用心经营！\a\n");
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
void checkUser()
{
	if (getCard() == FALSE)
	{
		return NULL;
	}
	char aName[20] = {0};
	int i;
	int flag = 0;
	printf("请输入您要查询的账户:");
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
					printf("用户名为：%s的卡信息如下\n", aName);
					printf("上机时间\t\t\t下机时间\t\t\t使用金额\n");
					for (i = 0; i < pCur->data.Usecnt; i++)
					{
						printf("%s\t\t%s\t\t%.2lf\n", pCur->data.everytime1[i], pCur->data.everytime2[i], pCur->data.everymoney[i]);
					}
					printf("该用户总使用次数为%d,账户余额为%.2lf\n", pCur->data.Usecnt, pCur->data.Balance);
					if (pCur->data.nStatus == 2)
						printf("该用户已注销！\n");
				}
				else
				{
					printf("该用户暂无使用记录\n");
				}
			}
			pCur = pCur->next;
		}
		if (flag == 0)
		{
			printf("未查找到相关用户信息!\a\n");
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
	printf("请输入您要查询的开始日期（格式为YYYY-MM-DD）:");
	scanf("%s", Btime);
	printf("请输入您要查询的结束日期（格式为YYYY-MM-DD）:");
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
				printf("账号\t消费\n");
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
			printf("该时间段内无用户!\n");
		}
	}
	if (flag1 != 0)
	{
		printf("总消费金额为：%.2lf\n", all);
	}
}