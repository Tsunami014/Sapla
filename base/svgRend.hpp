#pragma once
#include <QColor>
#include <QByteArray>

using _colourMapTyp = QMap<QString, QString>;

extern _colourMapTyp pallete;
QString getCol(QString palleteCol, int avgOffs = 0, int offset = 0, int diff = 5);
QColor getQCol(QString palleteCol, int avgOffs = 0, int offset = 0, int diff = 5);

extern _colourMapTyp colours;
void initColours();


QByteArray RenderSvg(const QString& svgPath);

