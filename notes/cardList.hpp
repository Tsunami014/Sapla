#pragma once
#include "note.hpp"
#include <deque>

void CLaddCard(FlashCard* newCard);
void CLremoveCard(FlashCard* card);
std::vector<std::deque<FlashCard*>> getCardLists();

FlashCard* NextFC();

