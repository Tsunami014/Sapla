#include "cardList.hpp"
#include "../log.hpp"
#include <QRandomGenerator>

std::deque<FlashCard*> cards = {};
std::deque<FlashCard*> donefcs = {};

void CLaddCard(FlashCard* newCard) {
    cards.push_back(newCard);
}
void CLremoveCard(FlashCard* card) {
    auto rmIts = [&card](std::deque<FlashCard*>& dq) {
        auto it = std::find(dq.begin(), dq.end(), card);
        if (it != dq.end()) {
            dq.erase(it);
            return true;
        }
        return false;
    };

    if (rmIts(cards)) return;
    if (rmIts(donefcs)) return;

    Log::Error("CardList") << "Failed to erase card from list!";
}
std::vector<std::deque<FlashCard*>> getCardLists() {
    return {cards, donefcs};
}

FlashCard* NextFC() {
    if (cards.size() == 0) {  // If no more flashcards, shuffle done cards and use that as the input list
        // Shuffle flashcard queue
        std::random_device rd;
        std::shuffle(donefcs.begin(), donefcs.end(), std::mt19937(rd()));
        cards = donefcs;
        donefcs.clear();
    }
    // Cycle the queue of cards
    auto* fc = cards.front();
    cards.pop_front();
    donefcs.push_back(fc);
    return fc;
}

