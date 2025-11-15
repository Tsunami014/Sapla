#pragma once
#include <QByteArray>

using _colourMapTyp = QMap<QString, QString>;

extern _colourMapTyp pallete;
QString getCol(QString palleteCol, std::tuple<int, int, int> offsets, int diff = 5);

extern _colourMapTyp colours;
void initColours();


QByteArray RenderSvg(const QString& svgPath);

