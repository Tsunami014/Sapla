#include "cardList.hpp"
#include "pile.hpp"
#include "../log.hpp"
#include <QRandomGenerator>

const QString MODULE = "CardList";

std::vector<FlashCard*> allCards;

const unsigned int maxCurPileWeight = 6; // new cards - if a card is known well, the curpile can hold more cards (up to double this)

Pile otherpile{}; // Cards that aren't in the curpile
CurPile curpile{};

std::vector<_progressVal> getProgresses() {
    unsigned int cardsln = allCards.size();
    unsigned int news = 0;
    unsigned int pracs = 0;
    unsigned int learns = 0;
    for (auto* fc : allCards) {
        if (fc->schd.isNew()) news++;
        else if (fc->schd.score < ScheduleInfo.notnewSco) pracs++;
        else if (fc->schd.score < ScheduleInfo.learntSco) learns++;
    }
    return {
        {"Revising", cardsln-(news+pracs+learns)},
        {"Learning", learns},
        {"New", pracs},
        {"Unseen", news},
    };
}
_overallProgr getOverallProgress() {
    unsigned int cardsln = allCards.size();
    float completes = 0;
    for (auto* fc : allCards) {
        // Add percentage of the way through the card
        completes += fc->schd.percentage();
    }
    return { cardsln, completes };
}

const std::vector<FlashCard*>& CardList(bool sorted) {
    return allCards;
}

void resetCurPile() {
    curpile.clear();
}

size_t CLlen() {
    return allCards.size();
}

void addCard(FlashCard* newCard) {
    otherpile.push(newCard);
}
void CLaddCard(FlashCard* newCard) {
    allCards.push_back(newCard);
    addCard(newCard);
}

void CLclear() {
    allCards.clear();
    otherpile.clear();
    curpile.clear();
}
bool CLremoveCard(FlashCard* card) {
    {
        auto it = std::find(allCards.begin(), allCards.end(), card);
        if (it != allCards.end()) {
            allCards.erase(it);
        } else {
            return false;
        }
    }
    return
        otherpile.erase(card) ||
        curpile.erase(card)
    ;
}

void CLrefreshCard(FlashCard* card) {
    otherpile.erase(card) ||
    curpile.erase(card)
    ;
    addCard(card);
}

double activeWeight = 0;
void refreshCurPile() {
    while (1) {
        if (otherpile.empty()) break;
        double newWeight = cardweight(otherpile.top());
        if (curpile.weight() + newWeight >= maxCurPileWeight - activeWeight) {
            break;
        }
        curpile.push(otherpile.pop_top());
    }
}

GetFlashCard::GetFlashCard() {
    bool ptrAlive = false;
    refreshCurPile();
    while (!ptrAlive && !curpile.empty()) {
        ptr = curpile.pop_top();
        ptrAlive = ptr && ptr->isAlive();
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
    } else {
        activeWeight += cardweight(ptr);
    }
    modify = ptr->isAlive();
}
GetFlashCard::~GetFlashCard() {
    finish();
}

void GetFlashCard::updateSchedule(int rating) {
    if (ptr && ptr->isAlive()) {
        ptr->update(rating);
    }
}
void GetFlashCard::finish() {
    if (modify && ptr && ptr->isAlive()) {
        if (ptr->schd.score >= ScheduleInfo.leaveSco) {
            otherpile.push(ptr);
        } else {
            curpile.push(ptr);
        }
        activeWeight -= cardweight(ptr);
    }
    modify = false;
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
        finish();
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
