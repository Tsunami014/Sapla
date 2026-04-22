#include "deckProgress.hpp"
#include "../notes/cardList.hpp"
#include "../base/font.hpp"
#include "../core.hpp"
#include <QFontMetrics>

DeckProgress::DeckProgress(QGraphicsItem* parent)
    : RectItem(parent), pb(this), txt(this) {
        txt.setFont(getFont(1.25));
        initialupd();
        txt.setTransformOriginPoint(txt.boundingRect().topLeft());
        txt.setRotation(90);
    }

void DeckProgress::setRect(const QRectF& newRect) {
    prepareGeometryChange();
    qreal yoffs = newRect.height()*0.02;
    qreal hei = newRect.height()-yoffs*2;
    qreal xspace = newRect.width()*0.02;
    QFontMetrics metrics(txt.font());
    qreal wid = metrics.height();
    rect = {
        newRect.left()+xspace, yoffs, 
        (wid+xspace)*2, hei
    };
    pb.setRect({
        rect.x(), yoffs,
        wid, hei
    });
    txt.setPos(rect.x()+wid+xspace*2, rect.y());
}

void DeckProgress::upd() {
    auto progrs = getProgresses();
    std::vector<float> nums;
    for (auto p : progrs) {
        nums.push_back(p.cur);
    }
    pb.setValues(nums);
    update();
    pb.update();
}
void DeckProgress::initialupd() {
    auto progrs = getProgresses();
    std::vector<float> nums;
    QStringList outTxts;
    size_t maxSze = MG->styls.pbcols.size()-3;
    unsigned int idx = 0;
    for (auto p : progrs) {
        nums.push_back(p.cur);
        if (!p.label.isEmpty()) {
            outTxts.append(
                "<b style='color:" + MG->styls.pbcols[idx + 3].name(QColor::HexRgb) + "'>"
                + p.label + "</b>"
            );
        }
        idx = (idx + 1) % maxSze;
    }
    pb.setValues(nums);
    pb.update();
    txt.setHtml(outTxts.join("  "));
}
