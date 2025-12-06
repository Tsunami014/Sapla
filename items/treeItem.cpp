#include "treeItem.hpp"
#include "../core.hpp"
#include "../base/svgRend.hpp"
#include "../scenes/winScn.hpp"

#define MAX_PHASE 5

QSvgRenderer* Tree::baseRend = NULL;

QByteArray rendTreePhase(int phase) {
    if (phase == -1) phase = MAX_PHASE;
    return RenderSvg(QString::fromStdString(
        ":/assets/TreeParts/phase" + std::to_string(phase) + ".svg"
    ));
}

Tree::Tree() : RectItem(), pb(this) {
    if (!baseRend) baseRend = new QSvgRenderer(RenderSvg(":/assets/treeGround.svg"));
    treeRend = nullptr;
    restart();
}
void Tree::restart() {
    phase = 0;
    growth = 0;
    toNext = 50;
    nextPhase();
}

bool Tree::isDone() {
    return phase == MAX_PHASE;
}

void Tree::setRect(const QRectF& newRect) {
    prepareGeometryChange();
    qreal hei = newRect.height()/2;
    qreal wid = (newRect.width()*0.3 + hei/2) / 2;
    rect = {
        newRect.right()-wid, newRect.y(), 
        wid, hei
    };
    pb.setRect({
        rect.x(), rect.y() + rect.height()*1.1,
        rect.width(), newRect.height()*0.04
    });
}

bool Tree::grow(double amount, bool canChangePhase) {
    if (growth + amount >= toNext && !canChangePhase) return false;
    growth += amount;
    if (growth < 0) {
        growth = 0;
    }
    if (growth >= toNext) {
        growth -= toNext;
        if (phase+1 >= MAX_PHASE) {
            MG->changeScene(new WinScene());
            restart();
            return true;
        }
        nextPhase();
    }
    pb.setValue(growth/toNext);
    return false;
}
void Tree::nextPhase() {
    if (treeRend) delete treeRend;
    treeRend = new QSvgRenderer(rendTreePhase(++phase));
    isSmall = treeRend->defaultSize().height() == 16;
    toNext = ((toNext - 50)/100 + 1)*100 + 50;

    if (phase == MAX_PHASE) pb.hide();
    update();
}

void Tree::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    // Because the svg is smaller than the bounding rect, and I really want to show them off
    QRectF fitR = rect.adjusted(-rect.width()*0.1, -rect.height()*0.1, rect.width()*0.1, rect.height()*0.1);
    QRectF sTreeR{
        fitR.x(), fitR.y() + fitR.height()/2, fitR.width(), fitR.height()/2
    };
    baseRend->render(painter, sTreeR);
    QRectF drawR;
    if (isSmall) {
        drawR = sTreeR;
    } else {
        drawR = fitR;
    }
    treeRend->render(painter, drawR);
}

