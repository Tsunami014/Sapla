#include "multiprogress.hpp"
#include "../core.hpp"
#include <QPainter>

MultiProgressBarItem::MultiProgressBarItem(QGraphicsItem* parent) : RectItem(parent), values() {
    setZValue(2);
}

void MultiProgressBarItem::setValues(std::vector<float> vs) {
    unsigned int tot = 0;
    std::vector<float> nvals;
    for (float v : vs) {
        float v2 = std::max(v, 0.0f);
        tot += v2;
        nvals.push_back(v2);
    }
    if (tot == 0) {
        if (values.empty()) return;
        values.clear();
        update();
        return;
    }
    values.clear();
    double tot2 = 0;
    for (auto v : nvals) {
        double val = v/tot;
        values.push_back(tot2 + val);
        tot2 += val;
    }
    update();
}

QPainterPath MultiProgressBarItem::shape() const {
    QPainterPath path;
    path.addRoundedRect(rect, 10, 10);
    return path;
}

void MultiProgressBarItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    QLinearGradient gradient(0, rect.top(), 0, rect.bottom());
    if (values.empty()) {
        gradient.setColorAt(0, MG->styls.pbcols[1]);
    } else {
        const double offs = 0.01;
        unsigned int idx = 0;
        size_t maxSze = MG->styls.pbcols.size()-3;
        qreal last = 0;
        for (auto v : values) {
            qreal start = qMax(v-offs, 0.0);
            qreal end = qMin(v+offs, 1.0);
            if (last != v) {
                last = v;
                if (start > 0 && start < 1) {
                    gradient.setColorAt(start, MG->styls.pbcols[idx+3]);
                }
            }
            idx = (idx + 1) % maxSze;
            if (end > 0 && end < 1) {
                gradient.setColorAt(end, MG->styls.pbcols[idx+3]);
            }
        }
    }

    auto path = shape();
    painter->fillPath(path, gradient);
    painter->setPen(QPen(QBrush(MG->styls.pbcols[0]), 2));
    painter->drawPath(path);
}
