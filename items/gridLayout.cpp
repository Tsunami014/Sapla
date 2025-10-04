#include "gridLayout.hpp"
#include "cardIt.hpp"
#include "../main.hpp"

GLayoutGraphicItem::GLayoutGraphicItem(QGraphicsItem* parent)
    : QGraphicsItem(parent) {
        if (!MTrenderer) {
            MTrenderer = std::make_unique<QSvgRenderer>(QString(":/assets/CardLayouts/emptyIt.svg"));
        }
    }

void GLayoutGraphicItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    float colWid = rect.width()/Cols;
    float rowHei = rect.height()/2;
    for (int i = 0; i < Cols; i++) {
        for (int j = 0; j < 2; j++) {
            int x = rect.x() + i*colWid;
            int y = rect.y() + j*rowHei;
            bool good = true;
            for (gridItem& it : grid) {
                int itx = it.x;
                int itWid;
                if (j == 1 && it.y == 0) {
                    if (it.lay.botWid == -1) {
                        continue;
                    }
                    itx += it.lay.botOffs;
                    itWid = it.lay.botWid;
                } else {
                    itWid = it.lay.topWid;
                }
                if (i >= itx && i < itx + itWid) {
                    good = false;
                    break;
                }
            }
            if (good) {
                MTrenderer->render(painter, QRectF(x, y, colWid, rowHei));
            }
        }
    }
}

QRectF GLayoutGraphicItem::boundingRect() const {
    return rect;
}

void GLayoutGraphicItem::setRect(const QRectF& newRect) {
    prepareGeometryChange();
    rect = newRect;
    // Reduce height by 0.1 on both sides
    rect.setY(rect.y() + rect.height()*0.1);
    rect.setHeight(rect.height()*0.8);

    float colWid = rect.width()/Cols;
    float rowHei = rect.height()/2;
    for (gridItem& it : grid) {
        int hei;
        if (it.lay.botWid == -1) {
            hei = 1;
        } else {
            hei = 2;
        }
        it.item->setRect(QRectF(
            rect.x() + colWid * it.x, rect.y() + rowHei * it.y, 
            colWid * qMax(it.lay.botWid, it.lay.topWid), rowHei * hei
        ));
    }
}

bool GLayoutGraphicItem::addItem(CardGraphicItem* item) {
    uint8_t nx = 0;
    uint8_t ny = 0;
    // TODO: Find the right spot here, and return false if failed
    gridItem val;
    val.item = item;
    item->setParentItem(this);
    val.lay = item->lay;
    val.x = nx;
    val.y = ny;
    grid.push_back(val);
    MScene->addItem(item);
    setRect(rect);
    update();
    return true;
}

void GLayoutGraphicItem::removeItem(CardGraphicItem* item) {
    // <chatGPT>
    grid.erase(
        std::remove_if(grid.begin(), grid.end(),
                       [item](gridItem& it) { return it.item == item; }),
        grid.end()
    );
    // </chatGPT>
}

