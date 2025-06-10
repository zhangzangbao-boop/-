#pragma once
#include"model.h"
#include"global.h"
#ifndef _BILLING_SERVICE_H_
#define _BILLING_SERVICE_H_
int initBillingList();
void releaseBillingList();
int getBilling();
Billing* checkBilling(char* aNum, int* pIndex);
void statisticsByYearMonth(int year, int month);//7.6按年（月）统计上机下机消费情况
#endif // !_BILLING_SERVICE_H_