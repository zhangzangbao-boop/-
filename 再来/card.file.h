#pragma once
#include"model.h"
#include"global.h"
#ifndef CARD_FILE_H_
#define CARD_FILE_H_
int saveCard(Card* pCard, const char* pPath);
int countCard(const char* pPath);
int readFile(Card* pCard, const char* pPath);
int updateCard(Card* pCard, const char* pPath, int nIndex);
#endif // !CARD_FILE_H_