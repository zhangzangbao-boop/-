#pragma once
#include<stdio.h>
#include"model.h"
#ifndef _BILLING_FILE_H_
#define _BILLING_FILE_H_
int saveBilling(Billing* pBilling, const char* pPath);
int readBillingFile(Billing* pBilling, const char* pPath);
int countBilling(const char* pPath);
int updateBilling(Billing* pBilling, const char* pPath, int nIndex);
#endif // !_BILLING_FILE_H_