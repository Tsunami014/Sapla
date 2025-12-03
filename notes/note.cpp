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
    error = "";
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


FlashCard* Note::getFlashCard(int idx) {
    return &cards[idx];
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
    {
        auto it = templDefRe.globalMatch(orig);
        while (it.hasNext()) {
            auto m = it.next();
            QString title = m.captured(1);
            globalTemplates[title] = m.captured(2);
            templates.push_back(title);
        }
    }
}
QStringList parseCommas(QString inp) {
    QStringList out; QString cur;
    bool esc = false;

    for (QChar c : inp) {
        if (esc) {
            cur += c;
            esc = false;
        } else if (c == '\\') {
            esc = true;
        } else if (c == ',') {
            out.append(cur.trimmed());
            cur.clear();
        } else {
            cur += c;
        }
    }

    if (!cur.isEmpty()) {
        out.append(cur);
    }
    return out;
}
void Note::updateCards() {
    error = "";
    prio = 0;
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
        auto it = templApplyRe.globalMatch(conts);
        int offs = 0;
        while (it.hasNext()) {
            auto m = it.next();

            QString name = m.captured(1);
            if (globalTemplates.find(name) == globalTemplates.end()) {
                error += "Unknown template name: " + name + "\n";
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
    } {
        auto it = noteInfRe.globalMatch(orig);
        QMap<QString, bool> has;
        while (it.hasNext()) {
            auto m = it.next();
            QString title = m.captured(1);
            if (title == "tag" || title == "tags") {
                if (has.value("tag", false)) {
                    tags.clear();
                    error += "Cannot have multiple @tag:...@!\n";
                } else {
                    tags = parseCommas(m.captured(2));
                    has["tag"] = true;
                }
            } else if (title == "priority" || title == "prio") {
                if (has.value("prio", false)) {
                    prio = 0;
                    error += "Cannot have multiple @priority:...@!\n";
                } else {
                    bool ok;
                    int value = m.captured(2).toInt(&ok);
                    if (ok) {
                        prio = value;
                    } else {
                        error += "Priority could not be converted to an int: `" + m.captured(2) + "`\n";
                    }
                    has["prio"] = true;
                }
            } else {
                error += "Unknown note info title: `" + title + "`\n";
            }
        }
    }
    std::vector<QString> dominants;
    for (auto& f : CardFeats) {
        if (f->dominance(conts)) {
            dominants.push_back(f->getName());
        }
    }
    if (dominants.size() > 1) {
        error += "Found multiple dominant features on note where there should only be one: `" + dominants.back();
        dominants.pop_back();
        for (auto& d : dominants) {
            error += "`, `" + d;
        }
        error += "`\n";
        return;
    }
    for (auto& f : CardFeats) {
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
    QString n = orig;
    for (auto& f : Feats) {
        n = f->replacements(n, SIDE_NAME);
    }
    return n.simplified();
}

FlashCard::FlashCard(Note* p, const QString& fr, const QString& bk, Schedule s)
    : front(fr), back(bk), parent(p), schd(s), alive(true) {}
FlashCard::~FlashCard() { alive = false; }
bool FlashCard::isAlive() { return alive; }

QString FlashCard::getSide(Side s) const {
    QString txt;
    switch (s) {
        case SIDE_NAME:
            return "ERROR";
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
    return txt;
}
QString FlashCard::getSideHtml(Side s) const {
    return parseMarkdownHtml(getSide(s));
}

bool FlashCard::operator==(const FlashCard& other) const {
    return other.parent == parent && other.front == front && other.back == back;
}


Schedule Schedule::blank() {
    return {0, {}};
}
void Schedule::update(int rating) {
    
}

