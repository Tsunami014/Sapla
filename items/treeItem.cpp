#include "treeItem.hpp"

QSvgRenderer* Tree::baseRend = NULL;

Tree::Tree(QGraphicsItem* parent) : SvgGraphicItem(parent) {
    if (!baseRend) baseRend = new QSvgRenderer(QStringLiteral(":/assets/treeGround.svg"));
    phase = 0;
    nextPhase();
}
Tree::~Tree() {
    delete baseRend;
}

void Tree::setRect(const QRectF& newRect) {
    qreal wid = newRect.width()*0.2;
    qreal hei = wid * 2;
    treeR = {
        newRect.width()*0.75, (newRect.height() - hei)/2, 
        wid, hei
    };
    SvgGraphicItem::setRect(newRect);
}

void Tree::nextPhase() {
    delete renderer;
    renderer = new QSvgRenderer(QString::fromStdString(
        ":/assets/TreeParts/phase" + std::to_string(++phase) + ".svg"
    ));
    isSmall = renderer->defaultSize().height() == 16;
}

void Tree::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    QRectF sTreeR{
        treeR.x(), treeR.y() + treeR.height()/2, treeR.width(), treeR.height()/2
    };
    baseRend->render(painter, sTreeR);
    QRectF drawR;
    if (isSmall) {
        drawR = sTreeR;
    } else {
        drawR = treeR;
    }
    renderer->render(painter, drawR);
}

