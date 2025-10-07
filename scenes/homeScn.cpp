#include "homeScn.hpp"
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

    playBtn = new SvgBtnItem(":/assets/btn.svg", "Play!", this);
    playBtn->setTxtColour(QColor(184, 115, 51));
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
}
