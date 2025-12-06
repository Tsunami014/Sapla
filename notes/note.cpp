#include "note.hpp"
#include "getNotes.hpp"
#include "cardList.hpp"
#include "features.hpp"
#include "../log.hpp"
#include "../base/markdown.hpp"
#include <unordered_set>
#include <QApplication>
#include <QRegularExpression>
#include <cctype>

const QString MODULE = "Note";

std::map<QString, QString> globalTemplates;

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
        c->parent = this;
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
        c->parent = this;
    }
    other.cards.clear();
    other.templates.clear();
    return *this;
}


FlashCard* Note::getFlashCard(int idx) {
    return cards[idx].get();
}
int Note::getNumCards() {
    return cards.size();
}
int Note::getNumTemplates() {
    return templates.size();
}

void Note::reset() {
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
    std::map<QString, std::map<int, Schedule>> scheduleMap;
    auto it = scheduleInfRe.globalMatch(conts);
    while (it.hasNext()) {
        auto m = it.next();
        for (QString part : m.captured(1).split("|")) {
            QStringList spl = part.split(",");
            if (spl.size() != 4) {
                error += "Found not 4 parts in schedule string: `" + part + "`\n";
                continue;
            }
            bool ok;
            int idx = spl[1].toInt(&ok);
            if (!ok) {
                error += "Could not convert idx to int: `" + spl[1] + "`\n";
                continue;
            }
            float sco = spl[2].toFloat(&ok);
            if (!ok) {
                error += "Could not convert score to float: `" + spl[2] + "`\n";
                continue;
            }
            long long nxtTime = spl[3].toLongLong(&ok);
            if (!ok) {
                error += "Could not convert time to long long: `" + spl[3] + "`\n";
                continue;
            }
            auto schdMap = scheduleMap[spl[0]];
            auto [_, inserted] = schdMap.try_emplace(idx, idx, sco, nxtTime);
            if (!inserted) {
                error += "Schedule idx already exists: `" + spl[1] + "`\n";
            }
        }
    }
    for (auto& f : CardFeats) {
        auto fcs = f->getFlashCards(this, conts, scheduleMap[f->getName()]);
        std::move(fcs.begin(), fcs.end(), std::back_inserter(cards));
    }
    for (auto& item : cards)
        CLaddCard(item.get());
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

void Note::updateSchedules() {
    QString conts = contents()
        .replace(scheduleInfRe, "");
    QStringList infs;
    for (auto& fc : cards) {
        infs.append(fc->schd.toInf(fc->title));
    }
    conts += "\n||"+infs.join("|")+"||";
    orig = conts;
    writeNotes();
}

FlashCard::FlashCard(Note* p, const QString& fr, const QString& bk, QString t, Schedule s)
    : front(fr), back(bk), parent(p), title(t), schd(s), alive(true) {}
void FlashCard::MoveContents(FlashCard&& other) {
    // std move all but parent
    front = std::move(other.front);
    back = std::move(other.back);
    parent = other.parent;
    title = std::move(other.title);
    alive = true;
    other.alive = false;
}
FlashCard::FlashCard(FlashCard&& other) noexcept :schd(std::move(other.schd)) {
    MoveContents(std::move(other));
    other.alive = false;
}
// Move assignment.
FlashCard& FlashCard::operator=(FlashCard&& other) noexcept {
    if (this != &other) {
        MoveContents(std::move(other));
        schd = std::move(other.schd);
    }
    return *this;
}
FlashCard::~FlashCard() {
    if (!alive) return;
    alive = false;
    auto it = std::find(allCards.begin(), allCards.end(), this);
    if (it != allCards.end()) {
        allCards.erase(it);
    } else {
        Log::Error(MODULE) << "Failed to erase flashcard from list!";
    }
}
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

