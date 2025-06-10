#pragma once
#include<stdio.h>
#include"model.h"
#ifndef _CARD_SERVICE_H_
#define _CARD_SERVICE_H_
extern lpCardNode cardList;
void releaseCardList();//¡¥±Ì Õ∑≈
int initCardList();
int getCard();
Card* doLogon(char* aNum, char* aCode);
int checkCard(char* str);
Card* quaryCard(char* str);
Card* doSettle(char* aNum, char* aCode, int* nIndex);
Card* doAddmoney(const char* name, const char* code, double* money);
Card* doReturnmoney(const char* name, const char* code, double* money);
Card* doCancel(const char* name, const char* code);
Card* queryCards(const char* pName, int* pIndex);
void findmost();
void checkcardStd();
void findunder();
void totalMoney();
void checkUser();
Card* getlucky(const char* name, const char* code,int* nIndex);
void checkdate();
#endif // !_CARD_SERVICE_H_