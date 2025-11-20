#include "note.hpp"
#include "cardList.hpp"
#include "features.hpp"
#include "../log.hpp"
#include "../base/markdown.hpp"
#include <unordered_set>
#include <QApplication>
#include <QRegularExpression>

const QString MODULE = "Note";

std::map<QString, QString> globalTemplates = {};

Note::Note(QString conts) {
    setContents(conts);
}
Note::~Note() {
    if (!QApplication::closingDown()) {  // If it is closing down this will fail
        reset();
    }
}
bool Note::operator==(const Note& other) const {
    return orig == other.orig;
}
// Move constructor
Note::Note(Note&& other) noexcept : orig(std::move(other.orig)),
                                    cards(std::move(other.cards)),
                                    templates(std::move(other.templates)) {
    for (auto& c : cards) {
        c.parent = this;
    }
    other.cards.clear();
    other.templates.clear();
}
// Move assignment
Note& Note::operator=(Note&& other) noexcept {
    if (this == &other) return *this;
    orig = std::move(other.orig);
    cards = std::move(other.cards);
    templates = std::move(other.templates);
    for (auto& c : cards) {
        c.parent = this;
    }
    other.cards.clear();
    other.templates.clear();
    return *this;
}


int Note::getNumCards() {
    return cards.size();
}
int Note::getNumTemplates() {
    return templates.size();
}

void Note::reset() {
    for (auto& item : cards)
        CLremoveCard(&item);

    cards.clear();
    for (auto& t : templates) {
        globalTemplates.erase(t);
    }
    templates.clear();
}
void Note::setContents(const QString& nc) {
    orig = nc;
    reset();
    auto it = templDefRe.globalMatch(orig);
    while (it.hasNext()) {
        auto m = it.next();
        QString title = m.captured(1);
        globalTemplates[title] = m.captured(2);
        templates.push_back(title);
    }
}
void Note::updateCards() {
    QString conts = orig;
    {
        auto it = templDefRe.globalMatch(orig);
        int offs = 0;
        while (it.hasNext()) {
            auto m = it.next();
            int start = m.capturedStart(0) + offs;
            int end = m.capturedEnd(0) + offs;
            conts.replace(start, end - start, "");
            offs -= (end - start);
        }
    } {
        auto it = templApplyRe.globalMatch(orig);
        int offs = 0;
        while (it.hasNext()) {
            auto m = it.next();

            QString name = m.captured(1);
            if (globalTemplates.find(name) == globalTemplates.end()) {
                Log::Warn(MODULE) << "Unknown template name: " << name;
                continue;
            }
            QString templ = globalTemplates[name];
            QStringList gs = m.capturedTexts().sliced(2);
            for (auto& g : gs) {
                templ = templ.arg(g);
            }

            int start = m.capturedStart(0) + offs;
            int end = m.capturedEnd(0) + offs;
            conts.replace(start, end - start, templ);
            offs += templ.length() - (end - start);
        }
    }
    std::vector<QString> dominants;
    for (auto& f : Feats) {
        if (f->dominance(conts)) {
            dominants.push_back(f->getName());
        }
    }
    if (dominants.size() > 1) {
        auto logger = Log::Warn(MODULE);
        logger << "Found multiple dominant features on note where there should only be one: " << dominants.back();
        dominants.pop_back();
        for (auto& d : dominants) {
            logger << ", " << d;
        }
        return;
    }
    for (auto& f : Feats) {
        if (auto fcs = f->getFlashCards(this, conts)) {
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

