#include "cardIt.hpp"
#include "../playScn.hpp"
#include "overlay.hpp"
#include <QGraphicsSceneMouseEvent>

GridCGI::GridCGI(const QString& fname, const FlashCard* flashc, QGraphicsItem* parent)
    : CardGraphicItem(fname, flashc, parent), ontop(false) {}

void GridCGI::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
    bool oldHover = hover;
    if (touching(event->pos()) && !((PlayScene*)MG->curScene)->hasOverlay()) {
        hover = true;
        setCursor(Qt::PointingHandCursor);
    } else {
        unsetCursor();
        hover = false;
    }
    if (oldHover != hover) { update(); }
}

void GridCGI::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (touching(event->pos())) {
        PlayScene* curS = (PlayScene*)MG->curScene;
        if (curS->hasOverlay()) return;
        if (side == 0 && touching(event->pos())) {
            ontop = true;
            unsetCursor();
            side = 255; // TODO: Animations
            hover = true;
            setAcceptHoverEvents(false);
            setParentItem(nullptr); // So the item can be placed on the very top
            setPos(mapToScene(QPointF(0, 0)));
            setZValue(4);
            auto* overl = new Overlay();
            curS->scn.addItem(overl);
            overl->setZValue(3);
            curS->setOverlay(overl);
            curS->main->updateAllChildren();
        }
    }
}

void GridCGI::paint(QPainter* p, const QStyleOptionGraphicsItem* sogi, QWidget* w) {
    if (side == 0) {
        txt.setText(front);
    } else {
        if (side == 255) {
            txt.setText(back);
        } else {
            // TODO: Partial flipped states
        }
    }
    paintSvg(p);

    if (ontop || !((PlayScene*)MG->curScene)->hasOverlay()) {
        const int adjustment = 40;
        QRectF newr = rect.adjusted(adjustment, adjustment, -adjustment, -adjustment);
        txt.resize(newr.size().toSize());
        p->save();
        p->translate(newr.topLeft());
        txt.render(p);
        p->restore();
    }
}

