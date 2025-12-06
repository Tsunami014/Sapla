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

void CLclear() {
    allCards.clear();
    while (!cardQ.empty()) cardQ.pop();
}

GetFlashCard::GetFlashCard() {
    bool ptrAlive = false;
    while (!(cardQ.empty() || ptrAlive)) {
        ptr = cardQ.top();
        cardQ.pop();
        ptrAlive = ptr->isAlive();
    }
    if (!ptrAlive) {
        // This should NEVER happen, as all games should before running check if the cards list is the right size.
        if (allCards.empty()) {
            // This should REALLY never happen. If it does, it will most likely cause the game to crash!
            Log::Warn(MODULE) << "Could not find any avaliable card!";
        } else {
            ptr = allCards[QRandomGenerator::global()->bounded(int(allCards.size()))];
            Log::Debug(MODULE) << "Not enough cards avaliable, using random!";
        }
    }
    modify = ptr->isAlive();
    QString name = (ptr && ptr->isAlive()) ? ptr->getSide(SIDE_FRONT) : "null";
}
GetFlashCard::~GetFlashCard() {
    QString name = (ptr && ptr->isAlive()) ? ptr->getSide(SIDE_FRONT) : "null";
    if (modify && ptr && ptr->isAlive()) cardQ.push(ptr);
}

void GetFlashCard::updateSchedule(int rating) {
    if (ptr && ptr->isAlive()) {
        ptr->schd.update(rating);
    }
}

// Move constructor
GetFlashCard::GetFlashCard(GetFlashCard&& other) noexcept
    : ptr(other.ptr), modify(other.modify) {
        other.ptr = nullptr;
        other.modify = false;
    }
// Move assignment.
GetFlashCard& GetFlashCard::operator=(GetFlashCard&& other) noexcept {
    if (this != &other) {
        if (modify && ptr && ptr->isAlive()) {
            cardQ.push(ptr);
        }
        ptr = other.ptr;
        modify = other.modify;
        other.ptr = nullptr;
        other.modify = false;
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

