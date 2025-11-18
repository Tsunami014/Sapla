#include "note.hpp"
#include "cardList.hpp"
#include "features.hpp"
#include "../base/markdown.hpp"
#include <unordered_set>
#include <QApplication>

Note::Note(QString conts) {
    setContents(conts);
}
Note::~Note() {
    if (!QApplication::closingDown()) {  // If it is closing down this will fail
        ridCards();
    }
}
bool Note::operator==(const Note& other) const {
    return orig == other.orig;
}
// Copy constructor
Note::Note(const Note& other) : orig(other.orig) {
    // Deep-copy cards
    cards = other.cards;
    for (auto& c : cards) {
        c.parent = this;
        CLaddCard(&c);
    }
}
// Copy assignment
Note& Note::operator=(const Note& other) {
    if (this == &other) return *this;
    // Unregister existing
    ridCards();
    orig = other.orig;
    cards = other.cards;
    for (auto& c : cards) {
        c.parent = this;
        CLaddCard(&c);
    }
    return *this;
}
// Move constructor
Note::Note(Note&& other) noexcept : orig(std::move(other.orig)),
                                    cards(std::move(other.cards)) {
    for (auto& c : cards) {
        c.parent = this;
    }
    other.cards.clear();
}
// Move assignment
Note& Note::operator=(Note&& other) noexcept {
    if (this == &other) return *this;
    ridCards();
    orig = std::move(other.orig);
    cards = std::move(other.cards);
    for (auto& c : cards) {
        c.parent = this;
    }
    other.cards.clear();
    return *this;
}


int Note::getNumCards() {
    return cards.size();
}

void Note::ridCards() {
    for (auto& item : cards)
        CLremoveCard(&item);

    cards.clear();
}
void Note::setContents(QString nc) {
    orig = nc;
    ridCards();
    for (auto& f : Feats) {
        if (auto fcs = f->getFlashCards(this, nc)) {
            std::move(fcs->begin(), fcs->end(), std::back_inserter(cards));
        }
    }
    for (auto& item : cards)
        CLaddCard(&item);
}
QString Note::contents() const {
    return orig;
}
QString Note::title() {
    return orig.simplified();
}

FlashCard::FlashCard(Note* p, const QString& fr, const QString& bk) : front(fr), back(bk) {
    parent = p;
}
QString FlashCard::getSide(Side s) const {
    QString txt;
    switch (s) {
        case SIDE_FRONT:
            txt = front;
            break;
        case SIDE_BACK:
            txt = back;
            break;
    }
    for (auto& f : Feats) {
        txt = f->replacements(txt, s);
    }
    return parseMarkdownHtml(txt);
}

bool FlashCard::operator==(const FlashCard& other) const {
    return other.parent == parent && other.front == front && other.back == back;
}

