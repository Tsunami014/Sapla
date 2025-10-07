#pragma once
#include "../items/base/rectIt.hpp"

class BaseScene : public RectItem {
public:
    BaseScene() : RectItem() {}
    ~BaseScene() {}
    virtual void onEvent(QEvent* event) {}
    virtual void resize() {}  // Has the updated 'rect', no need for arguments
    void setRect(const QRectF& newRect) final {
        prepareGeometryChange();
        rect = newRect;
        resize();
    }
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {}
};

