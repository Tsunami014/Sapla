#include "treeItem.hpp"

#define MAX_PHASE 5

QSvgRenderer* Tree::baseRend = NULL;

Tree::Tree(QGraphicsItem* parent) : SvgGraphicItem(parent), pb(this) {
    if (!baseRend) baseRend = new QSvgRenderer(QStringLiteral(":/assets/treeGround.svg"));
    phase = 0;
    growth = 0;
    toNext = 50;
    nextPhase();
}
Tree::~Tree() {
    delete baseRend;
}

void Tree::setRect(const QRectF& newRect) {
    qreal wid = newRect.width()*0.2;
    qreal hei = wid * 2;
    treeR = {
        newRect.width()*0.75, (newRect.height()/2 - hei)/2, 
        wid, hei
    };
    SvgGraphicItem::setRect(newRect);
    pb.setRect({
        treeR.x(), treeR.y() + treeR.height()*1.1,
        treeR.width(), newRect.height()*0.04
    });
}

bool Tree::grow(double amount) {
    growth += amount;
    if (growth < 0) {
        growth = 0;
    }
    if (growth >= toNext) {
        growth -= toNext;
        if (phase+1 > MAX_PHASE) {
            return false;
        }
        nextPhase();
    }
    pb.setValue(growth/toNext);
    return true;
}
void Tree::nextPhase() {
    delete renderer;
    renderer = new QSvgRenderer(QString::fromStdString(
        ":/assets/TreeParts/phase" + std::to_string(++phase) + ".svg"
    ));
    isSmall = renderer->defaultSize().height() == 16;
    toNext = ((toNext - 50)/100 + 1)*100 + 50;
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

