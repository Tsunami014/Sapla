#include "../game.hpp"
#include "../playScn.hpp"
#include "cardIt.hpp"
#include "gridLayout.hpp"
#include "overlay.hpp"
#include <QCursor>
#include <QGraphicsSceneHoverEvent>

CardGraphicItem::CardGraphicItem(layout l, const FlashCard& flashc, QGraphicsItem* parent)
    : RectItem(parent), SvgUtils(l.fname), lay(l), side(0), fc(flashc), onTop(false),
      front(fc.getSide(SIDE_FRONT)), back(fc.getSide(SIDE_BACK)), txt() {
        setAcceptHoverEvents(true);
        txt.setTextFormat(Qt::RichText);
        txt.setWordWrap(true);
        txt.setAlignment(Qt::AlignCenter);
        txt.setStyleSheet("color: black;");
        txt.setAttribute(Qt::WA_TranslucentBackground);
        txt.setFont(getFont(1.5));
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
        unsetCursor();
        hover = false;
    }
    if (oldHover != hover) { update(); }
}
void CardGraphicItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    hover = false;
    update();
}
void CardGraphicItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    PlayScene* curS = (PlayScene*)MG->curScene;
    if (curS->hasOverlay()) return;
    if (side == 0 && touching(event->pos())) {
        onTop = true;
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

void CardGraphicItem::finish() {
    PlayScene* curS = (PlayScene*)MG->curScene;
    curS->removeOverlay();
    curS->main->removeItem(this);
    curS->scn.removeItem(this);
    curS->main->update();
    curS->main->updateAllChildren();
}

void CardGraphicItem::paint(QPainter* p, const QStyleOptionGraphicsItem* sogi, QWidget* w) {
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

    if (onTop || !((PlayScene*)MG->curScene)->hasOverlay()) {
        txt.resize(rect.size().toSize());
        p->save();
        p->translate(rect.topLeft());
        txt.render(p);
        p->restore();
    }
}
