#include "cardList.hpp"
#include <QRandomGenerator>
#include <deque>

std::vector<BaseCardTyp*> cards = {};
std::deque<FlashCard*> fcs = {};
std::deque<FlashCard*> doneFcs = {};

void CLreplaceCards(std::vector<BaseCardTyp*>& newCs) {
    for (auto* c : cards) delete c;
    cards = newCs;
    fcs.clear();  // All pointers should be to the cards anyway
    doneFcs.clear();
    for (auto* c : cards) {
        for (auto& fc : c->flashCs) {
            doneFcs.push_back(&fc);
        }
    }
}
void CLaddCard(BaseCardTyp* newCard) {
    cards.push_back(newCard);
    for (auto& fc : newCard->flashCs) {
        doneFcs.push_back(&fc);
    }
}
void CLremoveCard(BaseCardTyp* card) {
    auto it = std::find(cards.begin(), cards.end(), card);
    for (auto& fc : card->flashCs) {
        fcs.erase(std::remove(fcs.begin(), fcs.end(), &fc), fcs.end());
        doneFcs.erase(std::remove(doneFcs.begin(), doneFcs.end(), &fc), doneFcs.end());
    }
    if (it != cards.end()) {
        delete *it;
        cards.erase(it);
    }
}
const std::vector<BaseCardTyp*> getCards() {
    return cards;
}

FlashCard* NextFC() {
    if (fcs.size() == 0) {  // If no more flashcards, shuffle done fcs and use that as the input list
        // Shuffle flashcard queue
        std::random_device rd;
        std::shuffle(doneFcs.begin(), doneFcs.end(), std::mt19937(rd()));
        fcs = doneFcs;
        doneFcs.clear();
    }
    // Cycle the queue of cards
    auto* fc = fcs.front();
    fcs.pop_front();
    doneFcs.push_back(fc);
    return fc;
}

