#include "scheduleInfTxt.hpp"
#include "../notes/note.hpp"
#include <QTextDocument>
#include <QTextOption>

std::string format_duration(Duration dur) {
    using namespace std::chrono;

    auto mo = duration_cast<months>(dur);
    dur -= mo;
    auto w = duration_cast<weeks>(dur);
    dur -= w;
    auto d = duration_cast<days>(dur);
    dur -= d;
    auto h = duration_cast<hours>(dur);
    dur -= h;
    auto m = duration_cast<minutes>(dur);
    dur -= m;
    auto s = duration_cast<seconds>(dur);

    std::string result;
    if (mo.count() > 0)result += std::to_string(mo.count())+ "mo ";
    if (w.count() > 0) result += std::to_string(w.count()) + "w ";
    if (d.count() > 0) result += std::to_string(d.count()) + "d ";
    if (h.count() > 0) result += std::to_string(h.count()) + "h ";
    if (m.count() > 0) result += std::to_string(m.count()) + "m ";
    if (s.count() > 0) result += std::to_string(s.count()) + "s ";

    if (result.empty()) result = "0s";

    if (result.back() == ' ') result.pop_back();
    return result;
}

QGraphicsTextItem* getScheduleInfTxt(FlashCard* card, QGraphicsItem* parent) {
    auto* gti = new QGraphicsTextItem(parent);
    styleScheduleInfTxt(gti);
    setScheduleInfTxt(gti, card);
    return gti;
}
void styleScheduleInfTxt(QGraphicsTextItem* it) {
    it->document()->setDefaultTextOption(
        QTextOption(Qt::AlignCenter)
    );
    it->setDefaultTextColor(Qt::black);
}
void setScheduleInfTxt(QGraphicsTextItem* it, FlashCard* card) {
    QString txt = "<h2>";

    bool doneRedo = false;
    QString redoTxt = QString("<b>-</b> Redo +%1<br>").arg(format_duration(ScheduleInfo.redoAmnt));
    bool doneSkip = false;
    QString skipTxt = QString("<b>=</b> Skip +%1<br>").arg(format_duration(ScheduleInfo.skipAmnt));
    for (int i = 0; i < ScheduleInfo.ratesLen(); i++) {
        auto time = card->schd.getUpdatedTime(i);
        if (!doneRedo && time > ScheduleInfo.redoAmnt) {
            txt += redoTxt;
            doneRedo = true;
        }
        if (!doneSkip && time > ScheduleInfo.skipAmnt) {
            txt += skipTxt;
            doneSkip = true;
        }
        txt += "<b>" + std::to_string(i+1) + "</b>: +" + format_duration(time) + "<br>";
    }
    if (!doneRedo) txt += redoTxt;
    if (!doneSkip) txt += skipTxt;
    txt += "</h2>";

    it->setHtml(
        txt
    );
}
