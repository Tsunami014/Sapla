#pragma once

#include <QRandomGenerator>
#include <QApplication>
#include <deque>
#include "note.hpp"
#include "../log.hpp"


double cardweight(FlashCard* card) {
    return (2.0f - card->schd.percentage()) / 2.0f;
}

class PileBase {
public:
    virtual ~PileBase() = default;

    virtual double weight() = 0;
    virtual void clear() = 0;
    virtual FlashCard* top(bool dosort = true) = 0;
    virtual FlashCard* pop_top(bool dosort = true) = 0;
    virtual void push(FlashCard* card) = 0;
    virtual bool erase(FlashCard* card) = 0;
    virtual size_t size() = 0;
    virtual bool empty() = 0;
};

class Pile : public PileBase {
public:
    Pile() : cards() {}
    virtual void sort(bool dosort = true) {
        cards.erase(
            std::remove(cards.begin(), cards.end(), nullptr),
            cards.end()
        );
        if (dosort && dirty && !cards.empty()) {
            std::shuffle(cards.begin(), cards.end(), *QRandomGenerator::global()); // Shuffle first
            TimePoint now = std::chrono::system_clock::now();
            std::sort(cards.begin(), cards.end(),
                [now](const FlashCard* a, const FlashCard* b) {
                    // Other way around because we use the back as the highest
                    return a->schd.trueNxt(now) > b->schd.trueNxt(now);
                });
            dirty = false;
        }
    }

    double weight() override {
        double sum = 0;
        for (auto* c : cards) {
            sum += cardweight(c);
        }
        return sum;
    }

    void clear() override { cards.clear(); }
    virtual FlashCard* top(bool dosort = true) override {
        sort(dosort);
        if (cards.empty()) return nullptr;
        return cards.back();
    }
    virtual FlashCard* pop_top(bool dosort = true) override {
        sort(dosort);
        if (cards.empty()) return nullptr;
        if (!dosort) { dirty = true; }
        auto card = cards.back();
        cards.pop_back();
        return card;
    }
    virtual void push(FlashCard* card) override {
        dirty = true;
        cards.push_back(card);
    }
    bool erase(FlashCard* card) override {
        auto it = std::find(cards.begin(), cards.end(), card);
        if (it != cards.end()) {
            cards.erase(it);
            dirty = true;
            return true;
        }
        return false;
    }

    size_t size() override {
        sort(false);
        return cards.size();
    }
    bool empty() override {
        sort(false);
        return cards.empty();
    }
    const std::vector<FlashCard*>& cardList() { return cards; }
protected:
    bool dirty = false;
    std::vector<FlashCard*> cards;
};

class RandPile : public Pile {
public:
    void sort(bool dosort = true) override {
        cards.erase(
            std::remove(cards.begin(), cards.end(), nullptr),
            cards.end()
        );
        if (dosort && dirty && !cards.empty()) {
            std::shuffle(cards.begin(), cards.end(), *QRandomGenerator::global());
            dirty = false;
        }
    }
};
class BacklogPile : public Pile {
public:
    FlashCard* top(bool dosort = true) override {
        sort(dosort);
        if (cards.empty()) return nullptr;
        for (auto it = cards.rbegin(); it != cards.rend(); ++it) {
            if (QRandomGenerator::global()->bounded(2)) { // 50% chance
                return *it;
            }
        }
        return cards.front();
    }
};

class CurPile : public PileBase {
public:
    CurPile() : cards() {}

    void sort() {
        cards.erase(
            std::remove(cards.begin(), cards.end(), nullptr),
            cards.end()
        );
    }

    double weight() override {
        double sum = 0;
        for (auto* c : cards) {
            sum += cardweight(c);
        }
        return sum;
    }

    void clear() override { cards.clear(); }

    FlashCard* top(bool dosort = true) override {
        if (cards.empty()) return nullptr;
        return cards.back();
    }

    FlashCard* pop_top(bool dosort = true) override {
        if (cards.empty()) return nullptr;
        if (dosort) sort();
        else dirty = true;
        auto card = cards.back();
        cards.pop_back();
        return card;
    }

    void push(FlashCard* card) override {
        dirty = true;
        cards.push_front(card);
        if (cards.size() < 2) return;
        if (QRandomGenerator::global()->bounded(2)) {
            auto last = cards.end() - 1;
            auto secondLast = cards.end() - 2;
            std::iter_swap(last, secondLast);
        }
    }

    bool erase(FlashCard* card) override {
        auto it = std::find(cards.begin(), cards.end(), card);
        if (it != cards.end()) {
            cards.erase(it);
            dirty = true;
            return true;
        }
        return false;
    }

    size_t size() override { return cards.size(); }
    bool empty() override { return cards.empty(); }
    const std::deque<FlashCard*>& cardList() { return cards; }

protected:
    bool dirty = false;
    std::deque<FlashCard*> cards;
};
