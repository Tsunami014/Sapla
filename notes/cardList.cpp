#include "cardList.hpp"
#include "../log.hpp"
#include <QRandomGenerator>
#include <queue>

const QString MODULE = "CardList";

struct CompareFC {
    bool operator()(const FlashCard* a, const FlashCard* b) const {
        return a->schd.nxt > b->schd.nxt;
    }
};

std::vector<FlashCard*> allCards;
std::priority_queue<FlashCard*, std::vector<FlashCard*>, CompareFC> cardQ;

void CLaddCard(FlashCard* newCard) {
    allCards.push_back(newCard);
    cardQ.push(newCard);
}
void CLremoveCard(FlashCard* card) {
    auto it = std::find(allCards.begin(), allCards.end(), card);
    if (it != allCards.end()) {
        allCards.erase(it);
    } else {
        Log::Error(MODULE) << "Failed to erase card from list!";
    }
}

GetFlashCard::GetFlashCard() {
    bool ptrAlive = false;
    while (!(cardQ.empty() || ptrAlive)) {
        ptr = cardQ.top();
        cardQ.pop();
        ptrAlive = ptr->isAlive();
    }
    if (!ptrAlive) {
        // This should only happen if the number of cards is less than the number of cards that the game wants to use
        ptr = allCards[QRandomGenerator::global()->bounded(int(allCards.size()))];
        Log::Debug(MODULE) << "Not enough cards avaliable, using random!";
    }
    modify = ptrAlive;
}
GetFlashCard::~GetFlashCard() {
    if (modify && ptr && ptr->isAlive()) cardQ.push(ptr);
}

GetFlashCard::GetFlashCard(GetFlashCard&& other) noexcept : ptr(other.ptr) {
    other.ptr = nullptr;
}

// Move assignment.
GetFlashCard& GetFlashCard::operator=(GetFlashCard&& other) noexcept {
    if (this != &other) {
        if (modify && ptr && ptr->isAlive()) {
            cardQ.push(ptr);
        }
        ptr = other.ptr;
        other.ptr = nullptr;
    }
    return *this;
}

GetFlashCard::operator bool() const {
    return ptr && ptr->isAlive();
}

FlashCard& GetFlashCard::operator*() const {
    if (!*this) throw std::runtime_error("Dereferencing null FlashCard");
    return *ptr;
}

