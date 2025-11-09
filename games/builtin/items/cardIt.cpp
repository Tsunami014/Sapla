#include "../game.hpp"
#include "../playScn.hpp"
#include "cardIt.hpp"
#include <QCursor>
#include <QGraphicsSceneHoverEvent>

CardGraphicItem::CardGraphicItem(const QString& fname, const FlashCard& flashc, QGraphicsItem* parent)
    : RectItem(parent), SvgUtils(fname), fc(flashc) {
        side = 0;
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
    if (touching(event->pos())) {
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
    if (touching(event->pos())) {
        if (side == 0) {
            unsetCursor();
            setZValue(4);
            side = 255;  // TODO: Animations
            hover = false;
        }
    }
}

void CardGraphicItem::finish() {
    PlayScene* curS = (PlayScene*)MG->curScene;
    scene()->removeItem(this);
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
