#include "layout.hpp"

LayoutGraphicItem::LayoutGraphicItem(QGraphicsItem* parent)
    : QGraphicsItem(parent) {
        if (!MTrenderer) {
            MTrenderer = std::make_unique<QSvgRenderer>(QString(":/assets/FCLayouts/emptyIt.svg"));
        }
    }

void LayoutGraphicItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    float colWid = rect.width()/Cols;
    float rowHei = rect.height()/2;
    for (int i = 0; i < Cols; i++) {
        for (int j = 0; j < 2; j++) {
            int x = i*colWid;
            int y = j*rowHei;
            MTrenderer->render(painter, QRectF(x, y, colWid, rowHei));
        }
    }
}

QRectF LayoutGraphicItem::boundingRect() const {
    return rect;
}

void LayoutGraphicItem::setRect(const QRectF& newRect) {
    prepareGeometryChange();
    rect = newRect;

    float colWid = newRect.width()/Cols;
    float rowHei = newRect.height()/2;
    for (int i = 0; i < Cols; i++) {
        for (int y = 0; y < 2; y++) {
        }
    }
}

bool LayoutGraphicItem::addItem(QGraphicsItem* item, layout lay) {
    uint8_t nx = 0;
    uint8_t ny = 0;
    // TODO: Find the right spot here, and return false if failed
    gridItem val;
    val.item = item;
    val.lay = lay;
    val.x = nx;
    val.y = ny;
    grid.push_back(val);
    return true;
}

