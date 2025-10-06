#include "homeScn.hpp"
#include <QFontMetricsF>
#include <QTextOption>
#include <QTextDocument>

HomeScene::HomeScene() : BaseScene() {
    txt = new QGraphicsTextItem("Sapla", this);
    QTextOption opt = txt->document()->defaultTextOption();
    opt.setAlignment(Qt::AlignHCenter);
    txt->document()->setDefaultTextOption(opt);
}

void HomeScene::onEvent(QEvent* event) {
}

void HomeScene::resize() {
    qreal targetH = rect.height()*0.2;

    QFont font("Sans Serif");
    QFontMetricsF metrics(font);
    qreal currentH = metrics.height();
    font.setPointSizeF(font.pointSizeF() * (targetH / currentH));

    txt->setFont(font);
    txt->setTextWidth(rect.width());
}
