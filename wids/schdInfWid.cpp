#include "schdInfWid.hpp"
#include "../core.hpp"
#include "../base/font.hpp"
#include "../wids/svgBtn.hpp"
#include "../scenes/browseScn.hpp"
#include <QHBoxLayout>
#include <QMessageBox>

SchdInfoWid::SchdInfoWid(QWidget* parent) : QWidget(parent) {
    auto lay = new QHBoxLayout(this);
    auto font = getFont(1.5);
    auto font2 = getFont(1);

    txt = new QLabel(this);
    txt->setAlignment(Qt::AlignCenter);
    txt->setFont(font);
    lay->addWidget(txt);

    lay->addStretch();

    auto btn1 = new SvgBtn(":/assets/btn.svg", this);
    btn1->setPadding(20, 10);
    btn1->setFont(font2);
    btn1->setText("Unschedule");
    connect(btn1, &SvgBtn::clicked, this, [this](){
        if (n == nullptr) return;
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Are you sure?", "Are you sure you want to remove all schedules on this card?",
              QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            removeSchdls();
        }
    });
    lay->addWidget(btn1);

    auto btn2 = new SvgBtn(":/assets/btn.svg", this);
    btn2->setPadding(20, 10);
    btn2->setFont(font2);
    btn2->setText("Schedule\ninfo");
    connect(btn2, &SvgBtn::clicked, this, &SchdInfoWid::schdInfoDialog);
    lay->addWidget(btn2);

    setLayout(lay);
    chngNote(nullptr);
}

void SchdInfoWid::chngNote(Note* note) {
    n = note;
    map = ScheduleMap();
    if (n == nullptr) {
        txt->setText("");
    } else {
        auto nmap = n->getSchdMap();
        unsigned int amnt = 0;
        float totperc = 0;
        // Remove empty maps & count them all for the text
        for (const auto& [key, innerMap] : nmap) {
            amnt += innerMap.size();
            if (!innerMap.empty()) {
                map.emplace(key, innerMap);
                float perc = 0;
                for (const auto& [idx, schd] : innerMap) {
                    perc += std::clamp(schd.score / ScheduleInfo.learntSco, 0.0f, 1.0f) * 100;
                }
                totperc += perc;
            }
        }
        uint8_t outperc = std::round(
            std::clamp(totperc / amnt, 0.0f, 100.0f)
        );
        txt->setText(
            QString("%1 schedules  \n%2% done").arg(amnt).arg(outperc)
        );
    }
}

void SchdInfoWid::removeSchdls() {
    if (n == nullptr) return;
    n->removeSchedules();
    auto scn = dynamic_cast<BrowseScene*>(MG->curScene);
    if (scn) scn->selectionChange();
}
void SchdInfoWid::schdInfoDialog() {
    if (n == nullptr) return;
}
