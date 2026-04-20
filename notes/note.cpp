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

QString trimNL(const QString& orig) {
    QString nstr = orig;
    static const QRegularExpression re(R"(^\s*\n|\n\s*$)");
    return nstr.replace("\\\n", "").remove(re);
}

Note::Note(QString conts) {
    error = "";
    orig = conts;
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

void Note::rmCards() {
    cards.clear();
}

bool Note::isGlobal() {
    QString hidden = TemplateFeat::instance->highersReplace(orig);
    return templDefRe.match(hidden).hasMatch();
}
void Note::reset() {
    error = "";
    templates.clear();
    cards.clear();
}
bool Note::setContents(const QString& nc) {
    bool ig = isGlobal();
    orig = nc;
    bool updAll = ig || isGlobal();
    update(updAll);
    return updAll;
}
void Note::update() {
    update(isGlobal());
}
void Note::update(bool isGlobal) {
    if (isGlobal) { // Update everything if it defines a global template
        updateNoteCards();
    } else {
        reset();
        updateCards();
    }
}
void Note::updateGlobals() {
    reset();
    QString hidden = TemplateFeat::instance->highersReplace(orig);
    auto it = templDefRe.globalMatch(hidden);
    while (it.hasNext()) {
        auto m = it.next();
        QString title = m.captured("nam");
        auto [it, inserted] = globalTemplates.insert_or_assign(
            title, Template(
            m.captured("conts"), m.captured("ptn")
        ));

        if (inserted) {
            templates.push_back(title);
        } else {
            error += "Multiple global templates named `" + title + "`!\n";
            if (!it->second.failed())
                it->second = Template();
        }
    }
}
ScheduleMap Note::getSchdMap() {
    ScheduleMap scheduleMap;
    auto it = scheduleInfRe.globalMatch(orig);
    while (it.hasNext()) {
        auto m = it.next();
        for (QString part : m.captured(1).split("|")) {
            QStringList spl = part.split(",");
            if (spl.size() != 4) {
                error += "Found not 4 parts in schedule string: `" + part + "`\n";
                continue;
            }
            QStringList spl2 = spl[0].split("_");
            if (spl2.size() == 1) {
                spl2.append("0");
            } else if (spl2.size() > 2) {
                error += "Found too many parts in schedule name: `" + spl[0] + "`\n";
                continue;
            }
            bool ok;
            uint num = spl2[1].toUInt(&ok);
            if (!ok) {
                error += "Could not convert schedule number to int: `" + spl2[1] + "`\n";
                continue;
            }
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
            auto [_, inserted] = scheduleMap[spl2.join('_')].try_emplace(idx, num, idx, sco, nxtTime);
            if (!inserted) {
                error += "Schedule idx already exists: `" + spl[1] + "`\n";
            }
        }
    }
    return scheduleMap;
}
void Note::updateCards() {
    QString totconts = orig;
    cards.clear();
    for (auto& f : Feats) {
        totconts = f->check(totconts, error);
    }

    {
        tags = {};
        QString hidden = TagFeat::instance->highersReplace(totconts);
        auto it = noteInfRe.globalMatch(hidden);
        QMap<QString, bool> has;
        while (it.hasNext()) {
            auto m = it.next();
            QString txt = m.captured(1).replace("\\ ", " ");
            QString txt2 = txt.sliced(1);
            /*if (txt[0] == '!') {
            } else {*/
                tags.push_back(txt);
            //}
        }
    }

    uint i = 0;
    for (QString conts : totconts.split(mnsRe)) {
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

        for (auto& f : Feats) {
            conts = f->replacements(conts, SIDE_GETFC);
        }

        auto scheduleMap = getSchdMap();
        for (auto& f : CardFeats) {
            QString nam = f->getName()+'_'+QString::number(i);
            auto fcs = f->getFlashCards(this, conts, scheduleMap[nam], i);
            for (auto& item : fcs)
                CLaddCard(item.get());
            std::move(fcs.begin(), fcs.end(), std::back_inserter(cards));
        }
        i++;
    }
}
QString Note::preview() {
    QString conts = orig;
    for (auto& f : Feats) {
        conts = f->check(conts, error);
    }
    for (auto& f : Feats) {
        conts = f->replacements(conts, SIDE_GETFC);
    }
    return trimNL(conts);
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
        .remove(scheduleInfRe);
    QStringList infs;
    for (auto& fc : cards) {
        infs.append(fc->schd.toInf(fc->title));
    }
    conts += "\n<<"+infs.join("|")+">>";
    orig = conts;
    writeNotes();
}
void Note::removeSchedules() {
    for (auto& fc : cards) {
        fc->schd = Schedule(fc->schd.part, fc->schd.idx);
        CLrefreshCard(fc.get());
    }
    updateSchedules();
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
    if (!CLremoveCard(this)) {
        Log::Warn(MODULE) << "Unable to remove flashcard from list!";
    }
}
bool FlashCard::isAlive() { return alive; }

void FlashCard::update(int rating) {
    schd.update(rating);
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
        default:
            return "ERROR";
    }
    for (auto& f : Feats) {
        txt = f->replacements(txt, s);
    }
    return trimNL(txt);
}
QString FlashCard::getSideHtml(Side s) const {
    return parseMarkdownHtml(getSide(s));
}

bool FlashCard::operator==(const FlashCard& other) const {
    return other.parent == parent && other.front == front && other.back == back;
}

