#include "../game.hpp"
#include "../playScn.hpp"
#include "cardIt.hpp"
#include "gridLayout.hpp"
#include "overlay.hpp"
#include <QCursor>
#include <QGraphicsSceneHoverEvent>

CardGraphicItem::CardGraphicItem(layout l, BaseSideRend* fr, BaseSideRend* bk, QGraphicsItem* parent)
    : RectItem(parent), SvgUtils(l.fname), lay(l), side(0) {
        setAcceptHoverEvents(true);
        front = fr;
        back = bk;
    }
CardGraphicItem::CardGraphicItem(layout l, const FlashCard& fc, QGraphicsItem* parent)
    : RectItem(parent), SvgUtils(l.fname), lay(l), side(0) {
        setAcceptHoverEvents(true);
        front = fc.getSide(SIDE_FRONT);
        back = fc.getSide(SIDE_BACK);
    }
CardGraphicItem::~CardGraphicItem() {
    delete front;
    delete back;
}

bool CardGraphicItem::operator==(const CardGraphicItem& other) const {
    return *front == *other.front && *back == *other.back;
}
bool CardGraphicItem::operator==(const FlashCard& other) const {
    CardGraphicItem nCGI(lay, other);
    return nCGI == *this;
}

void CardGraphicItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
    bool oldHover = hover;
    if (contains(event->pos()) && !((PlayScene*)MG->curScene)->hasOverlay()) {
        hover = true;
        setCursor(Qt::PointingHandCursor);
    } else {
        if (hover) {
            unsetCursor();
        }
        hover = false;
    }
    if (oldHover != hover) { update(); }
    RectItem::hoverMoveEvent(event);
}
void CardGraphicItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    hover = false;
    update();
}
void CardGraphicItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (contains(event->pos())) {
        if (side == 0) {
            PlayScene* curS = (PlayScene*)MG->curScene;
            curS->pauseTimer();
            unsetCursor();
            setParentItem(nullptr);  // So the item can be placed on the very top
            setPos(mapToScene(QPointF(0, 0)));
            setZValue(4);
            auto* overl = new Overlay();
            curS->scn.addItem(overl);
            overl->setZValue(3);
            curS->setOverlay(overl);
            side = 255;  // TODO: Animations
            hover = false;
            curS->main->updateAllChildren();
        }
    }
}

void CardGraphicItem::finish() {
    PlayScene* curS = (PlayScene*)MG->curScene;
    curS->removeOverlay();
    curS->main->removeItem(this);
    curS->main->update();
    scene()->removeItem(this);
    curS->resumeTimer();
    curS->main->updateAllChildren();
}

void CardGraphicItem::paint(QPainter* p, const QStyleOptionGraphicsItem* sogi, QWidget* w) {
    paintSvg(p);

    if (side == 0) {
        front->render(p, rect);
    } else {
        if (side == 255) {
            back->render(p, rect);
        } else {
            // TODO: Partial flipped states
        }
    }
}
