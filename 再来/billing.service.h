#pragma once
#include"model.h"
#include"global.h"
#ifndef _BILLING_SERVICE_H_
#define _BILLING_SERVICE_H_
int initBillingList();
void releaseBillingList();
int getBilling();
Billing* checkBilling(char* aNum, int* pIndex);
#endif // !_BILLING_SERVICE_H_