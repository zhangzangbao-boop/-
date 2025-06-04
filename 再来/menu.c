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
	printf("欢迎进入计费管理系统\n---------菜单---------\n");
	printf("1.添加卡\n2.查询卡\n3.上机\n4.下机\n5.充值\n6.退费\n7.查询统计\n8.注销卡\n9.幸运抽奖\n0.退出\n");
	printf("请选择菜单项编号（0~8)：");
}
void addcard()			    	//添加卡函数
{
	Card card;
	card.nStatus = 0;			   //未上机
	card.Totaluse = 0;
	card.Usecnt = 0;
	printf("---------添加卡---------\n");
	printf("请输入卡号<长度为1~18>:");
	scanf_s("%s", &card.aNum, 20);
	if (checkCard(card.aNum))
	{
		printf("账号已存在！请重新输入:\n");
		addcard();
	}
	else
	{
		printf("请输入密码<长度为1~8>:");
		scanf_s("%s", &card.aCode, 10);
		if (count(card.aNum) > 18 || count(card.aCode) > 8)
		{
			printf("账号或密码长度不合法！请重新输入:\n");
			addcard();
		}
		else
		{
			printf("请输入开卡金额<RMB>\n<单次充值100元及以上可开通vip服务>:");
			scanf_s("%lf", &card.Balance);
			card.tStart = card.tLastuse = time(NULL);
			card.undertime = card.tStart + WEEK;
			card.tEnd = card.tStart + VALIDITY;  // 有效期 = 开卡时间 + 三年
			card.Usecnt = 0;
			card.lucy = 0;
			card.everytime1[20][20] = NULL;
			card.everytime2[20][20] = NULL;
			card.everytime3[20][20] = NULL;
			card.everytime4[20][20] = NULL;
			if (card.Balance >= 100)
			{
				card.vipgress = 1;
				card.lucy = 1;
			}
			else card.vipgress = 0;
			if (saveCard(&card, CARDPATH))   //将卡信息写入文件
			{
				printf("添加卡信息成功！\n");
				printf("卡号\t密码\t余额\t卡状态\tvip状态\t抽奖次数\n");
				printf("%s\t%s\t%.2lf\t%d\t%d\t%d\n", card.aNum, card.aCode, card.Balance, card.nStatus,card.vipgress,card.lucy);
			}
		}
	}
}

void query()
{
	printf("---------查询卡---------\n");
	int choice = 0;
	printf("请选择查询类型（1---精确查询  2---模糊查询）：");
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
		printf("请输入查询卡号<长度为1-18>：");
		scanf("%s", aName);
		pCard = queryCards(aName, &num);
		if (pCard == NULL || num == 0)
			printf("\n没有该卡的信息\n");
		else
		{
			printf("\n查询到的卡信息：\n");
			printf("卡号\t\t余额\t\t总使用金额\t使用次数\t上一次使用时间\t\t\t卡状态\tvip状态\t抽奖次数\n");
			for (int i = 0; i < num; i++)
			{
				transTime(pCard[i].tLastuse, Lastuse);  //将时间转换为字符串
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
	default: printf("输入有误！\a\n"); break;
	          } 
}

void logon()
{
	Card* pCard;
	printf("---------上机---------\n");
	char aNum[20] = { 0 };
	char aCode[10] = { 0 };
	printf("请输入卡号<长度为1~18>：");
	scanf_s("%s", aNum, 20);
	printf("请输入密码<长度为1~8>:");
	scanf_s("%s", aCode, 10);
	pCard = doLogonInfo(aNum, aCode);
	if (pCard != NULL)
	{
		if (pCard->vipgress == 1)
		{
			color(12);
			printf("上机成功！尊敬的vip用户!\n");
			char Lastuse[20];
			transTime(pCard->tLastuse, Lastuse);
			color(7);
			printf("卡号\t\t余额\t\t上机时间\n");
			color(12);
			printf("%s\t\t%.2lf\t\t%s\n", pCard->aNum, pCard->Balance, Lastuse);
			color(7);
		}
		else {
			printf("上机成功！\n");
			char Lastuse[20];
			transTime(pCard->tLastuse, Lastuse);
			printf("卡号\t\t余额\t\t上机时间\n");
			printf("%s\t\t%.2lf\t\t%s\n", pCard->aNum, pCard->Balance, Lastuse);
		}
	}
	else
	{
		printf("上机失败！\n");
	}
}

void settle()
{
	printf("---------下机---------\n");
	Card* pCard = NULL;
	char aNum[20] = { 0 };
	char aCode[10] = { 0 };
	double fAmount = 0;
	printf("请输入卡号<长度为1~18>：");
	scanf_s("%s", aNum, 20);
	printf("请输入密码<长度为1~8>:");
	scanf_s("%s", aCode, 10);
	pCard = doSettleInfo(aNum, aCode, &fAmount);
	if (pCard != NULL)
	{
		if (pCard->vipgress == 1)
		{
			color(12);
			printf("欢迎您再次光临！\n");
			color(7);
			printf("卡号\t\t余额\t\t消费金额\t抽奖次数\n");
			color(12);
			printf("%s\t\t%.2lf\t\t%.2lf\t\t%d\n", pCard->aNum, pCard->Balance, fAmount,pCard->lucy);
			color(7);
		}
		else
		{
			printf("下机成功！\n");
			printf("卡号\t\t余额\t\t消费金额\t抽奖次数\n");
			printf("%s\t\t%.2lf\t\t%.2lf\t\t%d\n", pCard->aNum, pCard->Balance, fAmount,pCard->lucy);
		}
	}
	else
	{
		printf("下机失败！\n");
	}
}

void addmoney()
{
	printf("---------充值---------\n");
	Card* pCard = NULL;
	double money = 0;
	char aNum[20] = { 0 };
	char aCode[10] = { 0 };
	printf("请输入卡号<长度为1~18>：");
	scanf_s("%s", aNum, 20);
	printf("请输入密码<长度为1~8>:");
	scanf_s("%s", aCode, 10);
	printf("请输入充值金额\n<单次充值超过100元及以上可解锁vip服务>:");
	scanf("%lf", &money);
	pCard = doAddmoney(aNum, aCode, &money);
	if (pCard != NULL&&pCard->nStatus==0)
	{
		printf("充值成功！\n");
		printf("-----充值信息如下-----\n");
		printf("卡号\t\t充值金额\t余额\n");
		printf("%s\t\t%.2lf\t\t%.2lf\n", pCard->aNum, money, pCard->Balance);
	}
	else
	{
		if (pCard->nStatus == 1)
		{
			printf("该卡正在上机！\a\n");
		}
		printf("充值失败！\a\n");
	}
}

void returnmoney()
{
	printf("---------退费---------\n");
	Card* pCard = NULL;
	double money = 0;
	char aNum[20] = { 0 };
	char aCode[10] = { 0 };
	printf("请输入卡号<长度为1~18>：");
	scanf_s("%s", aNum, 20);
	printf("请输入密码<长度为1~8>:");
	scanf_s("%s", aCode, 10);
	printf("请输入退还金额：");
	scanf("%lf", &money);
	pCard = doReturnmoney(aNum, aCode, &money);
	if (pCard != NULL)
	{
		printf("-----退费信息如下-----\n");
		printf("卡号\t\t退还金额\t余额\n");
		printf("%s\t\t%.2lf\t\t%.2lf\n", pCard->aNum, money, pCard->Balance);
	}
	else
	{
		printf("退费失败！\a\n");
	}
}

void cancel()
{
	printf("---------注销---------\n");
	Card* pCard = NULL;
	char aNum[20] = { 0 };
	char aCode[10] = { 0 };
	printf("请输入卡号<长度为1~18>：");
	scanf_s("%s", aNum, 20);
	printf("请输入密码<长度为1~8>:");
	scanf_s("%s", aCode, 10);

	pCard = doCancel(aNum, aCode);
	if (pCard != NULL)
	{
		printf("--------卡信息---------\n");
		printf("卡号\t余额\tvip状态\t抽奖次数\t\n");
		printf("%s\t%.2lf\t%d\t%d\n", pCard->aNum, pCard->Balance, pCard->vipgress, pCard->lucy);
		printf("是否确定注销该卡（1---是 2---否）：");
		int choice;
		scanf("%d", &choice);
		if (choice == 1)
		{
			printf("-----注销信息如下-----\n");
			printf("卡号\t\t退还金额\n");
			printf("%s\t\t%.2lf\n", pCard->aNum, pCard->Balance);
		}
		if (choice == 2)
		{
			printf("注销失败！\a\n");
		}
	}
	else
	{
		printf("该卡不存在！\a\n");
	}
}

void manager()
{
	printf("-------查询统计-------\n");
	printf("请选择查询项目\n1---卡状态查询\n2---活跃用户查询\n3---潜在用户查询\n4---营业额统计\n5---用户查询\n6---某时间段内消费查询\n");
	printf("输入菜单编号：");
	int choice;
	scanf("%d", &choice);
	switch (choice)
	{
	case 1: {
		checkcardStd();
	}break;
	case 2:
	{
		findmost();
	}break;

	case 3: {
		findunder();
	}
		  break;
	case 4:
	{
		totalMoney();
	}
		  break;
	case 5:
	{
		checkUser();
	}
	break;
	case 6:
	{
		checkdate();
	}
	break;
	default: printf("您输入有误！\a\n"); break;
	}
}

void luckytime()
{
	Card* pCard = NULL;
	int Index = 0;
	char aNum[20] = { 0 };
	char aCode[20] = { 0 };
	printf("------幸运抽奖------\n");
	printf("账号："); scanf("%s", aNum);
	printf("密码："); scanf("%s", aCode);
	pCard = getlucky(aNum,aCode,&Index);
	if(pCard!=NULL)
	{ 
		int c = 0;
		printf("------抽奖规则------\n");
		printf("一等奖：网费20RMB\n二等奖：网费10RMB\n三等奖：网费5RMB\n");
		printf("是否消耗1次抽奖次数进行抽奖（1---是 2---否）："); scanf("%d", &c);
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
					printf("恭喜！您中了一等奖！20元\n");
					color(7);
					pCard->Balance += 20;
				}
				if (num < 95 && num >= 80)
				{
					color(6);
					printf("恭喜!您中了二等奖！10元\n");
					color(7);
					pCard->Balance += 10;
				}
				if (num < 80 && num >= 60)
				{
					color(9);
					printf("恭喜!您中了三等奖！5元\n");
					color(7);
					pCard->Balance += 5;
				}
				if(num<60)
				{
					printf("未中奖！感谢您的参与！\n");
				}
				updateCard(pCard, CARDPATH, Index);
				printf("是否再次进行抽奖(1---是 2---否):");
				scanf("%d", &c);
			} while (c == 1&&pCard->lucy>0);
			if (c == 1 && pCard->lucy <= 0)
			{
				printf("您的抽奖次数不足！\n");
			}
		}
	}
	else
	{
			printf("该卡不存在或您无抽奖次数！\a\n");
	}
}