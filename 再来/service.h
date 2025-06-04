#pragma once
#pragma once
#include"model.h"
#include"global.h"
#ifndef _SERVICE_H_
#define _SERVICE_H_
Card* doLogonInfo(char* aNum, char* aCode);
double getAmount(time_t tStart,int vip);
void releaseList();
Card* doSettleInfo(char* aNum, char* aCode, double* fAmount);
#endif