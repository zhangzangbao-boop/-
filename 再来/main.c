#define _CRT_SECURE_NO_WARNINGS//70
#include<stdio.h>
#include"menu.h"
#include"model.h"
#include"tool.h"
int cardCnt = 0;
extern lpCardNode cardList;
int main()
{
	int n;
	do {
		Outputmenu();
		while (scanf_s("%d", &n) != 1)
		{
			printf_s("输入菜单项序号错误，请重新输入：\n");
			while (getchar() != '\n');
		} //防止解决输入字符(串)后会进入死循环
		switch (n)
		{
		case 1:addcard(); break;
		case 2:query(); break;
		case 3:logon(); break;
		case 4:settle(); break;
		case 5: addmoney();  break;
		case 6: returnmoney(); break;
		case 7: manager(); break;
		case 8: cancel();  break;
		case 9: luckytime(); break;
		case 0: break;
		}
	} while (n != 0);
	return 0;
}