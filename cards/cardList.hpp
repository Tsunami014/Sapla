#pragma once
#include "cardTyps.hpp"

void replaceCards(std::vector<BaseCardTyp*>& newCards);
void addCard(BaseCardTyp* newCard);
void removeCard(BaseCardTyp* card);
const std::vector<BaseCardTyp*> getCards();

FlashCard* NextFC();

