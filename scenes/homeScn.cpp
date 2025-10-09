#include "homeScn.hpp"
#include "../main.hpp"
#include "playScn.hpp"
#include "browseScn.hpp"
#include <QFontMetricsF>
#include <QTextOption>
#include <QTextDocument>

HomeScene::HomeScene() : BaseScene() {
    txt = new QGraphicsTextItem(this);
    txt->setHtml("<b>Sapla</b>");
    txt->setDefaultTextColor(QColor(149, 69, 53));
    QTextOption opt = txt->document()->defaultTextOption();
    opt.setAlignment(Qt::AlignHCenter);
    txt->document()->setDefaultTextOption(opt);

    playBtn = new TxtBtnItem(":/assets/btn.svg", "Play!", this);
    playBtn->setTxtColour(QColor(184, 115, 51));
    playBtn->onClick = []() {
        MG->changeScene(new PlayScene());
    };
    browseBtn = new TxtBtnItem(":/assets/btn.svg", "Browse cards", this);
    browseBtn->setTxtColour(QColor(184, 115, 51));
    browseBtn->onClick = []() {
        MG->changeScene(new BrowseScene());
    };
}

QFont getFont(qreal targetH) {
    QFont font;
    font.setFamilies({"DejaVu Sans", "Ubuntu", "Segoe UI", "Arial", "Helvetica"});
    QFontMetricsF metrics(font);
    qreal currentH = metrics.height();
    font.setPointSizeF(font.pointSizeF() * (targetH / currentH));
    return font;
}

void HomeScene::resize() {
    txt->setFont(getFont(rect.height()*0.2));
    txt->setTextWidth(rect.width());

    playBtn->setFont(getFont(rect.height()*0.1));
    playBtn->setPos(QPointF(
        (rect.width() - playBtn->boundingRect().width())/2, rect.height() * 0.35
    ));
    browseBtn->setFont(getFont(rect.height()*0.1));
    browseBtn->setPos(QPointF(
        (rect.width() - browseBtn->boundingRect().width())/2, rect.height() * 0.36 + playBtn->boundingRect().height()
    ));
}
