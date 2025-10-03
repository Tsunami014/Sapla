#pragma once
#include <QGraphicsItem>

class BaseScene : public QGraphicsItem {
public:
    virtual ~BaseScene() {}
    virtual void resize() = 0;  // Has the updated 'rect', no need for arguments
    virtual void setRect(const QRectF& newRect) final {
        prepareGeometryChange();
        rect = newRect;
        resize();
    }
    QRectF boundingRect() const override final {
        return rect;
    }
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {}

    QRectF rect{0, 0, 800, 600};  // default size, will be overridden
};

