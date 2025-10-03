#include "getCards.hpp"

std::vector<BaseCardTyp*> cards = {};

void initCards() {
    // Fill with some sample cards
    cards.push_back(new TextCard("this is the front", "this is the back"));
}

