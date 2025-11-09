#include "../game.hpp"
#include "../playScn.hpp"
#include "cardIt.hpp"
#include <QCursor>
#include <QGraphicsSceneHoverEvent>

CardGraphicItem::CardGraphicItem(const QString& fname, const FlashCard& flashc, QGraphicsItem* parent)
    : RectItem(parent), SvgUtils(fname), side(0), fc(flashc),
      front(parseMarkdownHtml(fc.front)), back(parseMarkdownHtml(fc.back)), txt() {
        side = 0;
        setAcceptHoverEvents(true);
        txt.setTextFormat(Qt::RichText);
        txt.setWordWrap(true);
        txt.setAlignment(Qt::AlignCenter);
        txt.setMargin(4);
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

    txt.resize(rect.size().toSize());
    p->save();
    p->translate(rect.topLeft());
    txt.render(p);
    p->restore();
}
