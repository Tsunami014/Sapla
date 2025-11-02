#include "svgPic.hpp"

SvgPic::SvgPic(const QString& filePath, QWidget* parent) : SvgWidget(filePath, parent) {
    const auto box = renderer()->viewBoxF();
    if (!box.isEmpty()) {
        ratio = box.width() / box.height();
    } else {
        ratio = 1.0;
    }
}

void SvgPic::resizeEvent(QResizeEvent* event) {
    QSize sze = event->size();
    int nwid = sze.width();
    int nhei = static_cast<int>(nwid / ratio);

    if (nhei > sze.height()) {
        nhei = sze.height();
        nwid = static_cast<int>(nhei * ratio);
    }
    resize(nwid, nhei);
    SvgWidget::resizeEvent(event);
}

