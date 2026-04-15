#include "cardList.hpp"
#include "getNotes.hpp"
#include "pile.hpp"
#include "../log.hpp"
#include <QRandomGenerator>

const QString MODULE = "CardList";

std::vector<FlashCard*> allCards;

const uint maxCurPileWeight = 6; // new cards - if a card is known well, the curpile can hold more cards (up to double this)
// 8 revising then 5 new etc.
const uint otherCards = 8;
const uint newCards = 5;

RandPile newpile{};
BacklogPile otherpile{}; // Cards that aren't new
CurPile curpile{};
double activeWeight = 0; // Weight of cards from curpile that were temporarily removed for display

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
    activeWeight = 0;
}

size_t CLlen() {
    return allCards.size();
}

void addCard(FlashCard* newCard) {
    if (newCard->schd.isNew()) {
        newpile.push(newCard);
    } else {
        otherpile.push(newCard);
    }
}
void CLaddCard(FlashCard* newCard) {
    allCards.push_back(newCard);
    addCard(newCard);
}

void CLclear() {
    for (auto* n : notesL) {
        n->rmCards();
    }
    allCards.clear();
    newpile.clear();
    otherpile.clear();
    curpile.clear();
    activeWeight = 0;
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
        newpile.erase(card) ||
        otherpile.erase(card) ||
        curpile.erase(card)
    ;
}

void CLrefreshCard(FlashCard* card) {
    newpile.erase(card) ||
    otherpile.erase(card) ||
    curpile.erase(card)
    ;
    addCard(card);
}

void refreshCurPile() {
    static uint i = 0;
    static uint state = 1;
    bool empty = false;
    while (1) {
        uint mx = state == 0 ? newCards : otherCards;
        Pile* pile = state == 0 ?
            static_cast<Pile*>(&newpile) : static_cast<Pile*>(&otherpile);
        if (pile->empty()) {
            if (empty) {
                break;
            } else {
                empty = true;
                i = 0;
                state = 1 - state;
                continue;
            }
        }
        if (!empty && i >= mx) { // If the other pile is empty don't switch to it and just keep going
            i = 0;
            state = 1 - state;
            continue;
        }
        double newWeight = cardweight(pile->top());
        if (curpile.weight() + newWeight >= maxCurPileWeight - activeWeight) {
            break;
        }
        curpile.push(pile->pop_top());
        i++;
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
    }
    if (ptr && ptr->isAlive()) {
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
