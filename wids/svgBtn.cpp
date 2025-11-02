#include "svgBtn.hpp"
#include <QPainter>
#include <QTextDocument>

SvgBtn::SvgBtn(const QString& pth, QWidget* parnt)
    : QAbstractButton(parnt), rend(new QSvgRenderer(pth, this)) {
        setCursor(Qt::PointingHandCursor);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    }

const int hpad = 16;
const int vpad = 8;
int SvgBtn::heightForWidth(int w) const {
    int innerW = qMax(1, w - 2 * hpad);

    QFont f = font();
    f.setBold(true);
    QTextDocument doc;
    doc.setDefaultFont(f);
    doc.setTextWidth(innerW);
    doc.setPlainText(text());
    QTextOption opt;
    opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    opt.setAlignment(Qt::AlignCenter);
    doc.setDefaultTextOption(opt);

    return int(std::ceil(doc.size().height())) + 2 * vpad;
}
QSize SvgBtn::sizeHint() const {
    QFont f = font();
    f.setBold(true);
    QFontMetrics fm(f);

    int w = fm.horizontalAdvance(text()) + 2 * hpad;
    return {w, heightForWidth(w)};
}

void SvgBtn::paintEvent(QPaintEvent*) {
    QPainter p(this);

    if (isDown()) p.setOpacity(0.7);
    else if (underMouse()) p.setOpacity(0.9);
    else p.setOpacity(1.0);

    rend->render(&p, rect());
    
    QTextOption opt;
    opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    opt.setAlignment(Qt::AlignCenter);

    QFont f = font();
    f.setBold(true);
    p.setFont(f);
    QRect inner = rect().adjusted(hpad, vpad, -hpad, -vpad);
    p.drawText(inner, text(), opt);
}

