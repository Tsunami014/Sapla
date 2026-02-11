#pragma once
#include <QSvgRenderer>
#include <QColor>

class SvgUtils {
public:
    SvgUtils(const QString& path);
    SvgUtils();

    void setSvg(const QString& path);

    virtual QRect getRect() = 0;
protected:
    bool touching(const QPointF& pos);
    void paintSvg(QPainter* p);
    void paintSvg(QPainter* p, const QRect& r);

    bool hover;
    QColor hlcol;
    QColor hoverhlcol;
    double hlopacity;
    int hlthick;
private:
    QSvgRenderer rend;
    void init();
};

