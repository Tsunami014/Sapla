#include "cardList.hpp"
#include "../log.hpp"
#include <QRandomGenerator>
#include <queue>

const QString MODULE = "CardList";

std::vector<FlashCard*> allCards;

class Pile {
public:
    Pile() : cards() {}
    ~Pile() {
        Log::Error(MODULE) << "A pile just got deleted, OH NO!!!";
    }
    void sort() {
        if (dirty) {
            std::sort(cards.begin(), cards.end(),
                [](const FlashCard* a, const FlashCard* b) {
                    if (a->schd.nxt == b->schd.nxt) {
                        return static_cast<bool>(
                            QRandomGenerator::global()->bounded(2)
                        );
                    }
                    // Other way around because we use the back as the highest
                    return a->schd.nxt > b->schd.nxt;
                });
            dirty = false;
        }
    }

    void clear() { cards.clear(); }
    FlashCard* top(bool dosort = true) {
        if (cards.empty()) return nullptr;
        if (dosort) { sort(); }
        return cards.back();
    }
    FlashCard* pop_top(bool dosort = true) {
        if (cards.empty()) return nullptr;
        if (dosort) { sort(); }
        else { dirty = true; }
        auto card = cards.back();
        cards.pop_back();
        return card;
    }
    void push(FlashCard* card) {
        dirty = true;
        cards.push_back(card);
    }
    bool erase(FlashCard* card) {
        auto it = std::find(cards.begin(), cards.end(), card);
        if (it != cards.end()) {
            cards.erase(it);
            dirty = true;
            return true;
        }
        return false;
    }

    size_t size() { return cards.size(); }
    bool empty() { return cards.empty(); }
    const std::vector<FlashCard*>& cardList() { return cards; }
private:
    bool dirty = false;
    std::vector<FlashCard*> cards;
};


const unsigned int maxCurPileLen = 20;

Pile newpile{};
Pile learnpile{};
Pile curpile{};

std::vector<_progressVal> getProgresses() {
    unsigned int cardsln = allCards.size();
    unsigned int news = 0;
    unsigned int pracs = 0;
    unsigned int learns = 0;
    for (auto* fc : allCards) {
        if (fc->isnew()) news++;
        else if (fc->schd.score < ScheduleInfo.leaveSco) pracs++;
        else if (fc->schd.score < ScheduleInfo.learntSco) learns++;
    }
    return {
        {"Revising", cardsln-(news+pracs+learns)},
        {"Learning", learns},
        {"Current", pracs},
        {"New", news},
    };
}
_overallProgr getOverallProgress() {
    unsigned int cardsln = allCards.size();
    float completes = 0;
    for (auto* fc : allCards) {
        // Add percentage of the way through the card
        completes += std::min(fc->schd.score / ScheduleInfo.learntSco, 1.0f);
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
    if (newCard->isnew()) {
        newpile.push(newCard);
    } else {
        learnpile.push(newCard);
    }
}
void CLaddCard(FlashCard* newCard) {
    allCards.push_back(newCard);
    addCard(newCard);
}

void CLclear() {
    allCards.clear();
    newpile.clear();
    learnpile.clear();
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
        newpile.erase(card) ||
        learnpile.erase(card) ||
        curpile.erase(card)
    ;
}

void CLrefreshCard(FlashCard* card) {
    newpile.erase(card) ||
    learnpile.erase(card) ||
    curpile.erase(card)
    ;
    addCard(card);
}

int active = 0;
void refreshCurPile() {
    static bool newnext = true;
    while (curpile.size() < maxCurPileLen - active) {
        int left = 2;
        if (newnext && newpile.empty()) {
            newnext = false;
            left--;
        }
        if (!newnext && (
            learnpile.empty() || (!newpile.empty() && (
                learnpile.top()->schd.score >= ScheduleInfo.learntSco &&
                !learnpile.top()->schd.dueNow()
            ))
        )) {
            newnext = true;
            left--;
        }
        if (left == 0) {
            break;
        }
        if (newnext) {
            curpile.push(newpile.pop_top());
        } else {
            curpile.push(learnpile.pop_top());
        }
        newnext = !newnext;
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
        active++;
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
            learnpile.push(ptr);
        } else {
            curpile.push(ptr);
        }
        active--;
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
