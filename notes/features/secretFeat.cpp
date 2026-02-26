#include "features.hpp"
#include <QRandomGenerator>

const QRegularExpression secretRe(R"((?<!\\){(.*?[^\\\n])(?:::(?<group>.*?[^\\\n]))?(?:\?\?(?<hint>.*?[^\\\n]))?})");

struct GroupContents {
    QString group;
    qsizetype start;
    qsizetype end;
    QString hint;
    QString replace;
};

class SecretFlashCard : public FlashCard {
public:
    SecretFlashCard(Note* parent,
                    const QString& front,
                    const QString& back,
                    QString title,
                    Schedule s)
        : FlashCard(parent, front, back, title, s)
    {
        orig = back;
        orig.remove(scheduleInfRe);

        uint nxtgroup = 0;
        auto it = secretRe.globalMatch(orig);
        // It is already sorted
        while (it.hasNext()) {
            auto m = it.next();
            QString group = m.captured("group");
            if (group.isNull()) {
                group = "!" + QString::number(nxtgroup++);
            } else {
                if (group[0] == '!' || group[0] == ',') {
                    group = "!" + group;
                }
            }
            QString hint = m.captured("hint");
            if (hint.isNull() || hint == "") hint = "...";
            groups.append({
                group,
                m.capturedStart(0),
                m.capturedEnd(0),
                hint,
                m.captured(1)
            });
            groupNams.insert(group);
        }
        updateDifficulty();
    }
    void update(int rating = -1) override {
        schd.update(rating);
        updateDifficulty();
    }

protected:
    QString orig;

    QList<GroupContents> groups;
    QSet<QString> groupNams;
    void updateDifficulty() {
        uint initSze = groupNams.size();
        if (initSze == 0) return; // Should never happen
        uint useAmnt = std::round(
            qBound(0.0, schd.score / ScheduleInfo.learntSco, 1.0) * (initSze - 1)
        ) + 1;
        QSet<QString> touseGroups;
        QList<QString> gs = groupNams.values();
        for (uint i = 0; i < useAmnt; i++) {
            int idx = QRandomGenerator::global()->bounded(initSze-i);
            touseGroups.insert(gs[idx]);
            // This is an efficient way to delete the item, but it destroys the order. Good thing I don't care!
            gs[idx] = std::move(gs.last()); // Move last item to idx
            gs.removeLast(); // Remove last item
        }
        front = orig;
        back = orig;
        int froffs = 0;
        int bkoffs = 0;
        for (auto g : groups) {
            bool use = touseGroups.contains(g.group);
            QString frrepl = use ? (
                "\x01" + g.hint + "\x01"
            ) : g.replace;
            int frstart = g.start + froffs;
            int frend = g.end + froffs;
            front.replace(frstart, frend - frstart, frrepl);
            froffs += frrepl.length() - (frend - frstart);

            QString bkrepl = use ? (
                "**\x01" + g.replace + "\x01**"
            ) : g.replace;
            int bkstart = g.start + bkoffs;
            int bkend = g.end + bkoffs;
            back.replace(bkstart, bkend - bkstart, bkrepl);
            bkoffs += bkrepl.length() - (bkend - bkstart);
        }
    }
};

std::vector<std::unique_ptr<FlashCard>>SecretFeat::getFlashCards(Note* parent, const QString& txt, std::map<int, Schedule> schds) const {
    if (!secretRe.match(txt).hasMatch()) {
        return {};
    }
    std::vector<std::unique_ptr<FlashCard>> result;
    result.push_back(
        std::make_unique<SecretFlashCard>(
            parent, txt, txt, getName(), getSchd(schds, 0)
        )
    );
    return result;
}
QString SecretFeat::replacements(QString& txt, Side s) const {
    if (s == SIDE_NAME) {
        return txt;
    }
    static const QRegularExpression simpleSecretRe(R"((?<!\\){.*?[^\\\n]})");
    return txt.remove(simpleSecretRe);
}
QString SecretFeat::markup(QString& line) const {
    auto it = secretRe.globalMatch(line);
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();
        QString ans = m.captured(1);
        QString group = m.captured("group");
        QString hint = m.captured("hint");
        QString repl = "<b style='color:" + col + "'>{</b>" + ans;
        if (!group.isNull()) {
            repl += "<b style='color:" + col + "'>::</b>" + group;
        }
        if (!hint.isNull()) {
            repl += "<b style='color:" + col + "'>?\?</b>" + hint;
        }
        repl += "<b style='color:" + col + "'>}</b>";
        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        line.replace(start, end - start, repl);
        offs += repl.length() - (end - start);
    }
    return line;
}
QMap<QString, QString> SecretFeat::help() const {
    return {{"Secret note\n{answer::group??hint}",
        "Hides the contents on the front and reveals on the back.\n"
        "This differs from using [[::]] as this creates a card with progressive difficulty (where it randomly hides some and over time more are hidden).\n"
        "The group and hint are optional. You can have {answer} or {answer::group} or {answer??hint} as well.\n"
        "Note templates will not work inside this."
    }};
}
