#pragma once
#include "../base/svgWid.hpp"
#include <QSvgRenderer>
#include <QResizeEvent>

class SvgPic : public SvgWidget {
    Q_OBJECT

public:
    SvgPic(const QString& filePath, QWidget* parent = nullptr);
protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    double ratio;
};
