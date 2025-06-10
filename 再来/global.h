#pragma once
#include<stdio.h>
#include"model.h"
#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#define TRUE 1
#define FALSE 0
#define CARDPATH "data\\card.txt"
#define BILLINGPATH "data\\billing.ams"
#define CARDLEN 120
#define UNIT 1
#define CHARGE 1
#define VALIDITY (3*365*24*3600)
#define WEEK (7*24*3600)

#define MIN_BALANCE 50.0 // 这里假设特定值为 50 元
#endif // _GLOBAL_H_