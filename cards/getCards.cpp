#include "getCards.hpp"

std::vector<BaseCardTyp*> cards = {};

void initCards() {
    // Fill with some sample cards
    cards.push_back(new TextCard("This is the front", "This is the back"));
    cards.push_back(new TextCard("This is another front", "This is another back"));
    cards.push_back(new TextCard("This is yet another front", "This is yet another back"));
    cards.push_back(new TextCard("Would you believe it; this is *another* front!", "Look at this! *Another* back!"));
}

