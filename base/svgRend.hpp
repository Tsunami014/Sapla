#pragma once
#include <QByteArray>

using _colourMapTyp = QMap<QString, QString>;
extern _colourMapTyp colours;

QByteArray RenderSvg(const QString& svgPath);

