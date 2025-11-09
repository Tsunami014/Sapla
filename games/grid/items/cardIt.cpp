#include "../game.hpp"
#include "../playScn.hpp"
#include "cardIt.hpp"
#include "gridLayout.hpp"
#include "overlay.hpp"
#include <QCursor>
#include <QGraphicsSceneHoverEvent>

CardGraphicItem::CardGraphicItem(layout l, const FlashCard& flashc, QGraphicsItem* parent)
    : RectItem(parent), SvgUtils(l.fname), lay(l), fc(flashc), side(0) {
        setAcceptHoverEvents(true);
    }

bool CardGraphicItem::operator==(const CardGraphicItem& other) const {
    return fc == other.fc;
}
bool CardGraphicItem::operator==(const FlashCard& other) const {
    return fc == other;
}

void CardGraphicItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
    bool oldHover = hover;
    if (touching(event->pos()) && !((PlayScene*)MG->curScene)->hasOverlay()) {
        hover = true;
        setCursor(Qt::PointingHandCursor);
    } else {
        if (hover) {
            unsetCursor();
        }
        hover = false;
    }
    if (oldHover != hover) { update(); }
}
void CardGraphicItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    hover = false;
    update();
}
void CardGraphicItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (touching(event->pos())) {
        if (side == 0) {
            PlayScene* curS = (PlayScene*)MG->curScene;
            curS->pauseTimer();
            setParentItem(nullptr);  // So the item can be placed on the very top
            setPos(mapToScene(QPointF(0, 0)));
            setZValue(4);
            auto* overl = new Overlay();
            curS->scn.addItem(overl);
            overl->setZValue(3);
            curS->setOverlay(overl);
            side = 255;  // TODO: Animations
            unsetCursor();
            hover = true;
            setAcceptHoverEvents(false);
            curS->main->updateAllChildren();
        }
    }
}

void CardGraphicItem::finish() {
    PlayScene* curS = (PlayScene*)MG->curScene;
    curS->removeOverlay();
    curS->main->removeItem(this);
    curS->scn.removeItem(this);
    curS->main->update();
    curS->main->updateAllChildren();
    curS->resumeTimer();
}

void CardGraphicItem::paint(QPainter* p, const QStyleOptionGraphicsItem* sogi, QWidget* w) {
    paintSvg(p);

    QTextOption opts;
    opts.setAlignment(Qt::AlignHCenter);
    opts.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    if (side == 0) {
        p->drawText(rect, fc.front, opts);
    } else {
        if (side == 255) {
            p->drawText(rect, fc.back, opts);
        } else {
            // TODO: Partial flipped states
        }
    }
}
