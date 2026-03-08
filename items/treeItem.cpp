#include "treeItem.hpp"
#include "../core.hpp"
#include "../base/svgRend.hpp"
#include "../scenes/winScn.hpp"

#define MAX_PHASE 5

_treeInfo::_treeInfo() {
    restart();
}

void _treeInfo::restart() {
    phase = 0;
    growth = 0;
    nextPhase();
}

bool _treeInfo::isDone() {
    return phase == MAX_PHASE;
}

bool _treeInfo::grow(double amount, bool canChangePhase) {
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
    return false;
}
void _treeInfo::nextPhase() {
    toNext = 150 + (++phase)*100;
}

float _treeInfo::percentage() {
    return growth/toNext;
}

int _treeInfo::getPhase() {
    return phase;
}

_treeInfo TreeInfo{};


QByteArray rendTreePhase(int phase) {
    if (phase == -1) phase = MAX_PHASE;
    return RenderSvg(QString::fromStdString(
        ":/assets/TreeParts/phase" + std::to_string(phase) + ".svg"
    ));
}

Tree::Tree(QGraphicsItem* parent) : RectItem(parent), pb(this) {
    baseRend = new QSvgRenderer(RenderSvg(":/assets/treeGround.svg"));
    treeRend = nullptr;
    pb.setValue(TreeInfo.percentage());
    nextPhase(false);
}
Tree::~Tree() {
    if (baseRend) baseRend->deleteLater();
    if (treeRend) treeRend->deleteLater();
}

void Tree::restart() {
    TreeInfo.restart();
    nextPhase(false);
}

void Tree::setRect(const QRectF& newRect) {
    prepareGeometryChange();
    qreal hei = newRect.height()/3;
    qreal wid = (newRect.width()*0.2 + hei/2) / 2;
    rect = {
        newRect.right()-wid*1.05, newRect.y(),
        wid, hei
    };
    pb.setRect({
        rect.left(), rect.bottom() + newRect.height()*0.1,
        rect.width(), newRect.height()*0.04
    });
}

QRectF Tree::getRect() {
    auto pbRec = pb.boundingRect();
    return {rect.topLeft(), pbRec.bottomRight()};
}

bool Tree::grow(double amount, bool canChangePhase) {
    bool ret = TreeInfo.grow(amount, canChangePhase);
    if (!ret) {
        pb.setValue(TreeInfo.percentage());
    }
    return ret;
}
void Tree::nextPhase(bool updateInfo) {
    if (treeRend) delete treeRend;
    if (updateInfo) TreeInfo.nextPhase();
    int phase = TreeInfo.getPhase();
    treeRend = new QSvgRenderer(rendTreePhase(phase));
    isSmall = treeRend->defaultSize().height() == 16;

    if (phase == MAX_PHASE) pb.hide();
    update();
}
void Tree::nextPhase() {
    nextPhase(true);
}

void Tree::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    // Because the svg is smaller than the bounding rect, and I really want to show them off
    QRectF fitR = rect.adjusted(-rect.width()*0.1, -rect.height()*0.2, rect.width()*0.1, rect.height()*0.2);
    QRectF sTreeR{
        fitR.x(), fitR.y() + fitR.height()/2, fitR.width(), fitR.height()/2
    };
    baseRend->render(painter, sTreeR);
    QRectF drawR = isSmall ? sTreeR : fitR;
    treeRend->render(painter, drawR);
}

