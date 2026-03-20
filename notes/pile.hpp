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
    ~PileBase() {
        if (!QApplication::closingDown()) {
            Log::Error("CardPile") << "A pile just got deleted, OH NO!!!";
        }
    }

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
    void sort() {
        if (dirty) {
            cards.erase(
                std::remove(cards.begin(), cards.end(), nullptr),
                cards.end()
            );
            std::shuffle(cards.begin(), cards.end(), *QRandomGenerator::global()); // Shuffle first
            std::sort(cards.begin(), cards.end(),
                [](const FlashCard* a, const FlashCard* b) {
                    // Other way around because we use the back as the highest
                    return a->schd.trueNxt() > b->schd.trueNxt();
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
    FlashCard* top(bool dosort = true) override {
        if (cards.empty()) return nullptr;
        if (dosort) { sort(); }
        return cards.back();
    }
    FlashCard* pop_top(bool dosort = true) override {
        if (cards.empty()) return nullptr;
        if (dosort) { sort(); }
        else { dirty = true; }
        auto card = cards.back();
        cards.pop_back();
        return card;
    }
    void push(FlashCard* card) override {
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

    size_t size() override { return cards.size(); }
    bool empty() override { return cards.empty(); }
    const std::vector<FlashCard*>& cardList() { return cards; }
protected:
    bool dirty = false;
    std::vector<FlashCard*> cards;
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
