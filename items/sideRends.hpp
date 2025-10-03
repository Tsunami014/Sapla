#pragma once
#include <QPainter>
class BaseSideRend {
public:
    virtual ~BaseSideRend() {};
    virtual void render(QPainter* painter, QRectF rect) = 0;
    virtual void renderSelected(QPainter* painter, QRectF rect) = 0;
};

class TextSide : public BaseSideRend {
public:
    TextSide(QString txt) : text(txt) {};
    void render(QPainter* painter, QRectF rect) override;
    void renderSelected(QPainter* painter, QRectF rect) override;
private:
    QString text;
};

