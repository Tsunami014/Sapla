#pragma once
#include <QPainter>

#define RENDER_FUNC void render(QPainter* painter, QRectF& rect) override;

class BaseFormElm {
public:
    virtual void render(QPainter* painter, QRectF& rect);
    static int heightRatio;
    void* get() { return NULL; }
};

class LabelFormElm : public BaseFormElm {
public:
    LabelFormElm(QString txt) : text(txt) {}
    inline static int heightRatio = 1;
    QString* get() { return &text; }
RENDER_FUNC
private:
    QString text;
};

