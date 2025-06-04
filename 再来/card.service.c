#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>
#include"global.h"
#include"card.file.h"
#include"card.service.h"
#include"tool.h"

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
			node = node->next;
		}
		node->next = pCur;
	}
	//释放结构体数组内存
	free(pCard);
	pCard = NULL;
	return TRUE;
}
Card* doLogon(char* aNum, char* aCode)
{
	int nIndex = 0;
	//建立链表并判断是否成功
	if (getCard() == FALSE)
	{
		return NULL;
	}
	lpCardNode pCur = NULL;
	if (cardList != NULL)
	{
		pCur = cardList->next;
		//遍历链表
		while (pCur != NULL)
		{
			if (strcmp(pCur->data.aNum, aNum) == 0)
			{
				//卡要未注销
				if (pCur->data.nStatus != 2)
				{
					//卡密码需要输入正确
					if (strcmp(pCur->data.aCode, aCode) != 0)
					{
						printf("密码错误！\n");
						return NULL;
					}
					//卡余额要大于零
					if (pCur->data.Balance <= 0)
					{
						printf("余额不足！\n");
						return NULL;
					}
					//卡要处于未上机状态
					if (pCur->data.nStatus == 1)
					{
						printf("该卡正在上机！\n");
						return NULL;
					}
					//修改卡上机状态，开始计时
					pCur->data.nStatus = 1;
					pCur->data.Usecnt++;
					pCur->data.tLastuse = time(NULL);
					//判断是否将修改后的卡信息成功存入文件
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
	printf("该账号不存在！\n");
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
			if (strcmp(pCur->data.aNum, aNum) == 0)
			{
				if (pCur->data.nStatus != 2)
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
						printf("该卡已注销！\a");

					}
					else
					{
						pCur->data.Balance += *money;
						if (*money >= 100&&pCur->data.vipgress==0)
						{
							pCur->data.vipgress = 1;
						}
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
		return NULL;
	int flag = 0;
	lpCardNode pCur = NULL;
	if (cardList != NULL)
	{
		printf("-----潜在用户信息-----\n");
		pCur = cardList->next;

		while (pCur != NULL)
		{
			if (((pCur->data.tLastuse<pCur->data.undertime)&&(pCur->data.Totaluse>50))||pCur->data.vipgress==1&&pCur->data.nStatus!=2)
			{
				if (flag == 0)
					printf("账号\t\t总使用金额\t上次使用时间\t\tvip状态\n");  //卡号，总使用金额，上次使用时间,vip
				flag = 1;
				char lastime[20] = { 0 };
				transTime(pCur->data.tLastuse, lastime);
				printf("%s\t\t%.2lf\t\t%s\t\t%d\n", pCur->data.aNum, pCur->data.Totaluse, lastime,pCur->data.vipgress);
			}
			pCur = pCur->next;
		}
		if (flag == 0)
		{
			printf("本系统暂无潜在用户，请用心经营！\a\n");
		}
	}
}

void totalMoney()
{
	double num1 = 0, num2=0;
	double max;
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
			num1 += pCur->data.Totaluse;
			num2 += pCur->data.Totaluse + pCur->data.Balance;
			pCur = pCur->next;
		}
		printf("营业以来，共有用户充值%.2lf元，共有用户使用%.2lf元\n", num2, num1);
	}
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