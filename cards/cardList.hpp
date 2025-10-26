#pragma once
#include "cardTyps.hpp"

void CLreplaceCards(std::vector<BaseCardTyp*>& newCards);
void CLaddCard(BaseCardTyp* newCard);
void CLremoveCard(BaseCardTyp* card);
const std::vector<BaseCardTyp*> getCards();

FlashCard* NextFC();

