#include "scheduleInfTxt.hpp"
#include "../core.hpp"
#include "../notes/note.hpp"
#include "../wids/svgBtn.hpp"
#include "../base/font.hpp"
#include <QTextDocument>
#include <QTextOption>
#include <QGridLayout>
#include <QApplication>

QString format_duration(Duration dur) {
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

    QString result;
    if (mo.count() > 0)result += QString::number(mo.count())+ "mo ";
    if (w.count() > 0) result += QString::number(w.count()) + "w ";
    if (d.count() > 0) result += QString::number(d.count()) + "d ";
    if (h.count() > 0) result += QString::number(h.count()) + "h ";
    if (m.count() > 0) result += QString::number(m.count()) + "m ";
    if (s.count() > 0) result += QString::number(s.count()) + "s ";

    if (result.isEmpty()) result = "0s";

    if (result.back() == ' ') result.chop(1);
    return result;
}

ScheduleInfoTxt::ScheduleInfoTxt(QGraphicsItem* parent)
    : RectItem(parent), proxy(this) { remove(); }
void ScheduleInfoTxt::remove() {
    if (last == nullptr) return;
    proxy.setWidget(nullptr);
    last = nullptr;
    setRect(rect);
}

void ScheduleInfoTxt::generate(FlashCard* card) {
    if (card == last) return;
    last = card;

    auto font = getFont(1);
    auto* container = new QWidget();
    container->setAttribute(Qt::WA_TranslucentBackground);
    container->setAutoFillBackground(false);
    container->setStyleSheet("background: transparent; color: black;");
    auto* lay = new QGridLayout();
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    unsigned int idx = 0;
    auto mkBtnBase = [=, &idx](QString title, int key) {
        auto btn = new SvgBtn(":/assets/btn2.svg", container);
        btn->setFont(font);
        btn->setText(title);
        btn->setPadding(0, 0);
        QObject::connect(btn, &SvgBtn::clicked, container, [=](){
            pressKey(key);
        });
        lay->addWidget(btn, idx / 2, idx % 2);
        idx += 1;
    };
    auto mkBtn = [=](QString txt, Duration increase, int key, QString extra="") {
        mkBtnBase(QString("<b>%1</b><br>+%2"+extra)
            .arg(txt)
            .arg(format_duration(increase))
        , key);
    };

    for (int i = 0; i < ScheduleInfo.ratesLen(); i++) {
        mkBtn(
            QString::number(i+1),
            card->schd.getUpdatedTime(i),
            Qt::Key_1 + i
        );
    }
    mkBtn(
        "-",
        ScheduleInfo.redoAmnt,
        Qt::Key_Minus,
        "<br><i>Redo</i>"
    );
    mkBtn(
        "=",
        ScheduleInfo.skipAmnt,
        Qt::Key_Equal,
        "<br><i>Skip</i>"
    );
    mkBtnBase(
        "<b>E</b><br><i>Edit</i>",
        Qt::Key_E
    );

    container->setLayout(lay);
    proxy.setWidget(container);
    setRect(rect);
}

void ScheduleInfoTxt::pressKey(int key) {
    MG->curScene->setFocus(); 

    QKeyEvent* keyPress = new QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, "");
    QApplication::sendEvent(MG->curScene, keyPress);

    QKeyEvent* keyRelease = new QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, "");
    QApplication::sendEvent(MG->curScene, keyRelease);
}

void ScheduleInfoTxt::setRect(const QRectF& newRect) {
    RectItem::setRect(newRect);
    setPos(newRect.topLeft());
    proxy.setGeometry(QRectF({0,0}, newRect.size()));
}
